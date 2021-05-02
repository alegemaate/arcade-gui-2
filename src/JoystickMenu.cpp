#include "JoystickMenu.h"

#include "util/JoystickListener.h"
#include "util/KeyListener.h"
#include "util/MouseListener.h"

JoystickMenu::JoystickMenu() {
  // Setup colours, options are BG_VIBRANT, BG_GRAYSCALE, BG_PASTEL, BG_BALANCED
  // and a speed (1 is default)
  main_bg = new ColorBackground("images/overlay.png", BG_BALANCED, 0.2);

  // Minigame
  ship_x = SCREEN_W / 2;
  ship_y = SCREEN_H - 630;

  // Joystick
  joystick_background =
      load_bitmap_ex("images/joystick/joystick_background.png");

  // Buttons
  for (int i = 0; i < 8; i++) {
    std::string file_name = std::string("images/joystick/joystick_button_") +
                            std::to_string(i + 1) + std::string(".png");
    img_joystick_button[i] = load_bitmap_ex(file_name.c_str());
  }

  // Positions
  for (int i = 0; i < 8; i++) {
    std::string file_name = std::string("images/joystick/joystick_pos_") +
                            std::to_string(i + 1) + std::string(".png");
    img_joystick_positions[i] = load_bitmap_ex(file_name.c_str());
  }

  joystick_background2 = load_bitmap_ex("images/joystick/background.png");

  ship = load_bitmap_ex("images/joystick/ship.png");

  // Gui images
  cursor = load_bitmap_ex("images/cursor.png");
}

void JoystickMenu::update() {
  // Move ship in minigame
  if ((JoystickListener::stick[0] || KeyListener::key[ALLEGRO_KEY_LEFT]) &&
      (ship_x > (SCREEN_W / 2 - 298))) {
    ship_x -= 2;
  } else if ((JoystickListener::stick[0] ||
              KeyListener::key[ALLEGRO_KEY_RIGHT]) &&
             (ship_x < (SCREEN_W / 2 + 298 - al_get_bitmap_width(ship)))) {
    ship_x += 2;
  }

  // Change background colour
  main_bg->changeColors();

  // Hide mouse if joystick used
  if (JoystickListener::anyButtonPressed) {
    hide_mouse = true;
  }

  // Detect mouse motion to unhide
  if (MouseListener::mouse_moved) {
    hide_mouse = false;
  }
}

void JoystickMenu::draw() {
  // Draw background
  main_bg->draw();

  // Background
  al_draw_bitmap(joystick_background2,
                 SCREEN_W / 2 - (al_get_bitmap_width(joystick_background2) / 2),
                 SCREEN_H - (al_get_bitmap_height(joystick_background) +
                             al_get_bitmap_height(joystick_background2)),
                 0);
  al_draw_bitmap(joystick_background,
                 SCREEN_W / 2 - (al_get_bitmap_width(joystick_background) / 2),
                 SCREEN_H - (al_get_bitmap_height(joystick_background)), 0);

  // Buttons
  if (JoystickListener::button[0] || KeyListener::key[ALLEGRO_KEY_1])
    al_draw_bitmap(img_joystick_button[0], SCREEN_W / 2 - 78, SCREEN_H - 424,
                   0);
  if (JoystickListener::button[1] || KeyListener::key[ALLEGRO_KEY_2])
    al_draw_bitmap(img_joystick_button[1], SCREEN_W / 2 + 30, SCREEN_H - 440,
                   0);
  if (JoystickListener::button[2] || KeyListener::key[ALLEGRO_KEY_3])
    al_draw_bitmap(img_joystick_button[2], SCREEN_W / 2 + 135, SCREEN_H - 427,
                   0);
  if (JoystickListener::button[3] || KeyListener::key[ALLEGRO_KEY_4])
    al_draw_bitmap(img_joystick_button[3], SCREEN_W / 2 - 81, SCREEN_H - 491,
                   0);
  if (JoystickListener::button[4] || KeyListener::key[ALLEGRO_KEY_5])
    al_draw_bitmap(img_joystick_button[4], SCREEN_W / 2 + 20, SCREEN_H - 513,
                   0);
  if (JoystickListener::button[5] || KeyListener::key[ALLEGRO_KEY_6])
    al_draw_bitmap(img_joystick_button[5], SCREEN_W / 2 + 122, SCREEN_H - 492,
                   0);
  if (JoystickListener::button[10] || KeyListener::key[ALLEGRO_KEY_7])
    al_draw_bitmap(img_joystick_button[6], SCREEN_W / 2 + 181, SCREEN_H - 551,
                   0);
  if (JoystickListener::button[11] || KeyListener::key[ALLEGRO_KEY_8])
    al_draw_bitmap(img_joystick_button[7], SCREEN_W / 2 + 251, SCREEN_H - 546,
                   0);

  // Stick
  if ((JoystickListener::stick[0] && JoystickListener::stick[1]) ||
      (KeyListener::key[ALLEGRO_KEY_LEFT] &&
       KeyListener::key[ALLEGRO_KEY_UP])) {
    al_draw_bitmap(img_joystick_positions[0], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  } else if ((JoystickListener::stick[0] && JoystickListener::stick[1]) ||
             (KeyListener::key[ALLEGRO_KEY_RIGHT] &&
              KeyListener::key[ALLEGRO_KEY_UP])) {
    al_draw_bitmap(img_joystick_positions[2], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  } else if ((JoystickListener::stick[0] && JoystickListener::stick[1]) ||
             (KeyListener::key[ALLEGRO_KEY_RIGHT] &&
              KeyListener::key[ALLEGRO_KEY_DOWN])) {
    al_draw_bitmap(img_joystick_positions[4], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  } else if ((JoystickListener::stick[0] && JoystickListener::stick[1]) ||
             (KeyListener::key[ALLEGRO_KEY_LEFT] &&
              KeyListener::key[ALLEGRO_KEY_DOWN])) {
    al_draw_bitmap(img_joystick_positions[6], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  } else if ((JoystickListener::stick[1]) || KeyListener::key[ALLEGRO_KEY_UP]) {
    al_draw_bitmap(img_joystick_positions[1], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  } else if ((JoystickListener::stick[0]) ||
             KeyListener::key[ALLEGRO_KEY_RIGHT]) {
    al_draw_bitmap(img_joystick_positions[3], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  } else if ((JoystickListener::stick[1]) ||
             KeyListener::key[ALLEGRO_KEY_DOWN]) {
    al_draw_bitmap(img_joystick_positions[5], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  } else if ((JoystickListener::stick[0]) ||
             KeyListener::key[ALLEGRO_KEY_LEFT]) {
    al_draw_bitmap(img_joystick_positions[7], SCREEN_W / 2 - 306,
                   SCREEN_H - 553, 0);
  }

  // Minigame
  al_draw_bitmap(ship, ship_x, ship_y, 0);

  // Draw mouse if joystick not in use
  if (!hide_mouse) {
    al_draw_bitmap(cursor, MouseListener::mouse_x, MouseListener::mouse_y, 0);
  }
}

JoystickMenu::~JoystickMenu() {}
