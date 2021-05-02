#include "ColorBackground.h"
#include "globals.h"
#include "tools.h"

#include <allegro5/allegro_primitives.h>

ColorBackground::ColorBackground(const std::string& overlay,
                                 int preset_name,
                                 double speed) {
  overlay_image = load_bitmap_ex(overlay.c_str());

  background_r = 0;
  background_g = 0;
  background_b = 0;
  background_r_up = false;
  background_g_up = false;
  background_b_up = false;

  setupColors(preset_name, speed);
}

// Setup colours for background
// For more vibrant colours, space out numbers farther e.g. 1, 125, 254
// For more pastel colours, keep numbers close together e.g. 1, 10, 20
void ColorBackground::setupColors(int preset_name, double newSpeed) {
  if (preset_name == BG_VIBRANT) {
    background_r = 1.0;
    background_g = 166.0;
    background_b = 77.0;
    background_r_up = background_g_up = true;
    background_b_up = false;
  } else if (preset_name == BG_GRAYSCALE) {
    background_r = 1.0;
    background_g = 1.0;
    background_b = 1.0;
    background_r_up = background_g_up = background_b_up = true;
  } else if (preset_name == BG_PASTEL) {
    background_r = 1.0;
    background_g = 40.0;
    background_b = 80.0;
    background_r_up = background_g_up = background_b_up = true;
  } else if (preset_name == BG_BALANCED) {
    background_r = 1.0;
    background_g = 100.0;
    background_b = 200.0;
    background_r_up = background_g_up = background_b_up = true;
  }
  background_speed = newSpeed;
}

// Change colours
void ColorBackground::changeColors() {
  // If colour value increase, increase by 1, else decrease by 1
  // When increasing,  coolbackground_r_up = true or 1, *2 -1 still equals 1
  // When increasing, background_r_up = false or 0, *2 -1 still equals -1
  background_r += (background_r_up * 2 - 1) * background_speed;
  background_g += (background_g_up * 2 - 1) * background_speed;
  background_b += (background_b_up * 2 - 1) * background_speed;

  // Start decreasing once it reaches the end
  if (background_r >= 255 || background_r <= 0)
    background_r_up = !background_r_up;
  if (background_g >= 255 || background_g <= 0)
    background_g_up = !background_g_up;
  if (background_b >= 255 || background_b <= 0)
    background_b_up = !background_b_up;
}

void ColorBackground::draw() {
  // Background
  al_draw_filled_rectangle(
      0, 0, SCREEN_W, SCREEN_H,
      al_map_rgb(background_r, background_g, background_b));

  // Overlay
  al_draw_scaled_bitmap(overlay_image, 0, 0, al_get_bitmap_width(overlay_image),
                        al_get_bitmap_height(overlay_image), 0, 0, SCREEN_W,
                        SCREEN_H, 0);
}
