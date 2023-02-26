#pragma once

#include <string>

#include "core/Bitmap.h"
#include "core/Vec3.h"

enum class ColorPreset { VIBRANT, GRAYSCALE, PASTEL, BALANCED };

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
   * @brief Construct a new Color Background object
   *
   * @param overlay - Path to the overlay image
   * @param preset - Color preset
   * @param speed - Speed at which the colors change
   */
  ColorBackground(ColorBackgroundPreset preset);

  /**
   * @brief Setup the colors of the background.
   *
   * @param preset - Color preset
   * @param speed - Speed at which the colors change
   */
  void setupColors(ColorPreset preset, double speed);

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
