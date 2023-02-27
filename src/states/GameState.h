#pragma once

#include "StateId.h"

// GameState, holds all levels
class GameState {
 public:
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual ~GameState() {}

  StateId getNextStateId() const { return nextStateId; }

 protected:
  void changeState(const StateId stateId) { nextStateId = stateId; }

 private:
  StateId nextStateId = StateId::NONE;
};
