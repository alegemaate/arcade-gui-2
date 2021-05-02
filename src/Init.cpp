#include "Init.h"

#include <allegro5/allegro.h>

#include "globals.h"
#include "tools.h"

Init::Init() {
  // Load fonts
  segoe = load_font_ex("fonts/keyboard.ttf", 64);

  // Get screen size
}

void Init::update() {
  // Change to splash screen
  set_next_state(STATE_MENU);
}
