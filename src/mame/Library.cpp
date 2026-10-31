#include "Library.h"

#include <asw/asw.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <functional>
#include <future>
#include <nlohmann/json.hpp>
#include <optional>
#include <sstream>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>

#include "../Settings.h"
#include "../adsgames/Api.h"
#include "../adsgames/Http.h"

namespace fs = std::filesystem;

namespace {
constexpr const char* LIBRARY_FILE = "library.json";

// Art from the libretro thumbnail sets, named after the full title
constexpr const char* ART_URL =
    "https://raw.githubusercontent.com/libretro-thumbnails/MAME/master/";
constexpr const char* TITLES = "Named_Titles";
constexpr const char* SNAPS = "Named_Snaps";

// Downloads at once while fetching art
constexpr std::size_t ART_BATCH = 8;

// MAME's own folders it would write to the current folder, e.g. cfg
constexpr std::array<std::pair<const char*, const char*>, 8> WRITE_FOLDERS = {
    {
        {"-cfg_directory", "cfg"},
        {"-nvram_directory", "nvram"},
        {"-input_directory", "inp"},
        {"-state_directory", "sta"},
        {"-snapshot_directory", "snap"},
        {"-diff_directory", "diff"},
        {"-comment_directory", "comments"},
        {"-share_directory", "share"},
    },
};

fs::path mame_dir() {
  return adsgames::cache_dir() / "mame";
}

std::string trim(const std::string& value) {
  const auto start = value.find_first_not_of(" \t\r\n\"");
  if (start == std::string::npos) {
    return "";
  }
  const auto end = value.find_last_not_of(" \t\r\n\"");
  return value.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& value, char separator) {
  std::vector<std::string> parts;
  std::stringstream stream(value);
  std::string part;

  while (std::getline(stream, part, separator)) {
    part = trim(part);
    if (!part.empty()) {
      parts.push_back(part);
    }
  }

  return parts;
}

// Run a program and read its output, for quick commands on the main thread
std::optional<std::string> run(const std::vector<std::string>& args) {
  std::vector<const char*> argv;
  argv.reserve(args.size() + 1);
  for (const auto& arg : args) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  SDL_Process* process = SDL_CreateProcess(argv.data(), true);
  if (process == nullptr) {
    return std::nullopt;
  }

  std::size_t size = 0;
  int exit_code = 0;
  void* data = SDL_ReadProcess(process, &size, &exit_code);
  SDL_DestroyProcess(process);

  if (data == nullptr) {
    return std::nullopt;
  }

  std::string output(static_cast<const char*>(data), size);
  SDL_free(data);
  return output;
}

// Places MAME is often installed, PATH first. Apps started from the desktop
// may not have the shell's PATH.
std::vector<std::string> executable_candidates() {
#ifdef _WIN32
  return {"mame.exe", "mame"};
#else
  return {
      "mame",
      "/opt/homebrew/bin/mame",
      "/usr/local/bin/mame",
      "/usr/games/mame",
      "/usr/bin/mame",
  };
#endif
}

// Folders MAME searches for roms, relative ones from the current folder
std::vector<fs::path> rom_folders(const std::string& executable,
                                  const std::string& configured) {
  std::string rompath = configured;

  if (rompath.empty()) {
    const auto config = run({executable, "-showconfig"});
    std::stringstream lines(config.value_or(""));
    std::string line;

    while (std::getline(lines, line)) {
      if (line.starts_with("rompath ")) {
        rompath = trim(line.substr(std::string("rompath").size()));
        break;
      }
    }
  }

  std::vector<fs::path> folders;
  std::error_code error;
  for (const auto& part : split(rompath, ';')) {
    folders.push_back(fs::absolute(expand_home(part), error));
  }

  return folders;
}

// Changes when MAME or anything in the rom folders changes
std::string signature(const std::string& version,
                      const std::vector<fs::path>& folders) {
  std::string key = version;

  for (const auto& folder : folders) {
    key += "|" + folder.string();

    std::vector<std::string> files;
    std::error_code error;
    for (const auto& item : fs::directory_iterator(folder, error)) {
      const auto size = item.is_regular_file(error) ? item.file_size(error) : 0;
      // The count can be 128 bits, more than to_string takes
      const auto time = static_cast<long long>(
          item.last_write_time(error).time_since_epoch().count());
      files.push_back(item.path().filename().string() + ":" +
                      std::to_string(size) + ":" + std::to_string(time));
    }

    std::ranges::sort(files);
    for (const auto& file : files) {
      key += "|" + file;
    }
  }

  return std::to_string(std::hash<std::string>{}(key));
}

// libretro names files after the title, with characters files can not hold
// swapped for _
std::string art_file(const std::string& title) {
  std::string name = title;
  for (char& c : name) {
    if (std::string_view("&*/:`<>?\\|\"").find(c) != std::string_view::npos) {
      c = '_';
    }
  }
  return name + ".png";
}

std::string url_encode(const std::string& value) {
  constexpr std::string_view HEX = "0123456789ABCDEF";
  constexpr unsigned HIGH_SHIFT = 4U;
  constexpr unsigned LOW_MASK = 0x0FU;

  std::string encoded;
  for (const char c : value) {
    const auto byte = static_cast<unsigned char>(c);
    if (std::isalnum(byte) != 0 ||
        std::string_view("-_.~()!,'").find(c) != std::string_view::npos) {
      encoded += c;
    } else {
      encoded += '%';
      encoded += HEX.at(byte >> HIGH_SHIFT);
      encoded += HEX.at(byte & LOW_MASK);
    }
  }
  return encoded;
}

// Image for a title, from the cache or downloaded. Empty if there is none.
fs::path fetch_art(const char* kind, const std::string& title) {
  const auto file = art_file(title);
  auto path = mame_dir() / "images" / kind / file;

  std::error_code error;
  if (fs::exists(path, error)) {
    return path;
  }

  const auto data =
      adsgames::http::get(std::string(ART_URL) + kind + "/" + url_encode(file));
  if (!data || !adsgames::write_file(path, *data)) {
    return {};
  }

  return path;
}

// Check if a line holds some text
bool has(const std::string& line, std::string_view text) {
  return line.find(text) != std::string::npos;
}

// Rom sets from -verifyroms output with a status, e.g.
// "romset pacman [puckman] is good"
std::vector<std::string> parse_status(const std::string& output,
                                      const std::string& status) {
  std::vector<std::string> names;
  std::stringstream lines(output);
  std::string line;
  const auto suffix = " is " + status;

  while (std::getline(lines, line)) {
    if (!line.starts_with("romset ") || !trim(line).ends_with(suffix)) {
      continue;
    }

    std::stringstream words(line);
    std::string word;
    std::string name;
    words >> word >> name;
    names.push_back(name);
  }

  return names;
}

// Files each rom set is missing, from -verifyroms output, e.g.
// "mslug2      : sp1-u4.bin (131072 bytes) - NOT FOUND (neogeo)". Files with
// no good dump known are left out, MAME runs without them.
std::unordered_map<std::string, std::vector<std::string>> parse_missing(
    const std::string& output) {
  std::unordered_map<std::string, std::vector<std::string>> missing;
  std::stringstream lines(output);
  std::string line;

  while (std::getline(lines, line)) {
    const auto colon = line.find(" : ");
    if (colon == std::string::npos || !has(line, "NOT FOUND") ||
        has(line, "NO GOOD DUMP KNOWN")) {
      continue;
    }

    const auto rest = line.substr(colon + 3);
    const auto file = rest.substr(0, rest.find(" ("));
    missing[trim(line.substr(0, colon))].push_back(trim(file));
  }

  return missing;
}

// Value of an attribute in one line of -listxml output, empty if missing
std::string xml_attr(const std::string& line, const std::string& name) {
  const auto key = " " + name + "=\"";
  const auto start = line.find(key);
  if (start == std::string::npos) {
    return "";
  }

  const auto value = start + key.size();
  return line.substr(value, line.find('"', value) - value);
}

// BIOS options of a rom set, and the BIOS each of its files belongs to
struct BiosInfo {
  std::string default_bios;
  std::unordered_map<std::string, std::string> file_bios;
};

// From -listxml output, which has one element per line
std::unordered_map<std::string, BiosInfo> parse_bios(const std::string& xml) {
  std::unordered_map<std::string, BiosInfo> machines;
  std::stringstream lines(xml);
  std::string line;
  BiosInfo* current = nullptr;
  bool has_default = false;

  while (std::getline(lines, line)) {
    if (has(line, "<machine ")) {
      current = &machines[xml_attr(line, "name")];
      has_default = false;
    } else if (current != nullptr && has(line, "<biosset ")) {
      // The one marked default, or else the first
      if (xml_attr(line, "default") == "yes" ||
          (!has_default && current->default_bios.empty())) {
        current->default_bios = xml_attr(line, "name");
        has_default = xml_attr(line, "default") == "yes";
      }
    } else if (current != nullptr && has(line, "<rom ")) {
      current->file_bios[xml_attr(line, "name")] = xml_attr(line, "bios");
    }
  }

  return machines;
}

// Rom sets MAME calls bad that still run, where every missing file belongs
// to a BIOS the set does not use. Neo Geo sets list every BIOS, so one
// with only its default BIOS is bad but plays.
std::vector<std::string> playable_bad(
    const std::vector<std::string>& bad,
    const std::unordered_map<std::string, std::vector<std::string>>& missing,
    const std::unordered_map<std::string, BiosInfo>& machines) {
  std::vector<std::string> playable;

  for (const auto& name : bad) {
    const auto set_missing = missing.find(name);
    const auto machine = machines.find(name);
    if (set_missing == missing.end() || machine == machines.end()) {
      continue;
    }

    const auto& info = machine->second;
    const bool only_other_bios =
        std::ranges::all_of(set_missing->second, [&info](const auto& file) {
          const auto bios = info.file_bios.find(file);
          return bios != info.file_bios.end() && !bios->second.empty() &&
                 bios->second != info.default_bios;
        });

    if (only_other_bios) {
      playable.push_back(name);
    }
  }

  return playable;
}

// Titles from -listfull output, e.g. pacman "Pac-Man (Midway)"
std::unordered_map<std::string, std::string> parse_titles(
    const std::string& output) {
  std::unordered_map<std::string, std::string> titles;
  std::stringstream lines(output);
  std::string line;

  while (std::getline(lines, line)) {
    const auto quote = line.find('"');
    if (quote == std::string::npos) {
      continue;
    }

    titles[trim(line.substr(0, quote))] = trim(line.substr(quote));
  }

  return titles;
}

nlohmann::json to_json(const std::string& signature,
                       const std::vector<mame::Entry>& list) {
  auto games = nlohmann::json::array();
  for (const auto& entry : list) {
    games.push_back({
        {"name", entry.name},
        {"title", entry.title},
        {"titleImage", entry.title_image.string()},
        {"snapImage", entry.snap_image.string()},
    });
  }
  return {{"signature", signature}, {"games", games}};
}

std::vector<mame::Entry> from_json(const nlohmann::json& doc) {
  std::vector<mame::Entry> list;
  for (const auto& game : doc.value("games", nlohmann::json::array())) {
    list.push_back({
        .name = game.value("name", ""),
        .title = game.value("title", ""),
        .title_image = game.value("titleImage", ""),
        .snap_image = game.value("snapImage", ""),
    });
  }
  return list;
}
}  // namespace

