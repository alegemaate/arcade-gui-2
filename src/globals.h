#ifndef GLOBALS_H_
#define GLOBALS_H_

// All libaries
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>

// Game states
enum GameStates {
  STATE_NULL,
  STATE_INIT,
  STATE_MENU,
  STATE_JOYSTICK,
  STATE_EXIT,
};

// Frames
extern int frames_done;

// State variables
extern int stateID;
extern int nextState;

// Font
extern ALLEGRO_FONT* segoe;

// Screen size
extern int SCREEN_W;
extern int SCREEN_H;

#endif  // GLOBALS_H_
