#pragma once

#ifdef __unix__
#include <cstdlib>
#elif defined(_WIN32) || defined(WIN32)
#include <shlwapi.h>
#endif

#include <fstream>
#include <string>
#include <vector>

#include "../game/Game.h"
#include "../util/Logger.h"
#include "LoaderPlugin.h"

const std::string MAME_LOADER_NAME = "MameLoader";

/**
 * @brief Mame Loader Plugin
 *
 */
class MameLoader : public LoaderPlugin {
 public:
  /**
   * @brief Construct a new Mame Loader object
   *
   */
  MameLoader() {}

  /**
   * @brief Destroy the Mame Loader object
   *
   */
  ~MameLoader() {}

  /**
   * @brief Checks if mame is installed
   *
   * @return true - if mame is installed
   * @return false - if mame is not installed
   */
  bool validate() override {
    logger.log("Checking mame is installed");
    auto status = system("mame -help > /dev/null");
    logger.log("Mame installed: " + std::to_string(status == 0));
    return status == 0;
  }

  /**
   * @brief Loads games from mame roms list and caches to roms.txt
   *
   */
  void load() override {
    auto status = verifyRoms();

    if (!status) {
      logger.log("Mame roms could not be verified");
      return;
    }

    std::ifstream rom_file("roms.txt");
    std::string line;

    while (std::getline(rom_file, line)) {
      if (line.find("is good") == std::string::npos) {
        continue;
      }

      // Remove "romset " and " is good" to get game id
      std::string rom_name = line;
      rom_name = rom_name.substr(7, rom_name.length());
      rom_name = rom_name.substr(0, rom_name.find(" "));

      logger.log("Found rom: " + rom_name);

      Game game;
      game.name = rom_name;
      game.path = "mame " + rom_name;
      game.icon = Bitmap("assets/images/icons/icon_mame.png");
      games.push_back(game);
    }

    rom_file.close();
  }

  /**
   * @brief Revalidates mame roms and caches to roms.txt
   *
   */
  void revalidate() override { verifyRoms(true); }

  /**
   * @brief Return name of plugin
   *
   * @return std::string - name of plugin
   */
  std::string getName() override { return MAME_LOADER_NAME; }

 private:
  /**
   * @brief Verifies mame roms and caches to roms.txt
   *
   * @param skipCache - skips cache and verifies roms again
   * @return true - if roms are verified
   * @return false
   */
  bool verifyRoms(bool skipCache = false) {
    std::ifstream rom_file("roms.txt");

    if (rom_file.good() && !skipCache) {
      logger.log("Mame roms already verified, loading cache");
      return true;
    } else if (skipCache) {
      logger.log("Mame roms cache skipped");
    } else {
      logger.log("Mame roms cache not found");
    }

    auto status = system("mame -verifyroms > roms.txt");

    if (status != 0) {
      logger.log("Error verifying roms");
    }

    rom_file.close();

    return status == 0;
  }

  /// @brief Logger instance
  Logger logger = Logger(MAME_LOADER_NAME);
};