#include "Settings.h"

#include <SDL3/SDL.h>

#include <nlohmann/json.hpp>

#include "adsgames/Api.h"

namespace {
constexpr const char* SETTINGS_FILE = "settings.json";
}  // namespace

Settings Settings::load() {
  Settings settings;
  const auto data = adsgames::read_file(adsgames::cache_dir() / SETTINGS_FILE);

  if (!data) {
    return settings;
  }

  const auto doc = nlohmann::json::parse(*data, nullptr, false);
  if (doc.is_object()) {
    settings.mame_rom_path = doc.value("mameRomPath", "");
  }

  return settings;
}

bool Settings::save() const {
  const nlohmann::json doc = {{"mameRomPath", mame_rom_path}};
  return adsgames::write_file(adsgames::cache_dir() / SETTINGS_FILE,
                              doc.dump(2));
}

std::string expand_home(const std::string& path) {
  if (path.empty() || path.front() != '~') {
    return path;
  }

  const char* home = SDL_GetEnvironmentVariable(SDL_GetEnvironment(), "HOME");
#ifdef _WIN32
  if (home == nullptr) {
    home = SDL_GetEnvironmentVariable(SDL_GetEnvironment(), "USERPROFILE");
  }
#endif

  if (home == nullptr) {
    return path;
  }

  return std::string(home) + path.substr(1);
}
