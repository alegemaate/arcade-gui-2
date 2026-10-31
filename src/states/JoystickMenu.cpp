#include "JoystickMenu.h"

#include <string>

#include "../Controls.h"
#include "../globals.h"

namespace {
using asw::input::ControllerButton;
using asw::input::Key;

// Ship speed in pixels per second
constexpr float SHIP_SPEED = 120.0F;

// Ship stays within this distance of the screen centre
constexpr float SHIP_RANGE = 298.0F;

// Offset of the stick images from the bottom centre of the screen
constexpr float STICK_X = -306.0F;
constexpr float STICK_Y = -553.0F;

// A cabinet button, lit when its key or controller button is down
struct CabinetButton {
  Key key;
  ControllerButton button;

  // Offset from the bottom centre of the screen
  float x;
  float y;
};

const std::array<CabinetButton, 8> BUTTONS = {
    {
        {.key = Key::Num1, .button = ControllerButton::A, .x = -78, .y = -424},
        {.key = Key::Num2, .button = ControllerButton::B, .x = 30, .y = -440},
        {.key = Key::Num3, .button = ControllerButton::X, .x = 135, .y = -427},
        {.key = Key::Num4, .button = ControllerButton::Y, .x = -81, .y = -491},
        {
            .key = Key::Num5,
            .button = ControllerButton::RightShoulder,
            .x = 20,
            .y = -513,
        },
        {
            .key = Key::Num6,
            .button = ControllerButton::LeftShoulder,
            .x = 122,
            .y = -492,
        },
        {
            .key = Key::Num7,
            .button = ControllerButton::Back,
            .x = 181,
            .y = -551,
        },
        {
            .key = Key::Num8,
            .button = ControllerButton::Start,
            .x = 251,
            .y = -546,
        },
    },
};

bool button_down(const CabinetButton& button) {
  return asw::input::get_key(button.key) ||
         asw::input::get_controller_button(asw::input::ANY_CONTROLLER,
                                           button.button);
}

// Image index for the direction held, -1 when centred. Starts at up left and
// goes clockwise.
int stick_direction() {
  const bool up = asw::input::get_action(controls::UI_UP);
  const bool down = asw::input::get_action(controls::UI_DOWN);
  const bool left = asw::input::get_action(controls::UI_LEFT);
  const bool right = asw::input::get_action(controls::UI_RIGHT);

  if (up && left) {
    return 0;
  }
  if (up && right) {
    return 2;
  }
  if (down && right) {
    return 4;
  }
  if (down && left) {
    return 6;
  }
  if (up) {
    return 1;
  }
  if (right) {
    return 3;
  }
  if (down) {
    return 5;
  }
  if (left) {
    return 7;
  }
  return -1;
}
}  // namespace

void JoystickMenu::init() {
  cursor = asw::assets::load_texture("assets/images/cursor.png");
  joystick_background = asw::assets::load_texture(
      "assets/images/joystick/joystick_background.png");
  ship = asw::assets::load_texture("assets/images/joystick/ship.png");
  joystick_background2 =
      asw::assets::load_texture("assets/images/joystick/background.png");

  main_bg.setPreset({
      .color = asw::Vec3f(0.0F, 10.0F, 20.0F),
      .color_direction = asw::Vec3f(1.0F, 1.0F, 0.0F),
      .overlay_image = asw::assets::load_texture("assets/images/overlay.png"),
  });

  // Minigame
  ship_position = asw::Vec2f(SCREEN_W / 2, SCREEN_H - 630);

  for (int i = 0; i < BUTTON_COUNT; i++) {
    const auto number = std::to_string(i + 1);
    img_joystick_button.at(i) = asw::assets::load_texture(
        "assets/images/joystick/joystick_button_" + number + ".png");
    img_joystick_positions.at(i) = asw::assets::load_texture(
        "assets/images/joystick/joystick_pos_" + number + ".png");
  }
}

void JoystickMenu::update(float dt) {
  const float ship_width = asw::util::get_texture_size(ship).x;
  const float step = SHIP_SPEED * dt;

  // Move ship in minigame
  if (asw::input::get_action(controls::UI_LEFT) &&
      ship_position.x > (SCREEN_W / 2) - SHIP_RANGE) {
    ship_position.x -= step;
  } else if (asw::input::get_action(controls::UI_RIGHT) &&
             ship_position.x < (SCREEN_W / 2) + SHIP_RANGE - ship_width) {
    ship_position.x += step;
  }

  // Change background colour
  main_bg.changeColors(dt);

  if (asw::input::get_action_down(controls::UI_BACK)) {
    manager.set_next_scene(StateId::Menu);
  }
}

void JoystickMenu::drawStick() const {
  const int direction = stick_direction();

  if (direction >= 0) {
    asw::draw::sprite(img_joystick_positions.at(direction),
                      asw::Vec2f((SCREEN_W / 2) + STICK_X, SCREEN_H + STICK_Y));
  }
}

void JoystickMenu::draw() {
  // Draw background
  main_bg.draw();

  // Cabinet
  const auto panel_size = asw::util::get_texture_size(joystick_background);
  const auto screen_size = asw::util::get_texture_size(joystick_background2);

  asw::draw::sprite(joystick_background2,
                    asw::Vec2f((SCREEN_W / 2) - (screen_size.x / 2),
                               SCREEN_H - (panel_size.y + screen_size.y)));
  asw::draw::sprite(
      joystick_background,
      asw::Vec2f((SCREEN_W / 2) - (panel_size.x / 2), SCREEN_H - panel_size.y));

  // Buttons
  for (int i = 0; i < BUTTON_COUNT; i++) {
    const auto& button = BUTTONS.at(i);

    if (button_down(button)) {
      asw::draw::sprite(
          img_joystick_button.at(i),
          asw::Vec2f((SCREEN_W / 2) + button.x, SCREEN_H + button.y));
    }
  }

  drawStick();

  // Minigame
  asw::draw::sprite(ship, ship_position);

  // When using joystick, dont show mouse
  if (!controls::using_controller()) {
    asw::draw::sprite(cursor, asw::input::get_mouse().position);
  }
}
