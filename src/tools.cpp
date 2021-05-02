#include "tools.h"

#include <allegro5/allegro_native_dialog.h>

// Set next state
void set_next_state(int newState) {
  // If the user doesn't want to exit
  if (nextState != STATE_EXIT) {
    // Set the next state
    nextState = newState;
  }
}

/*
 *  ERROR REPORTING
 */
void abort_on_error(const std::string& message) {
  al_show_native_message_box(nullptr, "Error", "", message.c_str(), nullptr,
                             ALLEGRO_MESSAGEBOX_ERROR);
  exit(-1);
}

// Load bitmap with error checked_array_iterator
ALLEGRO_BITMAP* load_bitmap_ex(const std::string& path) {
  ALLEGRO_BITMAP* temp_loader;
  if (!(temp_loader = al_load_bitmap(path.c_str())))
    abort_on_error(("Cannot find image " + std::string(path) +
                    "\nPlease check your files and try again")
                       .c_str());

  return temp_loader;
}

// Load and error check sounds
ALLEGRO_SAMPLE* load_sample_ex(const std::string& path) {
  ALLEGRO_SAMPLE* temp_loader = al_load_sample(path.c_str());
  if (!temp_loader) {
    abort_on_error(("Cannot find sound " + std::string(path) +
                    "\nPlease check your files and try again")
                       .c_str());
  }

  return temp_loader;
}

// Load and error check fonts
ALLEGRO_FONT* load_font_ex(const std::string& path, const int size) {
  ALLEGRO_FONT* temp_loader = al_load_ttf_font(path.c_str(), size, 0);
  if (!temp_loader) {
    abort_on_error("Cannot find font " + path +
                   "\nPlease check your files and try again");
  }

  return temp_loader;
}
