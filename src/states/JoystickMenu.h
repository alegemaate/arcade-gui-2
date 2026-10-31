#pragma once

#include <asw/asw.h>

#include <array>

#include "../ColorBackground.h"
#include "StateId.h"

class JoystickMenu : public asw::scene::Scene<StateId> {
 public:
  using asw::scene::Scene<StateId>::Scene;

  void init() override;
  void update(float dt) override;
  void draw() override;

 private:
  /**
   * @brief Draw the stick in the direction held
   *
   */
  void drawStick() const;

  static constexpr int BUTTON_COUNT = 8;

  asw::Texture cursor;
  asw::Texture joystick_background;
  std::array<asw::Texture, BUTTON_COUNT> img_joystick_button;
  std::array<asw::Texture, BUTTON_COUNT> img_joystick_positions;

  // Minigame
  asw::Texture ship;
  asw::Texture joystick_background2;
  asw::Vec2f ship_position;

  ColorBackground main_bg;
};
