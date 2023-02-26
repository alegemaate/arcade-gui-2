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
  Bitmap buffer;
  Bitmap cursor;
  Bitmap overlay_text;

  int game_focus;
  bool hide_mouse;

  // Transition of icons
  float icon_transition;

  // Background
  ColorBackground main_bg;

  // Games
  std::vector<Game> games;

  // Font
  Font segoe;
};

#endif  // MENU_H_
