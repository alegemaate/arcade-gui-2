#pragma once

#include <string>
#include "../core/Bitmap.h"

/**
 * @brief Game structure
 *
 */
struct Game {
  /// @brief Name of game
  std::string name;

  /// @brief Path to game
  std::string path;

  /// @brief Icon of game
  Bitmap icon;
};