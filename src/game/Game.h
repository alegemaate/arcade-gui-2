#pragma once

#include <string>
#include "../core/Bitmap.h"

/**
 * @brief Game structure
 *
 */
struct Game {
  /// @brief Path to game
  std::string path;

  /// @brief Name of game
  std::string name;

  /// @brief Icon of game
  Bitmap icon;
};