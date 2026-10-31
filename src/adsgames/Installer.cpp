#include "Installer.h"

#include <asw/asw.h>
#include <miniz.h>

#include <array>
#include <climits>
#include <exception>
#include <string>
#include <system_error>

#include "Api.h"
#include "Http.h"

namespace fs = std::filesystem;

namespace {
// Find the game in an install folder, the shallowest executable file
std::optional<fs::path> find_executable(const fs::path& dir) {
  std::optional<fs::path> found;
  int found_depth = INT_MAX;
  std::error_code error;

  for (auto it = fs::recursive_directory_iterator(dir, error);
       it != fs::recursive_directory_iterator(); it.increment(error)) {
    if (error) {
      break;
    }

    if (!it->is_regular_file() || it.depth() >= found_depth) {
      continue;
    }

    const auto& path = it->path();
    const auto extension = path.extension();

#ifdef _WIN32
    const bool executable = extension == ".exe";
#else
    const bool library = extension == ".so" || extension == ".dylib";
    const bool executable =
        !library &&
        (it->status().permissions() & fs::perms::owner_exec) != fs::perms::none;
#endif

    if (executable) {
      found = path;
      found_depth = it.depth();
    }
  }

  return found;
}

// Unzip an archive into a folder, keeping unix permissions
bool extract(const std::string& data, const fs::path& dest) {
  mz_zip_archive zip{};

  if (mz_zip_reader_init_mem(&zip, data.data(), data.size(), 0) == 0) {
    asw::log::error("Download is not a zip");
    return false;
  }

  bool ok = true;
  const mz_uint count = mz_zip_reader_get_num_files(&zip);

  for (mz_uint i = 0; i < count && ok; i++) {
    mz_zip_archive_file_stat stat{};
    std::array<char, MZ_ZIP_MAX_ARCHIVE_FILENAME_SIZE> name{};

    if (mz_zip_reader_file_stat(&zip, i, &stat) == 0 ||
        mz_zip_reader_get_filename(&zip, i, name.data(), name.size()) == 0) {
      ok = false;
      break;
    }

    // Refuse entries that would land outside the install folder
    const auto relative = fs::path(name.data()).lexically_normal();
    if (relative.empty() || relative.is_absolute() ||
        *relative.begin() == "..") {
      asw::log::error("Zip entry outside install folder: {}", name.data());
      ok = false;
      break;
    }

    const auto target = dest / relative;
    std::error_code error;

    if (mz_zip_reader_is_file_a_directory(&zip, i) != 0) {
      fs::create_directories(target, error);
      continue;
    }

    fs::create_directories(target.parent_path(), error);

    if (mz_zip_reader_extract_to_file(&zip, i, target.string().c_str(), 0) ==
        0) {
      asw::log::error("Could not unzip {}", name.data());
      ok = false;
      break;
    }

#ifndef _WIN32
    // Zips made on unix keep the mode in the high bits
    constexpr unsigned MODE_SHIFT = 16U;
    constexpr unsigned MODE_MASK = 0777U;
    const auto mode = (stat.m_external_attr >> MODE_SHIFT) & MODE_MASK;
    if (mode != 0) {
      fs::permissions(target, static_cast<fs::perms>(mode),
                      fs::perm_options::replace, error);
    }
#endif
  }

  mz_zip_reader_end(&zip);
  return ok;
}

// Ask the API for the newest release, the one we have if it can not answer
Release resolve(const Release& fallback) {
  const auto body = adsgames::http::get(std::string(adsgames::API_URL) +
                                        "/games/" + fallback.game_id);

  if (!body) {
    return fallback;
  }

  const auto game = nlohmann::json::parse(*body, nullptr, false);
  if (game.is_discarded()) {
    return fallback;
  }

  return adsgames::latest_release(game).value_or(fallback);
}
}  // namespace

adsgames::Installer::~Installer() {
  cancelled = true;
  if (worker.joinable()) {
    worker.join();
  }
}

bool adsgames::Installer::isBusy() const {
  const auto current = state.load();
  return current == State::Downloading || current == State::Installing;
}

void adsgames::Installer::start(const Release& release) {
  if (isBusy()) {
    return;
  }

  // The last worker has finished
  if (worker.joinable()) {
    worker.join();
  }

  progress = 0.0F;
  state = State::Downloading;
  // A disk or memory error fails the install instead of the program. The
  // check can not see that everything is caught.
  // NOLINTNEXTLINE(bugprone-exception-escape)
  worker = std::thread([this, release] {
    try {
      run(release);
    } catch (const std::exception& e) {
      state = State::Failed;
      asw::log::error("Install failed: {}", e.what());
    } catch (...) {
      state = State::Failed;
    }
  });
}

std::optional<fs::path> adsgames::Installer::takeExecutable() {
  if (state.load() != State::Ready) {
    return std::nullopt;
  }

  state = State::Idle;
  return executable;
}

void adsgames::Installer::run(const Release& requested) {
  const auto release = resolve(requested);
  const auto dir = install_dir(release);

  // Installed already
  if (fs::exists(dir)) {
    if (auto found = find_executable(dir)) {
      asw::log::info("{} {} is installed", release.slug, release.version);
      executable = *found;
      state = State::Ready;
      return;
    }
  }

  asw::log::info("Downloading {}", release.url);
  const auto data =
      http::get(release.url, [this](uint64_t received, uint64_t total) {
        if (total > 0) {
          progress = static_cast<float>(received) / static_cast<float>(total);
        }
        return !cancelled.load();
      });

  if (!data) {
    state = State::Failed;
    return;
  }

  // Unzip next to the install, so a crash never leaves half a game behind
  state = State::Installing;
  auto partial = dir;
  partial += ".partial";

  std::error_code error;
  fs::remove_all(partial, error);
  fs::remove_all(dir, error);

  if (!extract(*data, partial)) {
    fs::remove_all(partial, error);
    state = State::Failed;
    return;
  }

  fs::rename(partial, dir, error);
  if (error) {
    asw::log::error("Could not install {}: {}", release.slug, error.message());
    state = State::Failed;
    return;
  }

  const auto found = find_executable(dir);
  if (!found) {
    asw::log::error("No executable in {}", release.url);
    state = State::Failed;
    return;
  }

  asw::log::info("Installed {} {}", release.slug, release.version);
  executable = *found;
  state = State::Ready;
}
