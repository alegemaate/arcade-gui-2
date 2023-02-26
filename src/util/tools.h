#pragma once

#include <time.h>
#include <string>

// Random number generator. Use int random(highest,lowest);
extern int random(int newLowest, int newHighest);

// Error reporting
extern void abort_on_error(const std::string& message);

// Area clicked
extern bool locationClicked(int min_x, int max_x, int min_y, int max_y);
