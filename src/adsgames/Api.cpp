#include "Api.h"

#include <asw/asw.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <vector>

namespace {
// Numbers in a version, e.g. "v1.10.2" is {1, 10, 2}
std::vector<int> version_numbers(const std::string& version) {
  constexpr int BASE = 10;
  std::vector<int> numbers;
  bool in_number = false;

  for (const char c : version) {
    if (std::isdigit(static_cast<unsigned char>(c)) == 0) {
      in_number = false;
      continue;
    }

    if (!in_number) {
      numbers.push_back(0);
      in_number = true;
    }

    numbers.back() = (numbers.back() * BASE) + (c - '0');
  }

  return numbers;
}
}  // namespace

const char* adsgames::platform() {
#ifdef _WIN32
  return "WINDOWS";
#elif defined(__APPLE__)
  return "MAC";
#else
  return "LINUX";
#endif
}

std::filesystem::path adsgames::cache_dir() {
  const std::filesystem::path path =
      asw::assets::get_save_path("adsgames", "arcade-gui");
  std::error_code error;
  std::filesystem::create_directories(path, error);
  return path;
}

std::filesystem::path adsgames::install_dir(const Release& release) {
  return cache_dir() / "games" / release.slug / release.version;
}

std::optional<Release> adsgames::latest_release(const nlohmann::json& game) {
  std::optional<Release> latest;
  std::vector<int> latest_numbers;

  for (const auto& item : game.value("releases", nlohmann::json::array())) {
    if (item.value("platform", "") != platform()) {
      continue;
    }

    Release release{
        .game_id = game.value("id", ""),
        .slug = game.value("slug", ""),
        .version = item.value("version", ""),
        .url = item.value("url", ""),
    };

    // The API sorts versions as text, so v1.10 would come before v1.9
    auto numbers = version_numbers(release.version);
    if (!latest || numbers > latest_numbers) {
      latest = std::move(release);
      latest_numbers = std::move(numbers);
    }
  }

  if (latest && (latest->game_id.empty() || latest->url.empty())) {
    return std::nullopt;
  }

  return latest;
}

std::optional<std::string> adsgames::read_file(
    const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);

  if (!file) {
    return std::nullopt;
  }

  return std::string(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>());
}

bool adsgames::write_file(const std::filesystem::path& path,
                          const std::string& data) {
  std::error_code error;
  std::filesystem::create_directories(path.parent_path(), error);

  std::ofstream file(path, std::ios::binary | std::ios::trunc);
  file.write(data.data(), static_cast<std::streamsize>(data.size()));

  return file.good();
}
