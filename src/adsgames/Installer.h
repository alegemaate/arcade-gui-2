#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <thread>

#include "../game/Game.h"

namespace adsgames {

/**
 * @brief Downloads and unzips A.D.S. Games releases in the background
 *
 */
class Installer {
 public:
  Installer() = default;
  Installer(const Installer&) = delete;
  Installer& operator=(const Installer&) = delete;
  Installer(Installer&&) = delete;
  Installer& operator=(Installer&&) = delete;

  /**
   * @brief Cancel a download in progress and wait for the worker
   *
   */
  ~Installer();

  enum class State : uint8_t {
    Idle,
    Downloading,
    Installing,
    Ready,
    Failed,
  };

  /**
   * @brief Install the newest build of a game, does nothing while busy. Asks
   * the API for the newest release first, and falls back to the one given.
   *
   * @param release - Release to install
   */
  void start(const Release& release);

  /**
   * @brief Get the state of the current install
   *
   * @return State - The state
   */
  State getState() const { return state.load(); }

  /**
   * @brief Check if an install is running
   *
   * @return true - While downloading or installing
   */
  bool isBusy() const;

  /**
   * @brief Get download progress
   *
   * @return float - 0 to 1
   */
  float getProgress() const { return progress.load(); }

  /**
   * @brief Take the executable of a finished install, and go back to idle
   *
   * @return std::optional<std::filesystem::path> - The executable, empty
   * unless the install is ready
   */
  std::optional<std::filesystem::path> takeExecutable();

 private:
  /**
   * @brief Install a release, runs on the worker thread
   *
   * @param requested - Release to install
   */
  void run(const Release& requested);

  std::atomic<State> state{State::Idle};
  std::atomic<float> progress{0.0F};
  std::atomic<bool> cancelled{false};

  // Written by the worker before it sets Ready
  std::filesystem::path executable;

  std::thread worker;
};

}  // namespace adsgames
