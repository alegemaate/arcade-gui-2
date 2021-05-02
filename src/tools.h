#ifndef TOOLS_H_
#define TOOLS_H_
#define BACKGROUND false
#define FOREGROUND true
#define MAIN_MENU 0
#define HELP 1
#define OPTIONS 2
#define STORY 3

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include <string>
#include "globals.h"

// Random number generator. Use int random(highest,lowest);
extern int random(int newLowest, int newHighest);

// Error reporting
extern void abort_on_error(const std::string& message);

// Set next state
extern void set_next_state(int newState);

// Load and error check images
extern ALLEGRO_BITMAP* load_bitmap_ex(const std::string& path);

// Load and error check sounds
extern ALLEGRO_SAMPLE* load_sample_ex(const std::string& path);

// Load and error check fonts
extern ALLEGRO_FONT* load_font_ex(const std::string& path, const int size);

#endif  // TOOLS_H_
