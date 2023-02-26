#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include "globals.h"

#include "states/StateEngine.h"

#include "util/JoystickListener.h"
#include "util/KeyListener.h"
#include "util/Logger.h"
#include "util/MouseListener.h"

// FPS system variables
int fps;
double old_time = 0;
const float MAX_FPS = 60;
int frames_array[100];
int frame_index = 0;

bool joystick_enabled = false;

// Allegro events
ALLEGRO_EVENT_QUEUE* event_queue = nullptr;
ALLEGRO_TIMER* timer = nullptr;
ALLEGRO_DISPLAY* display = nullptr;

// Input listener wrapper classes
MouseListener m_listener;
KeyListener k_listener;
JoystickListener j_listener;

// Game state object
StateEngine engine;

// Logger
Logger logger = Logger("System");

/**
 * @brief Setup core Allegro functions
 *
 */
void setupAllegroCore() {
  if (!al_init()) {
    abort_on_error("Allegro could not initilize");
  }

  al_set_window_title(display, "Loading...");
}

/**
 * @brief Setup input devices
 *
 */
void setupInputs() {
  if (!al_install_keyboard()) {
    abort_on_error("Keyboard could not be installed");
  }

  if (!al_install_mouse()) {
    abort_on_error("Mouse could not be installed");
  }

  if (!al_install_joystick()) {
    abort_on_error("Joystick could not be installed");
  }

  joystick_enabled = (al_get_num_joysticks() > 0);
}

/**
 * @brief Setup allegro addons
 *
 */
void setupAddons() {
  if (!al_init_image_addon()) {
    abort_on_error("Image addon could not be installed");
  }

  if (!al_init_primitives_addon()) {
    abort_on_error("Primitives addon could not be installed");
  }

  if (!al_init_ttf_addon()) {
    abort_on_error("TTF addon could not be installed");
  }

  if (!al_install_audio()) {
    abort_on_error("Audio could not be installed");
  }

  if (!al_init_acodec_addon()) {
    abort_on_error("Audio codec could not be installed");
  }

  if (!al_reserve_samples(10)) {
    abort_on_error("Audio samples could not be reserved");
  }
}

/**
 * @brief Create the display
 *
 */
void setupDisplay() {
  // Set display mode to windowed
  ALLEGRO_MONITOR_INFO info;
  al_get_monitor_info(0, &info);
  SCREEN_W = (info.x2 - info.x1);
  SCREEN_H = (info.y2 - info.y1);

  al_set_new_display_flags(ALLEGRO_WINDOWED);
  display = al_create_display(SCREEN_W, SCREEN_H);

  if (!display) {
    abort_on_error("Screen could not be created");
  }

  // Window title
  al_set_window_title(display, "Arcade GUI");
}

/**
 * @brief Setup events
 *
 */
void setupEvents() {
  // Timer
  timer = al_create_timer(1.0 / MAX_FPS);

  if (!timer) {
    abort_on_error("Timer could not be created");
  }

  // Register events
  event_queue = al_create_event_queue();

  if (!event_queue) {
    abort_on_error("Event queue could not be created");
  }

  al_register_event_source(event_queue, al_get_display_event_source(display));
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_joystick_event_source());

  // Timer
  al_start_timer(timer);
}

/**
 * @brief Setup routines
 *
 */
void setup() {
  logger.log("Setting up program");

  setupAllegroCore();
  logger.log("Allegro core initialized");

  setupInputs();
  logger.log("Inputs initialized");

  setupAddons();
  logger.log("Addons initialized");

  setupDisplay();
  logger.log("Display initialized");

  setupEvents();
  logger.log("Events initialized");
}

/**
 * @brief Shutdown the program
 *
 */
void shutdown() {
  logger.log("Shutting down program");

  al_destroy_display(display);
  al_destroy_timer(timer);
  al_destroy_event_queue(event_queue);
}

/**
 * @brief Update the game state
 *
 */
void update() {
  // Event checking
  ALLEGRO_EVENT ev;
  al_wait_for_event(event_queue, &ev);

  // Timer
  switch (ev.type) {
    case ALLEGRO_EVENT_TIMER:
      // Update listeners
      m_listener.update();
      k_listener.update();
      j_listener.update();

      // Update state
      engine.update();
      break;

      // Exit
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      engine.setState(StateId::EXIT);
      break;

      // Keyboard
    case ALLEGRO_EVENT_KEY_DOWN:
    case ALLEGRO_EVENT_KEY_UP:
      k_listener.on_event(ev.type, ev.keyboard.keycode);
      break;

      // Joystick Button
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
    case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
      j_listener.on_event(ev.type, ev.joystick.button);
      break;

      // Joystick Axis
    case ALLEGRO_EVENT_JOYSTICK_AXIS:
      j_listener.on_event(ev.type, ev.joystick.stick, ev.joystick.axis,
                          ev.joystick.pos);
      break;

      // Joystick plugged or unplugged
    case ALLEGRO_EVENT_JOYSTICK_CONFIGURATION:
      al_reconfigure_joysticks();
      joystick_enabled = (al_get_num_joysticks() > 0);
      break;
  }

  // Drawing
  if (al_is_event_queue_empty(event_queue)) {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    engine.draw();

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

/**
 * @brief Main function
 *
 * @param argc - number of arguments
 * @param argv - arguments
 * @return int - exit code
 */
int main(int argc, char** argv) {
  // Setup allegro functionality
  setup();

  engine.setState(StateId::MENU);

  while (!engine.isClosing()) {
    update();
  }

  // Shutdown
  shutdown();

  return 0;
}
