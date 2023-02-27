#ifndef MENU_H_
#define MENU_H_

#include "GameState.h"

#include <allegro5/allegro.h>
#include <string>
#include <vector>

#include "../ColorBackground.h"
#include "../core/Bitmap.h"
#include "../core/Font.h"
#include "../game/GameDirectory.h"
#include "../globals.h"
#include "../plugins/FileLoader.h"
#include "../plugins/MameLoader.h"
#include "../util/Logger.h"
#include "../util/tools.h"

class Menu : public GameState {
 public:
  // Main loop functions
  Menu();

  void update();
  void draw();

 private:
  // Images
  Bitmap cursor = Bitmap("./assets/images/cursor.png");
  Bitmap overlay_text = Bitmap("./assets/images/overlay_text.png");

  int game_focus = 0;
  bool hide_mouse = false;

  // Transition of icons
  float icon_transition = 0;

  // Background
  ColorBackground main_bg{};

  // Font
  Font segoe = Font("./assets/fonts/keyboard.ttf", 40);

  // Logger
  Logger logger = Logger("Menu");

  // Mame adapter
  GameDirectory directory;
};

#endif  // MENU_H_
