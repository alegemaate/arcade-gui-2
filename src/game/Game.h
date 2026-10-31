#pragma once

#include <asw/asw.h>

#include <optional>
#include <string>
#include <vector>

/**
 * @brief A downloadable build of a game on A.D.S. Games
 *
 */
struct Release {
  /// @brief A.D.S. Games id of the game
  std::string game_id;

  /// @brief Short name of the game, e.g. "mini-jim"
  std::string slug;

  /// @brief Version, e.g. "v1.2.0"
  std::string version;

  /// @brief Zip of the build
  std::string url;
};

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
  asw::Texture icon;

  /// @brief Build to download, set instead of path for A.D.S. Games games
  std::optional<Release> release;

  /// @brief Line under the name, e.g. the version
  std::string subtitle;

  /// @brief Wide art shown behind the menu, may be empty
  asw::Texture backdrop;

  /// @brief Program and arguments that run the game, e.g. for MAME
  std::vector<std::string> command;
};
