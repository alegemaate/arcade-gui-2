#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

#include "globals.h"
#include "util/tools.h"

#include "states/GameState.h"

#include "states/JoystickMenu.h"
#include "states/Menu.h"

#include "util/JoystickListener.h"
#include "util/KeyListener.h"
#include "util/MouseListener.h"

// FPS system variables
int fps;
double old_time = 0;
const float MAX_FPS = 60;
int frames_array[100];
int frame_index = 0;

// Closing or naw
bool closing = false;
bool joystick_enabled = false;

// Allegro events
ALLEGRO_EVENT_QUEUE* event_queue = nullptr;
ALLEGRO_TIMER* timer = nullptr;
ALLEGRO_DISPLAY* display = nullptr;
ALLEGRO_BITMAP* buffer;

// Input listener wrapper classes
MouseListener m_listener;
KeyListener k_listener;
JoystickListener j_listener;

// Game state object
GameState* currentState = nullptr;

// Change game screen
void changeState() {
  // If the state needs to be changed
  if (nextState == ProgramState::NONE) {
    return;
  }

  // Delete the current state
  if (currentState != nullptr) {
    delete currentState;
  }

  // Change the state
  switch (nextState) {
    case ProgramState::MENU:
      currentState = new Menu();
      break;

    case ProgramState::JOYSTICK:
      currentState = new JoystickMenu();
      break;

    case ProgramState::EXIT:
      closing = true;
      break;

    default:
      break;
  }

  // Change the current state ID
  stateID = nextState;

  // NULL the next state ID
  nextState = ProgramState::NONE;
}

// Setup game
void setup() {
  // Init allegro
  if (!al_init()) {
    abort_on_error("Allegro could not initilize");
  }

  // Window title
  al_set_window_title(display, "Loading...");

  // Controls
  al_install_keyboard();
  al_install_mouse();
  al_install_joystick();

  // GFX addons
  al_init_image_addon();
  al_init_primitives_addon();

  // Font
  al_init_ttf_addon();

  // Audio
  al_install_audio();
  al_init_acodec_addon();
  al_reserve_samples(20);

  // Set display mode to windowed
  ALLEGRO_MONITOR_INFO info;
  al_get_monitor_info(0, &info);
  SCREEN_W = (info.x2 - info.x1) / 2;
  SCREEN_H = (info.y2 - info.y1) / 2;

  al_set_new_display_flags(ALLEGRO_WINDOWED);
  display = al_create_display(SCREEN_W, SCREEN_H);
  buffer = al_create_bitmap(SCREEN_W, SCREEN_H);

  if (!display || !buffer) {
    abort_on_error("Screen could not be created");
  }

  // Timer
  timer = al_create_timer(1.0 / MAX_FPS);

  // Register events
  event_queue = al_create_event_queue();
  al_register_event_source(event_queue, al_get_display_event_source(display));
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_joystick_event_source());

  // Timer
  al_start_timer(timer);

  // Window title
  al_set_window_title(display, "Arcade GUI");

  joystick_enabled = (al_get_num_joysticks() > 0);
}

void update() {
  // Event checking
  ALLEGRO_EVENT ev;
  al_wait_for_event(event_queue, &ev);

  // Timer
  if (ev.type == ALLEGRO_EVENT_TIMER) {
    // Update listeners
    m_listener.update();
    k_listener.update();
    j_listener.update();

    try {
      // Update state
      currentState->update();

      // Switch state if needed
      changeState();
    } catch (std::exception& e) {
      abort_on_error(e.what());
    } catch (...) {
      abort_on_error("Unknown error");
    }
  }

  // Exit
  else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
    closing = true;
  }

  // Keyboard
  else if (ev.type == ALLEGRO_EVENT_KEY_DOWN ||
           ev.type == ALLEGRO_EVENT_KEY_UP) {
    k_listener.on_event(ev.type, ev.keyboard.keycode);
  }

  // Joystick Button
  else if (ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN ||
           ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP) {
    j_listener.on_event(ev.type, ev.joystick.button);
  }

  // Joystick Axis
  else if (ev.type == ALLEGRO_EVENT_JOYSTICK_AXIS) {
    j_listener.on_event(ev.type, ev.joystick.stick, ev.joystick.axis,
                        ev.joystick.pos);
  }

  // Joystick plugged or unplugged
  else if (ev.type == ALLEGRO_EVENT_JOYSTICK_CONFIGURATION) {
    al_reconfigure_joysticks();
    joystick_enabled = (al_get_num_joysticks() > 0);
  }

  // Drawing
  if (al_is_event_queue_empty(event_queue)) {
    // Render a frame
    al_set_target_bitmap(buffer);
    al_clear_to_color(al_map_rgb(0, 0, 0));

    try {
      currentState->draw();
    } catch (std::exception& e) {
      abort_on_error(e.what());
    } catch (...) {
      abort_on_error("Unknown error");
    }

    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(buffer, 0, 0, 0);

    // Flip (OpenGL)
    al_flip_display();

    // Update fps buffer
    for (int i = 99; i > 0; i--) {
      frames_array[i] = frames_array[i - 1];
    }
    frames_array[0] = (1.0 / (al_get_time() - old_time));
    old_time = al_get_time();

    int fps_total = 0;
    for (int i = 0; i < 100; i++) {
      fps_total += frames_array[i];
    }

    // FPS = average
    fps = fps_total / 100;
  }
}

// Main function*/
int main(int argc, char** argv) {
  // Setup basic functionality
  setup();

  nextState = ProgramState::MENU;
  changeState();

  while (!KeyListener::key[ALLEGRO_KEY_ESCAPE] && !closing) {
    update();
  }

  return 0;
}
