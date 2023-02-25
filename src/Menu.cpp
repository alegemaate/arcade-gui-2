#include "Menu.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "util/JoystickListener.h"
#include "util/KeyListener.h"
#include "util/MouseListener.h"

#ifdef __unix__
#include <cstdlib>
#elif defined(_WIN32) || defined(WIN32)
#include <shlwapi.h>
#endif

// Scale of icon in focus
const int ICON_SCALE = 200;
const int ICON_DISTANCE = 300;
const float ICON_TRANSITION_SPEED = 0.08;
const float ICON_FOCUS_SCALE_MULTIPLER = 2;

// Area clicked
bool Menu::locationClicked(int min_x, int max_x, int min_y, int max_y) {
  return MouseListener::mouse_x >= min_x && MouseListener::mouse_x <= max_x &&
         MouseListener::mouse_y >= min_y && MouseListener::mouse_y <= max_y &&
         (MouseListener::mouse_button & 1 || JoystickListener::button[1]);
}

void Menu::loadGames(const std::string& path) {
  // Open file or abort if it does not exist
  std::ifstream file(path);

  // Create buffer
  nlohmann::json doc = nlohmann::json::parse(file);

  // Reads the data in the game node, assigning it to the games[] structure
  for (auto const& item : doc) {
    std::string icon_path = item["icon"];

    Game game;
    game.name = item["name"];
    game.path = item["path"];
    game.icon = load_bitmap_ex("images/icons/" + icon_path);
    game.w = al_get_bitmap_width(game.icon);
    game.h = al_get_bitmap_height(game.icon);
    games.push_back(game);
  }
}

Menu::Menu() {
  // Focused game
  game_focus = 0;
  icon_transition = 0;

  // Create buffer
  buffer = al_create_bitmap(SCREEN_W, SCREEN_H);

  // Setup colours, options are VIBRANT, GREYSCALE, PASTEL, BALANCED and a speed
  // (1 is default)
  main_bg = new ColorBackground("images/overlay.png", BG_BALANCED, 0.2);

  // Load games
  loadGames("games/games.json");

  // Gui images
  cursor = load_bitmap_ex("images/cursor.png");
  overlay_text = load_bitmap_ex("images/overlay_text.png");
}

void Menu::update() {
  // Joystick APP
  if ((locationClicked(442, 837, 322, 717) ||
       JoystickListener::buttonPressed[2] ||
       KeyListener::keyPressed[ALLEGRO_KEY_ENTER]) &&
      icon_transition == 0) {
    if (games[game_focus].path == "arcade://joystick") {
      set_next_state(STATE_JOYSTICK);
    } else {
      // #ifdef __unix__
      //       system(games.at(game_focus).path);
      // #elif defined(_WIN32) || defined(WIN32)
      //       ShellExecute(NULL, "open", games.at(game_focus).path.c_str(),
      //       NULL, NULL,
      //                    SW_SHOWDEFAULT);
      // #endif
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
      icon_transition == 0 && game_focus < games.size() - 1) {
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
  main_bg->changeColors();

  // Hide mouse
  if (JoystickListener::anyButtonPressed) {
    hide_mouse = true;
  }

  if (MouseListener::mouse_moved) {
    hide_mouse = false;
  }
}

void Menu::draw() {
  // Draw background
  main_bg->draw();

  // Title
  al_draw_scaled_bitmap(overlay_text, 0, 0, al_get_bitmap_height(overlay_text),
                        al_get_bitmap_height(overlay_text), 0, 50, SCREEN_W,
                        al_get_bitmap_height(overlay_text), 0);

  al_draw_text(segoe, al_map_rgb(0, 0, 0), SCREEN_W / 2, 80, 0,
               games.at(game_focus).name.c_str());

  // Draw icon (stretched if needed)
  for (int i = 0; i < games.size(); i++) {
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
    al_draw_scaled_bitmap(games.at(i).icon, 0, 0, games.at(i).w, games.at(i).h,
                          icon_x - offset_x, icon_y - offset_y, scale, scale,
                          0);
  }

  // When using joystick, dont show mouse
  if (!hide_mouse) {
    al_draw_bitmap(cursor, MouseListener::mouse_x, MouseListener::mouse_y, 0);
  }
}

Menu::~Menu() {}
