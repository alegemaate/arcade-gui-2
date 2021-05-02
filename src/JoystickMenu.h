#ifndef JOYSTICKMENU_H_
#define JOYSTICKMENU_H_

#include "GameState.h"

#include <allegro5/allegro.h>
#include <string>
#include <vector>

#include "ColorBackground.h"
#include "globals.h"
#include "tools.h"

class JoystickMenu : public GameState {
 public:
  // Main loop functions
  JoystickMenu();
  void update();
  void draw();
  ~JoystickMenu();

 private:
  ALLEGRO_BITMAP* cursor;
  ALLEGRO_BITMAP* joystick_background;
  ALLEGRO_BITMAP* img_joystick_button[8];
  ALLEGRO_BITMAP* img_joystick_positions[8];

  // Minigame
  ALLEGRO_BITMAP* ship;
  ALLEGRO_BITMAP* joystick_background2;
  int ship_x, ship_y;

  bool hide_mouse;

  ColorBackground* main_bg;
};

#endif  // JOYSTICKMENU_H_
