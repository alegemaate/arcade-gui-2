#pragma once

#include <asw/asw.h>

struct ColorBackgroundPreset {
  asw::Vec3f color;
  asw::Vec3f color_direction;
  asw::Texture overlay_image;
};

class ColorBackground {
 public:
  /**
   * @brief Setup the colors of the background.
   *
   * @param preset - Color preset
   */
  void setPreset(const ColorBackgroundPreset& preset);

  /**
   * @brief Change the colors of the background.
   *
   * @param dt - Time since last update in seconds
   */
  void changeColors(float dt);

  /**
   * @brief Draw the background.
   *
   */
  void draw() const;

 private:
  /// @brief The current color of the background.
  asw::Vec3f color{0.0F, 0.0F, 0.0F};

  /// @brief The direction the color components are moving.
  asw::Vec3f color_direction{1.0F, 1.0F, 1.0F};

  /// @brief The image to overlay on the background. (vignetted)
  asw::Texture overlay_image;
};
