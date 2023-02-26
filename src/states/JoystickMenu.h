#ifndef JOYSTICKMENU_H_
#define JOYSTICKMENU_H_

#include "GameState.h"

#include <allegro5/allegro.h>
#include <string>
#include <vector>

#include "../ColorBackground.h"
#include "../core/Bitmap.h"
#include "../globals.h"
#include "../util/tools.h"

class JoystickMenu : public GameState {
 public:
  // Main loop functions
  JoystickMenu();
  void update();
  void draw();
  ~JoystickMenu();

 private:
  Bitmap cursor;
  Bitmap joystick_background;
  Bitmap img_joystick_button[8];
  Bitmap img_joystick_positions[8];

  // Minigame
  Bitmap ship;
  Bitmap joystick_background2;
  int ship_x, ship_y;

  bool hide_mouse;

  ColorBackground main_bg;
};

#endif  // JOYSTICKMENU_H_
