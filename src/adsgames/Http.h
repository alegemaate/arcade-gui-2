#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>

namespace adsgames::http {

/// @brief Called as a download runs. Return false to cancel it.
using Progress = std::function<bool(uint64_t received, uint64_t total)>;

/**
 * @brief Download a url. Blocks until it is done, so call it off the main
 * thread for large files.
 *
 * @param url - Address to download
 * @param progress - Called as the download runs, optional
 * @return std::optional<std::string> - The body, empty on any error
 */
std::optional<std::string> get(const std::string& url,
                               const Progress& progress = {});

}  // namespace adsgames::http
