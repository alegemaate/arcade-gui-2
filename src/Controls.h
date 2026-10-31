#pragma once

// Input actions shared by keyboard and controllers
namespace controls {

inline constexpr const char* UI_UP = "ui_up";
inline constexpr const char* UI_DOWN = "ui_down";
inline constexpr const char* UI_LEFT = "ui_left";
inline constexpr const char* UI_RIGHT = "ui_right";
inline constexpr const char* UI_CONFIRM = "ui_confirm";
inline constexpr const char* UI_BACK = "ui_back";
inline constexpr const char* UI_SETTINGS = "ui_settings";

// Bind menu actions, call once after asw::core::init
void bind_ui();

// Check if the player last used a controller, the cursor is hidden then
bool using_controller();

}  // namespace controls
