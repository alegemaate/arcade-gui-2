#ifndef MENU_H_
#define MENU_H_

#include "GameState.h"

#include <allegro5/allegro.h>
#include <string>
#include <vector>

#include "../ColorBackground.h"
#include "../core/Bitmap.h"
#include "../core/Font.h"
#include "../globals.h"
#include "../util/Logger.h"
#include "../util/tools.h"

// Game icons
struct Game {
  int w;
  int h;
  std::string path;
  std::string name;
  Bitmap icon;
};

class Menu : public GameState {
 public:
  // Main loop functions
  Menu();

  void update();
  void draw();

 private:
  void loadGames(const std::string& path);

  // Images
  Bitmap cursor = Bitmap("./assets/images/cursor.png");
  Bitmap overlay_text = Bitmap("./assets/images/overlay_text.png");

  int game_focus = 0;
  bool hide_mouse = false;

  // Transition of icons
  float icon_transition = 0;

  // Background
  ColorBackground main_bg{};

  // Games
  std::vector<Game> games{};

  // Font
  Font segoe = Font("./assets/fonts/keyboard.ttf", 40);

  // Logger
  Logger logger = Logger("Menu");
};

#endif  // MENU_H_
