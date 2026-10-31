#pragma once

#include <SDL3/SDL.h>

#include <filesystem>
#include <string>
#include <vector>

/**
 * @brief A game started by the launcher, running alongside it
 *
 */
class GameProcess {
 public:
  GameProcess() = default;
  GameProcess(const GameProcess&) = delete;
  GameProcess& operator=(const GameProcess&) = delete;
  GameProcess(GameProcess&&) = delete;
  GameProcess& operator=(GameProcess&&) = delete;

  /**
   * @brief Let go of the game, it keeps running if the launcher quits
   *
   */
  ~GameProcess();

  /**
   * @brief Start a program, does nothing if one is running
   *
   * @param args - Program followed by its arguments
   * @param working_dir - Folder to run it in, empty for the current one
   * @return true - If it started
   */
  bool start(std::vector<std::string> args,
             const std::filesystem::path& working_dir);

  /**
   * @brief Check if the program is still running, does not block
   *
   * @return true - While it runs
   */
  bool isRunning();

 private:
  SDL_Process* process = nullptr;
};
