#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

#include "../game/Game.h"

namespace adsgames {

/// @brief Base url of the A.D.S. Games API
inline constexpr const char* API_URL = "https://api.adsgames.net";

/**
 * @brief Release platform of this build, as the API names it
 *
 * @return const char* - "WINDOWS", "MAC" or "LINUX"
 */
const char* platform();

/**
 * @brief Writable folder for downloaded games and cached data
 *
 * @return std::filesystem::path - The folder, created if needed
 */
std::filesystem::path cache_dir();

/**
 * @brief Folder a release installs to
 *
 * @param release - The release
 * @return std::filesystem::path - The folder, it may not exist
 */
std::filesystem::path install_dir(const Release& release);

/**
 * @brief Find the newest release of a game for this platform
 *
 * @param game - Game from the API, with its releases
 * @return std::optional<Release> - The release, empty if there is none
 */
std::optional<Release> latest_release(const nlohmann::json& game);

/**
 * @brief Read a whole file
 *
 * @param path - File to read
 * @return std::optional<std::string> - Contents, empty if it can not be read
 */
std::optional<std::string> read_file(const std::filesystem::path& path);

/**
 * @brief Write a whole file, replacing it
 *
 * @param path - File to write
 * @param data - Contents
 * @return true - If it was written
 */
bool write_file(const std::filesystem::path& path, const std::string& data);

}  // namespace adsgames
