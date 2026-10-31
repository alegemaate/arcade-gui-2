#include "AdsGamesLoader.h"

#include <asw/asw.h>

#include <future>
#include <initializer_list>
#include <optional>
#include <vector>

#include "../Textures.h"
#include "../adsgames/Api.h"
#include "../adsgames/Http.h"

namespace fs = std::filesystem;

namespace {
constexpr const char* CATALOGUE_FILE = "games.json";
constexpr const char* DEFAULT_ICON = "assets/images/icons/game_1.png";

// Url of the first image of a type found, in order of preference
std::string image_url(const nlohmann::json& game,
                      std::initializer_list<const char*> types) {
  const auto images = game.value("gameImages", nlohmann::json::array());

  for (const auto* type : types) {
    for (const auto& image : images) {
      if (image.value("type", "") == type) {
        return image.value("file", "");
      }
    }
  }

  return "";
}

// Image bytes, from the cache or downloaded. Uploads never change, so the
// cache is kept forever.
std::optional<std::string> fetch_image(const std::string& url) {
  if (url.empty()) {
    return std::nullopt;
  }

  const auto path = adsgames::cache_dir() / "images" / fs::path(url).filename();

  if (auto cached = adsgames::read_file(path)) {
    return cached;
  }

  auto data = adsgames::http::get(url);
  if (data) {
    adsgames::write_file(path, *data);
  }

  return data;
}

// Make a texture from image bytes, on the main thread. Empty if it can not
// be read.
asw::Texture load_texture(const std::optional<std::string>& data) {
  return data ? textures::from_memory(*data) : nullptr;
}
}  // namespace

bool AdsGamesLoader::validate() {
  const auto cache = adsgames::cache_dir() / CATALOGUE_FILE;
  auto body = adsgames::http::get(std::string(adsgames::API_URL) +
                                  "/games?visible=true");

  if (body) {
    adsgames::write_file(cache, *body);
  } else {
    asw::log::warn("A.D.S. Games is not reachable, using saved game list");
    body = adsgames::read_file(cache);
  }

  if (!body) {
    return false;
  }

  catalogue = nlohmann::json::parse(*body, nullptr, false);
  return catalogue.is_array();
}

void AdsGamesLoader::load() {
  struct Entry {
    std::string name;
    Release release;
    std::future<std::optional<std::string>> image;
    std::future<std::optional<std::string>> backdrop;
  };

  // Fetch images side by side, textures are made after on this thread
  std::vector<Entry> entries;

  for (const auto& game : catalogue) {
    auto release = adsgames::latest_release(game);

    if (!release) {
      continue;
    }

    entries.push_back({
        .name = game.value("title", release->slug),
        .release = *release,
        // Large art stays sharp on the enlarged card
        .image = std::async(std::launch::async, fetch_image,
                            image_url(game, {"LARGE", "THUMBNAIL"})),
        .backdrop = std::async(std::launch::async, fetch_image,
                               image_url(game, {"WIDE", "LARGE"})),
    });
  }

  for (auto& entry : entries) {
    auto icon = load_texture(entry.image.get());
    if (!icon) {
      icon = asw::assets::load_texture(DEFAULT_ICON);
    }

    addGame({
        .name = entry.name,
        .path = "",
        .icon = icon,
        .release = entry.release,
        .subtitle = "A.D.S. Games  " + entry.release.version,
        .backdrop = load_texture(entry.backdrop.get()),
        .command = {},
    });
  }
}
