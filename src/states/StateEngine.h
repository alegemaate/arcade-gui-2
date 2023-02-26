#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "GameState.h"

#include "../util/tools.h"

#include "./JoystickMenu.h"
#include "./Menu.h"

enum class StateId {
  NONE,
  MENU,
  JOYSTICK,
  EXIT,
};

class StateEngine {
 public:
  void update() {
    try {
      changeState();
      state.get()->update();
    } catch (std::exception& e) {
      abort_on_error(e.what());
    } catch (...) {
      abort_on_error("Unknown error");
    };
  };

  void draw() {
    try {
      state.get()->draw();
    } catch (std::exception& e) {
      abort_on_error(e.what());
    } catch (...) {
      abort_on_error("Unknown error");
    };
  };

  void setState(const StateId stateId) { nextStateId = stateId; }

  bool isClosing() { return closing; }

 private:
  void changeState() {
    // If the state needs to be changed
    if (nextStateId == StateId::NONE) {
      return;
    }

    // Change the state
    switch (nextStateId) {
      case StateId::MENU:
        state = std::make_unique<Menu>();
        break;

      case StateId::JOYSTICK:
        state = std::make_unique<JoystickMenu>();
        break;

      case StateId::EXIT:
        closing = true;
        break;

      default:
        break;
    }

    // Change the current state ID
    stateId = nextStateId;

    // NULL the next state ID
    nextStateId = StateId::NONE;
  }

  StateId nextStateId = StateId::NONE;
  StateId stateId = StateId::NONE;

  std::unique_ptr<GameState> state;

  bool closing = false;
};
