#pragma once

#include <string>

/**
 * @brief Launcher settings, saved in the save folder
 *
 */
struct Settings {
  /// @brief Folders MAME searches for roms, split by ';'. Empty uses MAME's own
  /// rompath.
  std::string mame_rom_path;

  /**
   * @brief Load the saved settings, defaults if there are none
   *
   * @return Settings - The settings
   */
  static Settings load();

  /**
   * @brief Save the settings
   *
   * @return true - If they were written
   */
  bool save() const;
};

/**
 * @brief Replace a leading ~ with the home folder
 *
 * @param path - Path that may start with ~
 * @return std::string - The expanded path
 */
std::string expand_home(const std::string& path);
