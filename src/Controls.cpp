#include "Controls.h"

#include <asw/asw.h>

#include <string>

namespace {
using asw::input::ControllerAxis;
using asw::input::ControllerAxisBinding;
using asw::input::ControllerButton;
using asw::input::ControllerButtonBinding;
using asw::input::Key;
using asw::input::KeyBinding;

constexpr auto ANY = asw::input::ANY_CONTROLLER;

// Stick must pass this before it counts as a direction
constexpr float STICK_THRESHOLD = 0.6F;

void bind_direction(const std::string& name,
                    Key key,
                    ControllerButton dpad,
                    ControllerAxis axis,
                    bool positive) {
  asw::input::bind_action(name, KeyBinding{key});
  asw::input::bind_action(
      name, ControllerButtonBinding{.button = dpad, .controller_index = ANY});
  asw::input::bind_action(name, ControllerAxisBinding{
                                    .axis = axis,
                                    .controller_index = ANY,
                                    .threshold = STICK_THRESHOLD,
                                    .positive_direction = positive,
                                });
}
}  // namespace

void controls::bind_ui() {
  bind_direction(UI_UP, Key::Up, ControllerButton::DPadUp,
                 ControllerAxis::LeftY, false);
  bind_direction(UI_DOWN, Key::Down, ControllerButton::DPadDown,
                 ControllerAxis::LeftY, true);
  bind_direction(UI_LEFT, Key::Left, ControllerButton::DPadLeft,
                 ControllerAxis::LeftX, false);
  bind_direction(UI_RIGHT, Key::Right, ControllerButton::DPadRight,
                 ControllerAxis::LeftX, true);

  asw::input::bind_action(UI_CONFIRM, KeyBinding{Key::Return});
  asw::input::bind_action(UI_CONFIRM, ControllerButtonBinding{
                                          .button = ControllerButton::A,
                                          .controller_index = ANY,
                                      });
  asw::input::bind_action(UI_CONFIRM, ControllerButtonBinding{
                                          .button = ControllerButton::X,
                                          .controller_index = ANY,
                                      });

  asw::input::bind_action(UI_SETTINGS, KeyBinding{Key::Tab});
  asw::input::bind_action(UI_SETTINGS, ControllerButtonBinding{
                                           .button = ControllerButton::Start,
                                           .controller_index = ANY,
                                       });

  asw::input::bind_action(UI_BACK, KeyBinding{Key::Escape});
  asw::input::bind_action(UI_BACK, ControllerButtonBinding{
                                       .button = ControllerButton::Back,
                                       .controller_index = ANY,
                                   });
}

bool controls::using_controller() {
  return asw::input::get_last_device() == asw::input::InputDevice::Controller;
}
