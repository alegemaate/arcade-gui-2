#pragma once

#include <string>
#include <vector>

#include "../game/Game.h"

/**
 * @brief Loader Plugin Interface
 *
 */
class LoaderPlugin {
 public:
  LoaderPlugin() = default;
  LoaderPlugin(const LoaderPlugin&) = default;
  LoaderPlugin& operator=(const LoaderPlugin&) = default;
  LoaderPlugin(LoaderPlugin&&) = default;
  LoaderPlugin& operator=(LoaderPlugin&&) = default;
  virtual ~LoaderPlugin() = default;

  /**
   * @brief Validate the plugin
   *
   * @return true - Configuration is valid, plugin can be used
   * @return false - Configuration is invalid, plugin cannot be used
   */
  virtual bool validate() = 0;

  /**
   * @brief Load plugin data into games vector
   *
   */
  virtual void load() = 0;

  /**
   * @brief Revalidate plugin data
   *
   */
  virtual void revalidate() = 0;

  /**
   * @brief Get name
   *
   */
  virtual std::string getName() const = 0;

  /**
   * @brief Pick up games found in the background, called each frame
   *
   * @return true - If the list of games changed
   */
  virtual bool update() { return false; }

  /**
   * @brief List games loaded by plugin
   *
   * @return const std::vector<Game>& - List of games
   */
  const std::vector<Game>& listGames() const { return games; }

 protected:
  /**
   * @brief Add a game to the list of loaded games
   *
   * @param game - Game to add
   */
  void addGame(Game game) { games.push_back(std::move(game)); }

  /**
   * @brief Remove every loaded game
   *
   */
  void clearGames() { games.clear(); }

 private:
  /// @brief List of games loaded by plugin
  std::vector<Game> games;
};
