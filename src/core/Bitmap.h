#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "./Vec2.h"

/**
 * @brief A wrapper around ALLEGRO_BITMAP
 *
 */
class Bitmap {
 public:
  /**
   * @brief Construct a new Bitmap object
   * @details Default consturctor
   *
   * @param dimensions - Dimensions of bitmap
   */
  Bitmap() {
    bitmap = nullptr;
    dimensions.x = 0;
    dimensions.y = 0;
  }

  /**
   * @brief Construct a new Bitmap object
   * @details Creates a bitmap of a given width and height and manages its
   * memory
   *
   * @param width - Width of bitmap
   * @param height - Height of bitmap
   */
  Bitmap(int width, int height) {
    bitmap = std::shared_ptr<ALLEGRO_BITMAP>(al_create_bitmap(width, height),
                                             al_destroy_bitmap);
    if (!bitmap) {
      throw std::runtime_error("Failed to create bitmap");
    }

    dimensions.x = width;
    dimensions.y = height;
  }

  /**
   * @brief Construct a new Bitmap object
   * @details Loads a bitmap from a file path and manages its memory
   *
   * @param path - File path to bitmap
   */
  Bitmap(const std::string& path) {
    bitmap = std::shared_ptr<ALLEGRO_BITMAP>(al_load_bitmap(path.c_str()),
                                             al_destroy_bitmap);
    if (!bitmap) {
      throw std::runtime_error("Failed to load bitmap: " + path);
    }

    dimensions.x = al_get_bitmap_width(bitmap.get());
    dimensions.y = al_get_bitmap_height(bitmap.get());
  }

  /**
   * @brief Get the ALLEGRO_BITMAP object
   *
   * @return ALLEGRO_BITMAP* - The ALLEGRO_BITMAP object
   */
  ALLEGRO_BITMAP* get() { return bitmap.get(); }

  /**
   * @brief Get the Width of the Bitmap
   *
   * @return int - width
   */
  int getWidth() { return this->dimensions.x; }

  /**
   * @brief Get the Height of the Bitmap
   *
   * @return int - height
   */
  int getHeight() { return this->dimensions.y; }

  /**
   * @brief Get the Dimensions of the Bitmap
   *
   * @return Vec2<int> - dimensions
   */
  Vec2<int> getDimensions() { return this->dimensions; }

 private:
  /// @brief The ALLEGRO_BITMAP object
  std::shared_ptr<ALLEGRO_BITMAP> bitmap;

  /// @breif Dimensions
  Vec2<int> dimensions;
};