mame::Library::~Library() {
  stop();
}

void mame::Library::stop() {
  cancelled = true;

  {
    const std::scoped_lock lock(mutex);
    if (running != nullptr) {
      SDL_KillProcess(running, true);
    }
  }

  if (worker.joinable()) {
    worker.join();
  }

  cancelled = false;
}

void mame::Library::clearCache() {
  std::error_code error;
  fs::remove(mame_dir() / LIBRARY_FILE, error);
}

std::vector<mame::Entry> mame::Library::getEntries() const {
  const std::scoped_lock lock(mutex);
  return entries;
}

void mame::Library::setEntries(std::vector<Entry> list) {
  std::ranges::sort(list, {}, &Entry::title);
  {
    const std::scoped_lock lock(mutex);
    entries = std::move(list);
  }
  generation++;
}

std::vector<std::string> mame::Library::launchArgs(
    const std::string& name) const {
  const auto base = mame_dir();
  std::vector<std::string> args = {executable};
  args.insert(args.end(), rom_args.begin(), rom_args.end());

  // Plugins keep their data in the home folder
  args.emplace_back("-homepath");
  args.push_back(base.string());

  for (const auto& [option, folder] : WRITE_FOLDERS) {
    args.emplace_back(option);
    args.push_back((base / folder).string());
  }

  args.push_back(name);
  return args;
}

