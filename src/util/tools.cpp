#include "tools.h"

#include <allegro5/allegro_native_dialog.h>

#include "./JoystickListener.h"
#include "./MouseListener.h"

/*
 *  ERROR REPORTING
 */
void abort_on_error(const std::string& message) {
  al_show_native_message_box(nullptr, "Error", "", message.c_str(), nullptr,
                             ALLEGRO_MESSAGEBOX_ERROR);
  exit(-1);
}

// Area clicked
bool locationClicked(int min_x, int max_x, int min_y, int max_y) {
  return MouseListener::mouse_x >= min_x && MouseListener::mouse_x <= max_x &&
         MouseListener::mouse_y >= min_y && MouseListener::mouse_y <= max_y &&
         (MouseListener::mouse_button & 1 || JoystickListener::button[1]);
}