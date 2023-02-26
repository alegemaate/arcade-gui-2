#include "ColorBackground.h"
#include "globals.h"
#include "util/tools.h"

#include <allegro5/allegro_primitives.h>

ColorBackground::ColorBackground()
    : color(0, 0, 0), color_direction(1.0, 1.0, 1.0) {}

void ColorBackground::setPreset(ColorBackgroundPreset preset) {
  color = preset.color;
  color_direction = preset.color_direction;
  overlay_image = preset.overlay_image;
}

// Change colours
void ColorBackground::changeColors() {
  // If colour value increase, increase by 1, else decrease by 1
  color +=
      Vec3<double>(color_direction.x, color_direction.y, color_direction.z);

  // Start decreasing once it reaches the end
  if (color.x >= 255 || color.x <= 0) {
    color_direction.x = -color_direction.x;
  }
  if (color.y >= 255 || color.y <= 0) {
    color_direction.y = -color_direction.y;
  }
  if (color.z >= 255 || color.z <= 0) {
    color_direction.z = -color_direction.z;
  }
}

void ColorBackground::draw() {
  // Background
  al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H,
                           al_map_rgb(color.x, color.y, color.z));

  // Overlay
  al_draw_scaled_bitmap(overlay_image.get(), 0, 0, overlay_image.getWidth(),
                        overlay_image.getHeight(), 0, 0, SCREEN_W, SCREEN_H, 0);
}
