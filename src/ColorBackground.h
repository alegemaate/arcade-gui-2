#pragma once

#include <string>

#include "core/Bitmap.h"
#include "core/Vec3.h"

struct ColorBackgroundPreset {
  Vec3<double> color;
  Vec3<double> color_direction;
  Bitmap overlay_image;
};

class ColorBackground {
 public:
  /**
   * @brief Construct a new Color Background object
   *
   */
  ColorBackground();

  /**
   * @brief Setup the colors of the background.
   *
   * @param preset - Color preset
   */
  void setPreset(ColorBackgroundPreset preset);

  /**
   * @brief Change the colors of the background.
   *
   */
  void changeColors();

  /**
   * @brief Draw the background.
   *
   */
  void draw();

 private:
  /// @brief The current color of the background.
  Vec3<double> color;

  /// @brief The direction the color components are moving.
  Vec3<double> color_direction;

  /// @brief The image to overlay on the background. (vignetted)
  Bitmap overlay_image;
};
