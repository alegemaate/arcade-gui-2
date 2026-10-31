#pragma once

#include <asw/asw.h>
#include <asw/modules/ui/ui.h>

#include <mutex>
#include <optional>
#include <string>

#include "../ColorBackground.h"
#include "../Settings.h"
#include "StateId.h"

class SettingsMenu : public asw::scene::Scene<StateId> {
 public:
  using asw::scene::Scene<StateId>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

 private:
  /**
   * @brief Save and go back to the menu
   *
   */
  void close();

  /**
   * @brief Open the system folder chooser for the rom folder
   *
   */
  void browse();

  /**
   * @brief Show a line under the buttons
   *
   * @param text - What to show
   */
  void setStatus(const std::string& text);

  /**
   * @brief Called by SDL when a folder is chosen, on any thread
   *
   * @param userdata - The settings menu
   * @param filelist - Chosen folders, null on error, empty when cancelled
   * @param filter - Unused
   */
  static void onFolderChosen(void* userdata,
                             const char* const* filelist,
                             int filter);

  Settings settings;

  asw::ui::Root ui;
  asw::ui::InputBox* rom_input = nullptr;
  asw::ui::Label* status = nullptr;

  asw::Texture cursor;
  asw::Font title_font;
  asw::Font body_font;
  asw::Font small_font;

  ColorBackground main_bg;

  // Folder from the chooser, handed to the main thread
  std::mutex chosen_mutex;
  std::optional<std::string> chosen_folder;
};