void mame::Library::refresh(const std::string& rom_path) {
  if (executable.empty()) {
    for (const auto& candidate : executable_candidates()) {
      const auto output = run({candidate, "-version"});
      if (output && !trim(*output).empty()) {
        executable = candidate;
        version = trim(*output);
        asw::log::info("Found MAME {} at {}", version, executable);
        break;
      }
    }
  }

  if (executable.empty()) {
    asw::log::info("MAME is not installed");
    return;
  }

  const auto configured = expand_home(rom_path);
  const auto folders = rom_folders(executable, configured);
  const auto key = signature(version, folders);

  // Same folders as the list shown or being scanned
  if (key == current_signature) {
    return;
  }

  stop();
  current_signature = key;
  rom_args.clear();
  if (!configured.empty()) {
    rom_args = {"-rompath", configured};
  }

  // Saved list, shown while a stale one is scanned again
  const auto saved = adsgames::read_file(mame_dir() / LIBRARY_FILE);
  const auto doc = nlohmann::json::parse(saved.value_or(""), nullptr, false);
  if (doc.is_object()) {
    setEntries(from_json(doc));

    if (doc.value("signature", "") == key) {
      asw::log::info("MAME roms unchanged, using saved list");
      return;
    }
  }

  asw::log::info("Scanning MAME roms");
  scanning = true;
  // A disk or memory error ends the scan instead of the program. The check
  // can not see that everything is caught.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  worker = std::thread([this, key] {
    try {
      scan(key);
    } catch (const std::exception& e) {
      asw::log::error("MAME scan failed: {}", e.what());
    } catch (...) {
      asw::log::error("MAME scan failed");
    }
    scanning = false;
  });
}

