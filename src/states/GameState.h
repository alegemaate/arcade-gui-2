#ifndef GAMESTATE_H_
#define GAMESTATE_H_

// GameState, holds all levels
class GameState {
 public:
  virtual void update() = 0;
  virtual void draw() = 0;
  virtual ~GameState() {}
};

#endif  // GAMESTATE_H_
