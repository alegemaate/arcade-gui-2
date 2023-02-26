#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "./Vec2.h"

/**
 * @brief A wrapper around ALLEGRO_FONT
 *
 */
class Font {
 public:
  /**
   * @brief Construct a new Font object
   * @details Default consturctor
   *
   * @param dimensions - Dimensions of font
   */
  Font() { font = nullptr; }

  /**
   * @brief Construct a new Font object
   * @details Loads a font from a file path and manages its memory
   *
   * @param path - File path to font
   */
  Font(const std::string& path, int size) {
    font = std::shared_ptr<ALLEGRO_FONT>(
        al_load_ttf_font(path.c_str(), size, 0), al_destroy_font);

    if (!font) {
      throw std::runtime_error("Failed to load font: " + path);
    }
  }

  /**
   * @brief Get the ALLEGRO_FONT object
   *
   * @return ALLEGRO_FONT* - The ALLEGRO_FONT object
   */
  ALLEGRO_FONT* get() { return font.get(); }

 private:
  /// @brief The ALLEGRO_FONT object
  std::shared_ptr<ALLEGRO_FONT> font;
};