#pragma once

#include <memory>
#include <vector>

#include "../plugins/LoaderPlugin.h"
#include "../util/Logger.h"
#include "Game.h"

class GameDirectory {
 public:
  /**
   * @brief Loads a plugin into the game directory
   *
   * @param plugin - Plugin to load
   */
  void loadPlugin(std::shared_ptr<LoaderPlugin> plugin) {
    logger.log("Loading plugin: " + plugin->getName());

    if (plugin->validate()) {
      logger.log("Plugin is valid, loading");
      plugin->load();
      plugins.push_back(std::move(plugin));
    } else {
      logger.log("Plugin is not valid, skipping");
    }

    for (auto const& game : plugins.back()->listGames()) {
      logger.log("Adding game: " + game.name);
      addGame(game);
    }
  }

  /**
   * @brief Adds a game to the game directory
   *
   * @param game - Game to add
   */
  void addGame(Game game) { games.push_back(game); }

  /**
   * @brief Lists all games in the game directory
   *
   * @return std::vector<Game> - List of games
   */
  std::vector<Game> listGames() { return games; }

 private:
  /// @brief List of games in the game directory
  std::vector<Game> games{};

  /// @brief List of plugins loaded into the game directory
  std::vector<std::shared_ptr<LoaderPlugin>> plugins{};

  /// @brief Logger instance
  Logger logger = Logger("GameDirectory");
};