#ifndef INIT_H_
#define INIT_H_

#include "GameState.h"

class Init : public GameState {
 public:
  // Main loop functions
  Init();
  void update();
  void draw() {}
  ~Init(){};
};

#endif  // INIT_H_
