#include "Menu.h"

#include <fstream>
#include <memory>

#ifdef __unix__
#include <cstdlib>
#elif defined(_WIN32) || defined(WIN32)
#include <shlwapi.h>
#endif

#include "../util/JoystickListener.h"
#include "../util/KeyListener.h"
#include "../util/MouseListener.h"
#include "../util/tools.h"

#include "StateId.h"

// Scale of icon in focus
const int ICON_SCALE = 200;
const int ICON_DISTANCE = 300;
const float ICON_TRANSITION_SPEED = 0.08;
const float ICON_FOCUS_SCALE_MULTIPLER = 2;

Menu::Menu() {
  // Setup colours
  ColorBackgroundPreset preset = {Vec3<double>(0.0, 100.0, 200.0),
                                  Vec3<double>(true, true, false),
                                  Bitmap("./assets/images/overlay.png")};

  main_bg.setPreset(preset);

  // Load games
  directory.loadPlugin(std::make_shared<MameLoader>());
  directory.loadPlugin(std::make_shared<FileLoader>());
}

void Menu::update() {
  auto focused_game = directory.listGames().at(game_focus);

  // Joystick APP
  if ((locationClicked(442, 837, 322, 717) ||
       JoystickListener::buttonPressed[2] ||
       KeyListener::keyPressed[ALLEGRO_KEY_ENTER]) &&
      icon_transition == 0) {
    if (focused_game.path == "arcade://joystick") {
      changeState(StateId::JOYSTICK);
    } else {
      logger.log("Launching game: " + focused_game.name);
#ifdef __unix__
      auto status = system(focused_game.path.c_str());
      logger.log("Game exited with status: " + std::to_string(status));
#elif defined(_WIN32) || defined(WIN32)
      ShellExecute(NULL, "open", focused_game.path.c_str(), NULL, NULL,
                   SW_SHOWDEFAULT);
#endif
    }
  }

  // Scroll Left
  if ((locationClicked(0, SCREEN_W / 4, 0, SCREEN_H) ||
       JoystickListener::stick[0] || KeyListener::key[ALLEGRO_KEY_LEFT]) &&
      icon_transition == 0 && game_focus > 0) {
    game_focus--;
    icon_transition = -1;
  }

  // Scroll Right
  if ((locationClicked(SCREEN_W - SCREEN_W / 4, SCREEN_W, 0, SCREEN_H) ||
       JoystickListener::stick[0] || KeyListener::key[ALLEGRO_KEY_RIGHT]) &&
      icon_transition == 0 && game_focus < directory.listGames().size() - 1) {
    game_focus++;
    icon_transition = 1;
  }

  // Scroll
  if (icon_transition > ICON_TRANSITION_SPEED) {
    icon_transition -= ICON_TRANSITION_SPEED;
  } else if (icon_transition > 0) {
    icon_transition = 0;
  } else if (icon_transition < 0) {
    icon_transition += ICON_TRANSITION_SPEED;
  } else if (icon_transition < ICON_TRANSITION_SPEED) {
    icon_transition = 0;
  }

  // Change background colour
  main_bg.changeColors();

  // Hide mouse
  if (JoystickListener::anyButtonPressed) {
    hide_mouse = true;
  }

  if (MouseListener::mouse_moved) {
    hide_mouse = false;
  }

  if (KeyListener::keyPressed[ALLEGRO_KEY_ESCAPE]) {
    changeState(StateId::EXIT);
  }
}

void Menu::draw() {
  // Draw background
  main_bg.draw();

  // Title
  al_draw_scaled_bitmap(overlay_text.get(), 0, 0, overlay_text.getHeight(),
                        overlay_text.getHeight(), 0, 50, SCREEN_W,
                        overlay_text.getHeight(), 0);

  al_draw_text(segoe.get(), al_map_rgb(0, 0, 0), SCREEN_W / 2, 80,
               ALLEGRO_ALIGN_CENTRE,
               directory.listGames().at(game_focus).name.c_str());

  // Draw icon (stretched if needed)
  for (int i = 0; i < directory.listGames().size(); i++) {
    // Initial scale is original
    int scale = ICON_SCALE;

    // If its the current icon, enlarge it
    if (i == game_focus) {
      scale *= ICON_FOCUS_SCALE_MULTIPLER - abs(icon_transition);
    }

    // Calc the positions
    int icon_x = SCREEN_W * 0.5 + ICON_DISTANCE * (i - game_focus);
    int icon_y = SCREEN_H * 0.6;

    int offset_x = scale / 2 - icon_transition * ICON_DISTANCE;
    int offset_y = scale / 2;

    // Draw the icon
    al_draw_scaled_bitmap(directory.listGames().at(i).icon.get(), 0, 0,
                          directory.listGames().at(i).icon.getWidth(),
                          directory.listGames().at(i).icon.getHeight(),
                          icon_x - offset_x, icon_y - offset_y, scale, scale,
                          0);
  }

  // When using joystick, dont show mouse
  if (!hide_mouse) {
    al_draw_bitmap(cursor.get(), MouseListener::mouse_x, MouseListener::mouse_y,
                   0);
  }
}
