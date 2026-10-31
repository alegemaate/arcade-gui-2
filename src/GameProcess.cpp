#include "GameProcess.h"

#include <asw/asw.h>

GameProcess::~GameProcess() {
  // Only frees the handle, the game is not stopped
  if (process != nullptr) {
    SDL_DestroyProcess(process);
  }
}

bool GameProcess::start(std::vector<std::string> args,
                        const std::filesystem::path& working_dir) {
  if (process != nullptr || args.empty()) {
    return false;
  }

  std::vector<char*> argv;
  argv.reserve(args.size() + 1);
  for (auto& arg : args) {
    argv.push_back(arg.data());
  }
  argv.push_back(nullptr);

  const SDL_PropertiesID props = SDL_CreateProperties();
  SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER,
                         static_cast<void*>(argv.data()));

  const auto dir = working_dir.string();
  if (!dir.empty()) {
    SDL_SetStringProperty(
        props, SDL_PROP_PROCESS_CREATE_WORKING_DIRECTORY_STRING, dir.c_str());
  }

  process = SDL_CreateProcessWithProperties(props);
  SDL_DestroyProperties(props);

  if (process == nullptr) {
    asw::log::error("Could not start {}: {}", args.front(), SDL_GetError());
    return false;
  }

  return true;
}

bool GameProcess::isRunning() {
  if (process == nullptr) {
    return false;
  }

  int exit_code = 0;
  if (!SDL_WaitProcess(process, false, &exit_code)) {
    return true;
  }

  asw::log::info("Game exited with status: {}", exit_code);
  SDL_DestroyProcess(process);
  process = nullptr;
  return false;
}
