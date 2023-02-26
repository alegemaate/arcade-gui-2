#ifndef GLOBALS_H_
#define GLOBALS_H_

// All libaries
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>

// Program states
enum class ProgramState {
  NONE,
  MENU,
  JOYSTICK,
  EXIT,
};

// State variables
extern ProgramState stateID;
extern ProgramState nextState;

// Screen size
extern int SCREEN_W;
extern int SCREEN_H;

#endif  // GLOBALS_H_
