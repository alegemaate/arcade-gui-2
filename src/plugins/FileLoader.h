#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../game/Game.h"
#include "../util/Logger.h"
#include "LoaderPlugin.h"

/**
 * @brief File Loader Plugin
 *
 */
class FileLoader : public LoaderPlugin {
 public:
  /**
   * @brief Construct a new File Loader object
   *
   */
  FileLoader() {}

  /**
   * @brief Destroy the File Loader object
   *
   */
  ~FileLoader() {}

  /**
   * @brief Checks if file is accessible
   *
   * @return true - if file is accessible
   * @return false - if file is not accessible
   */
  bool validate() override {
    logger.log("Checking file is installed");

    std::ifstream file("assets/games/games.json");

    return file.good();
  }

  /**
   * @brief Loads games from games.json
   *
   */
  void load() override {
    // Open file or abort if it does not exist
    std::ifstream file("./assets/games/games.json");

    // Create buffer
    nlohmann::json doc = nlohmann::json::parse(file);

    // Reads the data in the game node, assigning it to the games[] structure
    for (auto const& item : doc) {
      std::string icon_path = item["icon"];

      Game game;
      game.name = item["name"];
      game.path = item["path"];
      game.icon = Bitmap("./assets/images/icons/" + icon_path);
      games.push_back(game);
    }
  }

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
  std::string getName() override { return "FileLoader"; }

 private:
  Logger logger = Logger("FileLoader");
};