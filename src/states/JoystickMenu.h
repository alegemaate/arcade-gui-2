#ifndef JOYSTICKMENU_H_
#define JOYSTICKMENU_H_

#include "GameState.h"

#include <allegro5/allegro.h>
#include <string>
#include <vector>

#include "../ColorBackground.h"
#include "../core/Bitmap.h"
#include "../globals.h"
#include "../util/Logger.h"
#include "../util/tools.h"

class JoystickMenu : public GameState {
 public:
  // Main loop functions
  JoystickMenu();

  void update();

  void draw();

 private:
  Bitmap cursor = Bitmap("./assets/images/cursor.png");
  Bitmap joystick_background =
      Bitmap("./assets/images/joystick/joystick_background.png");
  Bitmap img_joystick_button[8];
  Bitmap img_joystick_positions[8];

  // Minigame
  Bitmap ship = Bitmap("./assets/images/joystick/ship.png");
  Bitmap joystick_background2 =
      Bitmap("./assets/images/joystick/background.png");
  int ship_x = 0;
  int ship_y = 0;

  bool hide_mouse = false;

  ColorBackground main_bg = ColorBackground();

  // Logger
  Logger logger = Logger("JoystickMenu");
};

#endif  // JOYSTICKMENU_H_
