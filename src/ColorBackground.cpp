#include "ColorBackground.h"

#include <algorithm>
#include <cstdint>

#include "globals.h"

namespace {
// Color units moved per second
constexpr float COLOR_SPEED = 60.0F;
constexpr float COLOR_MAX = 255.0F;

// Reverse a component once it reaches the end of the range
void bounce(float value, float& direction) {
  if (value >= COLOR_MAX || value <= 0) {
    direction = -direction;
  }
}

uint8_t to_channel(float value) {
  return static_cast<uint8_t>(std::clamp(value, 0.0F, COLOR_MAX));
}
}  // namespace

void ColorBackground::setPreset(const ColorBackgroundPreset& preset) {
  color = preset.color;
  color_direction = preset.color_direction;
  overlay_image = preset.overlay_image;
}

void ColorBackground::changeColors(float dt) {
  color += color_direction * (COLOR_SPEED * dt);

  bounce(color.x, color_direction.x);
  bounce(color.y, color_direction.y);
  bounce(color.z, color_direction.z);
}

void ColorBackground::draw() const {
  const asw::Quadf screen(0, 0, SCREEN_W, SCREEN_H);

  // Background
  asw::draw::rect_fill(screen,
                       asw::Color(to_channel(color.x), to_channel(color.y),
                                  to_channel(color.z)));

  // Overlay
  asw::draw::stretch_sprite(overlay_image, screen);
}
