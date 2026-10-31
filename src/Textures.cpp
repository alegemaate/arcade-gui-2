#include "Textures.h"

#include <SDL3_image/SDL_image.h>

#include <array>
#include <cstddef>
#include <cstdint>

namespace {
// Same as asw, the renderer may be gone at shutdown
asw::Texture wrap(SDL_Texture* texture) {
  if (texture == nullptr) {
    return nullptr;
  }

  return {texture, [](SDL_Texture* tex) {
            if (asw::display::get_renderer() != nullptr) {
              SDL_DestroyTexture(tex);
            }
          }};
}
}  // namespace

asw::Texture textures::from_memory(const std::string& data) {
  SDL_IOStream* stream = SDL_IOFromConstMem(data.data(), data.size());
  SDL_Texture* texture =
      IMG_LoadTexture_IO(asw::display::get_renderer(), stream, true);

  // Jpegs have no alpha, so SDL would ignore set_alpha without this
  if (texture != nullptr) {
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  }

  return wrap(texture);
}

asw::Texture textures::create_fade() {
  constexpr int HEIGHT = 256;
  constexpr int CHANNELS = 4;
  constexpr int ALPHA = 3;

  // One column of black pixels, alpha rising one step per row
  std::array<uint8_t, static_cast<std::size_t>(HEIGHT) * CHANNELS> pixels{};
  for (int y = 0; y < HEIGHT; y++) {
    pixels.at((static_cast<std::size_t>(y) * CHANNELS) + ALPHA) =
        static_cast<uint8_t>(y);
  }

  SDL_Texture* texture =
      SDL_CreateTexture(asw::display::get_renderer(), SDL_PIXELFORMAT_RGBA32,
                        SDL_TEXTUREACCESS_STATIC, 1, HEIGHT);
  if (texture == nullptr) {
    return nullptr;
  }

  SDL_UpdateTexture(texture, nullptr, pixels.data(), CHANNELS);
  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_LINEAR);

  return wrap(texture);
}
