#pragma once

#include <asw/asw.h>

#include <memory>
#include <vector>

#include "../plugins/LoaderPlugin.h"
#include "Game.h"

class GameDirectory {
 public:
  /**
   * @brief Loads a plugin into the game directory
   *
   * @param plugin - Plugin to load
   */
  void loadPlugin(const std::shared_ptr<LoaderPlugin>& plugin) {
    asw::log::info("Loading plugin: {}", plugin->getName());

    if (!plugin->validate()) {
      asw::log::info("Plugin is not valid, skipping");
      return;
    }

    asw::log::info("Plugin is valid, loading");
    plugin->load();
    plugins.push_back(plugin);
    rebuild();
  }

  /**
   * @brief Adds a game after the games from plugins
   *
   * @param game - Game to add
   */
  void addGame(const Game& game) {
    extras.push_back(game);
    rebuild();
  }

  /**
   * @brief Pick up games plugins found in the background
   *
   * @return true - If the list of games changed
   */
  bool update() {
    bool changed = false;
    for (const auto& plugin : plugins) {
      changed = plugin->update() || changed;
    }

    if (changed) {
      rebuild();
    }

    return changed;
  }

  /**
   * @brief Lists all games in the game directory
   *
   * @return const std::vector<Game>& - List of games
   */
  const std::vector<Game>& listGames() const { return games; }

 private:
  /**
   * @brief Gather the games from every plugin, then the extras
   *
   */
  void rebuild() {
    games.clear();

    for (const auto& plugin : plugins) {
      const auto& list = plugin->listGames();
      games.insert(games.end(), list.begin(), list.end());
    }

    games.insert(games.end(), extras.begin(), extras.end());
  }

  /// @brief List of games in the game directory
  std::vector<Game> games;

  /// @brief Games added directly, e.g. the joystick test
  std::vector<Game> extras;

  /// @brief List of plugins loaded into the game directory
  std::vector<std::shared_ptr<LoaderPlugin>> plugins;
};
