#pragma once

#include <SDL3/SDL.h>

#include <atomic>
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace mame {

/**
 * @brief A rom set that MAME can run
 *
 */
struct Entry {
  /// @brief Short name, e.g. "pacman"
  std::string name;

  /// @brief Full name, e.g. "Pac-Man (Midway)"
  std::string title;

  /// @brief Title screen image, empty if there is none
  std::filesystem::path title_image;

  /// @brief Gameplay image, empty if there is none
  std::filesystem::path snap_image;
};

/**
 * @brief The rom sets MAME can run. Scans in the background and keeps the
 * result, so later starts are instant until MAME or the rom folders change.
 *
 */
class Library {
 public:
  Library() = default;
  Library(const Library&) = delete;
  Library& operator=(const Library&) = delete;
  Library(Library&&) = delete;
  Library& operator=(Library&&) = delete;

  /**
   * @brief Stop a scan in progress
   *
   */
  ~Library();

  /**
   * @brief Find MAME and load the rom sets. Uses the saved list when nothing
   * changed, otherwise scans in the background.
   *
   * @param rom_path - Folders to search, split by ';'. Empty uses MAME's own
   * rompath.
   */
  void refresh(const std::string& rom_path);

  /**
   * @brief Forget the saved list, so the next refresh scans again
   *
   */
  static void clearCache();

  /**
   * @brief Check if MAME was found
   *
   * @return true - If MAME can be run
   */
  bool isAvailable() const { return !executable.empty(); }

  /**
   * @brief Check if a scan is running
   *
   * @return true - While scanning
   */
  bool isScanning() const { return scanning.load(); }

  /**
   * @brief Counter that goes up each time the rom sets change
   *
   * @return unsigned - The counter
   */
  unsigned getGeneration() const { return generation.load(); }

  /**
   * @brief Get the rom sets
   *
   * @return std::vector<Entry> - The rom sets, by title
   */
  std::vector<Entry> getEntries() const;

  /**
   * @brief Command that runs a rom set. MAME writes its settings and saves to
   * the save folder instead of the current folder.
   *
   * @param name - Short name of the rom set
   * @return std::vector<std::string> - Program and arguments
   */
  std::vector<std::string> launchArgs(const std::string& name) const;

 private:
  /**
   * @brief Scan for rom sets, runs on the worker thread
   *
   * @param signature - Identifies MAME and the rom folders, saved with the list
   */
  void scan(const std::string& signature);

  /**
   * @brief Run MAME and read its output, killed if the scan is stopped
   *
   * @param args - Arguments after the program
   * @return std::optional<std::string> - Output, empty if MAME did not run
   */
  std::optional<std::string> capture(const std::vector<std::string>& args);

  /**
   * @brief Stop the worker and wait for it
   *
   */
  void stop();

  /**
   * @brief Replace the rom sets
   *
   * @param list - The new rom sets
   */
  void setEntries(std::vector<Entry> list);

  // Found on the first refresh, used from the worker after
  std::string executable;
  std::string version;

  // -rompath and its value, empty to use MAME's own setting
  std::vector<std::string> rom_args;

  // Signature of the list shown or being scanned
  std::string current_signature;

  mutable std::mutex mutex;
  std::vector<Entry> entries;
  SDL_Process* running = nullptr;

  std::atomic<unsigned> generation{0};
  std::atomic<bool> scanning{false};
  std::atomic<bool> cancelled{false};

  std::thread worker;
};

}  // namespace mame
