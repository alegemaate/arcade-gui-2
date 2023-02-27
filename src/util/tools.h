#pragma once

#include <string>

// Error reporting
extern void abort_on_error(const std::string& message);

// Area clicked
extern bool locationClicked(int min_x, int max_x, int min_y, int max_y);
