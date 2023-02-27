#pragma once

#include <string>
#include <vector>

#include "../game/Game.h"
#include "../util/Logger.h"

/**
 * @brief Loader Plugin Interface
 *
 */
class LoaderPlugin {
 public:
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
  virtual std::string getName() = 0;

  /**
   * @brief List games loaded by plugin
   *
   * @return std::vector<Game> - List of games
   */
  std::vector<Game> listGames() { return games; }

 protected:
  /// @brief List of games loaded by plugin
  std::vector<Game> games{};

  /// @breif Logger instance
  Logger logger = Logger("LoaderPlugin");
};
