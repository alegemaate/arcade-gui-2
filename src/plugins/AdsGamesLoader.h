#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "LoaderPlugin.h"

/**
 * @brief Loads games released on A.D.S. Games. Games download when launched.
 *
 */
class AdsGamesLoader : public LoaderPlugin {
 public:
  /**
   * @brief Fetch the game list, or use the last one saved when offline
   *
   * @return true - if a game list is available
   * @return false - if offline and nothing is saved
   */
  bool validate() override;

  /**
   * @brief Add each game with a release for this platform
   *
   */
  void load() override;

  /**
   * @brief Revalidate plugin data, does nothing
   *
   */
  void revalidate() override {}

  /**
   * @brief Get name
   *
   * @return std::string - Name of plugin
   */
  std::string getName() const override { return "AdsGamesLoader"; }

 private:
  /// @brief Games from the API
  nlohmann::json catalogue;
};
