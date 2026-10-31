#pragma once

#include <asw/asw.h>

#include <filesystem>

#include <string>
#include <vector>

#include "../ColorBackground.h"
#include "../GameProcess.h"
#include "../adsgames/Installer.h"
#include "../game/GameDirectory.h"
#include "../mame/Library.h"
#include "StateId.h"

class Menu : public asw::scene::Scene<StateId> {
 public:
  using asw::scene::Scene<StateId>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

 private:
  /**
   * @brief Launch the focused game, or open the joystick test
   *
   */
  void launchFocusedGame();

  /**
   * @brief Run a game and hide the menu until it exits
   *
   * @param args - Program followed by its arguments
   * @param working_dir - Folder to run it in, empty for the current one
   */
  void runGame(std::vector<std::string> args,
               const std::filesystem::path& working_dir);

  /**
   * @brief Bring the menu back once the game exits
   *
   */
  static void restoreWindow();

  /**
   * @brief Draw the focused game's art behind the menu
   *
   */
  void drawBackdrop() const;

  /**
   * @brief Draw the row of game cards
   *
   */
  void drawCards() const;

  /**
   * @brief Draw the name and details of the focused game
   *
   */
  void drawHeader() const;

  /**
   * @brief Draw what confirm does, or download progress, under the focused card
   *
   */
  void drawStatus() const;

  /**
   * @brief Draw the controls along the bottom
   *
   */
  void drawHints() const;

  /**
   * @brief Area of the focused card
   *
   * @return asw::Quadf - The card
   */
  asw::Quadf focusedCard() const;

  /**
   * @brief Move the focus, and start fading in the new game's art
   *
   * @param focus - Index of the game to focus
   */
  void setFocus(int focus);

  // Images
  asw::Texture cursor;

  // Darkens the art behind the text
  asw::Texture fade;

  // Fonts
  asw::Font title_font;
  asw::Font body_font;
  asw::Font small_font;

  int game_focus = 0;

  // Card row position, eases towards game_focus
  float scroll = 0;

  // Fade of the focused game's art, 0 to 1
  float backdrop_fade = 0;

  // Whether the focused game is downloaded, checked when the focus changes
  bool focused_installed = false;

  // Background
  ColorBackground main_bg;

  // Games from every loader plugin
  GameDirectory directory;

  // Game started from the menu
  GameProcess game_process;
  bool game_running = false;

  // Rom sets MAME can run, kept while the menu is left and entered again
  mame::Library mame_library;

  // Downloads A.D.S. Games games, last so it stops before the rest go away
  adsgames::Installer installer;
};