std::optional<std::string> mame::Library::capture(
    const std::vector<std::string>& args) {
  std::vector<const char*> argv = {executable.c_str()};
  for (const auto& arg : args) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

  SDL_Process* process = SDL_CreateProcess(argv.data(), true);
  if (process == nullptr) {
    return std::nullopt;
  }

  {
    const std::scoped_lock lock(mutex);
    running = process;
  }

  // Stopped before the lock was taken
  if (cancelled) {
    SDL_KillProcess(process, true);
  }

  std::size_t size = 0;
  int exit_code = 0;
  void* data = SDL_ReadProcess(process, &size, &exit_code);

  {
    const std::scoped_lock lock(mutex);
    running = nullptr;
  }
  SDL_DestroyProcess(process);

  if (data == nullptr || cancelled) {
    SDL_free(data);
    return std::nullopt;
  }

  // Exits non zero when any set is incomplete, the output is still good
  std::string output(static_cast<const char*>(data), size);
  SDL_free(data);
  return output;
}

void mame::Library::scan(const std::string& signature) {
  auto verify_args = rom_args;
  verify_args.emplace_back("-verifyroms");

  const auto verified = capture(verify_args);
  if (!verified) {
    return;
  }

  auto names = parse_status(*verified, "good");

  // Bad sets that only miss other BIOS options still run
  const auto bad = parse_status(*verified, "bad");
  if (!bad.empty()) {
    std::vector<std::string> xml_args = {"-listxml"};
    xml_args.insert(xml_args.end(), bad.begin(), bad.end());

    const auto xml = capture(xml_args);
    if (!xml) {
      return;
    }

    const auto playable =
        playable_bad(bad, parse_missing(*verified), parse_bios(*xml));
    names.insert(names.end(), playable.begin(), playable.end());
  }

  asw::log::info("MAME found {} working rom sets", names.size());

  std::unordered_map<std::string, std::string> titles;
  if (!names.empty()) {
    std::vector<std::string> list_args = {"-listfull"};
    list_args.insert(list_args.end(), names.begin(), names.end());

    const auto listed = capture(list_args);
    if (!listed) {
      return;
    }
    titles = parse_titles(*listed);
  }

  std::vector<Entry> list;
  list.reserve(names.size());
  for (const auto& name : names) {
    const auto title = titles.find(name);
    list.push_back({
        .name = name,
        .title = title != titles.end() ? title->second : name,
        .title_image = {},
        .snap_image = {},
    });
  }

  // Art, a few downloads at a time
  for (std::size_t start = 0; start < list.size(); start += ART_BATCH) {
    if (cancelled) {
      return;
    }

    const auto end = std::min(list.size(), start + ART_BATCH);
    std::vector<std::future<std::pair<fs::path, fs::path>>> art;

    for (std::size_t i = start; i < end; i++) {
      art.push_back(std::async(std::launch::async, [title = list.at(i).title] {
        return std::pair{fetch_art(TITLES, title), fetch_art(SNAPS, title)};
      }));
    }

    for (std::size_t i = start; i < end; i++) {
      auto [title_image, snap_image] = art.at(i - start).get();
      list.at(i).title_image = std::move(title_image);
      list.at(i).snap_image = std::move(snap_image);
    }
  }

  adsgames::write_file(mame_dir() / LIBRARY_FILE,
                       to_json(signature, list).dump(2));
  setEntries(std::move(list));
}
