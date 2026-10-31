#include "MameLoader.h"

#include <asw/asw.h>

#include <filesystem>

#include "../Textures.h"
#include "../adsgames/Api.h"

namespace {
constexpr const char* MAME_ICON = "assets/images/icons/icon_mame.png";

// Load cached art, kept sharp since arcade screens are low resolution
asw::Texture load_art(const std::filesystem::path& path) {
  if (path.empty()) {
    return nullptr;
  }

  const auto data = adsgames::read_file(path);
  if (!data) {
    return nullptr;
  }

  auto texture = textures::from_memory(*data);
  if (texture) {
    asw::draw::set_scale_mode(texture, asw::ScaleMode::Nearest);
  }
  return texture;
}
}  // namespace

void MameLoader::load() {
  generation = library.getGeneration();
  clearGames();

  const auto fallback = asw::assets::load_texture(MAME_ICON);

  for (const auto& entry : library.getEntries()) {
    const auto title = load_art(entry.title_image);
    const auto snap = load_art(entry.snap_image);

    // Title screen on the card, gameplay behind it, either when one is missing
    auto icon = title ? title : snap;
    if (!icon) {
      icon = fallback;
    }

    addGame({
        .name = entry.title,
        .path = "",
        .icon = icon,
        .release = std::nullopt,
        .subtitle = "MAME  " + entry.name,
        .backdrop = snap ? snap : title,
        .command = library.launchArgs(entry.name),
    });
  }
}

bool MameLoader::update() {
  if (library.getGeneration() == generation) {
    return false;
  }

  load();
  return true;
}
