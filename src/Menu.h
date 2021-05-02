#ifndef MENU_H_
#define MENU_H_

#include "GameState.h"

#include <allegro5/allegro.h>
#include <string>
#include <vector>

#include "ColorBackground.h"
#include "globals.h"
#include "tools.h"

// Game icons
struct Game {
  int w;
  int h;
  std::string path;
  std::string name;
  ALLEGRO_BITMAP* icon;
};

class Menu : public GameState {
 public:
  // Main loop functions
  Menu();
  void update();
  void draw();
  ~Menu();

 private:
  // Images
  ALLEGRO_BITMAP* buffer;
  ALLEGRO_BITMAP* cursor;
  ALLEGRO_BITMAP* overlay_text;

  int game_focus;
  bool hide_mouse;

  // Transition of icons
  float icon_transition;

  bool locationClicked(int min_x, int max_x, int min_y, int max_y);
  void loadGames(const std::string& path);

  // Background
  ColorBackground* main_bg;

  // Games
  std::vector<Game> games;
};

#endif  // MENU_H_
