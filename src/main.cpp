#include <asw/asw.h>

#include <exception>

#include "Controls.h"
#include "globals.h"
#include "states/JoystickMenu.h"
#include "states/Menu.h"
#include "states/SettingsMenu.h"
#include "states/StateId.h"

// Errors show a message box and exit, the box itself can not be caught
// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
  try {
    asw::core::init(static_cast<int>(SCREEN_W), static_cast<int>(SCREEN_H));
    asw::display::set_title("Arcade GUI");
    asw::display::set_fullscreen(true);

    // Menus draw their own cursor
    asw::input::set_cursor_visible(false);

    controls::bind_ui();

    auto app = asw::scene::SceneManager<StateId>();
    app.register_scene<Menu>(StateId::Menu, app);
    app.register_scene<JoystickMenu>(StateId::Joystick, app);
    app.register_scene<SettingsMenu>(StateId::Settings, app);
    app.set_next_scene(StateId::Menu);

    app.start();
  } catch (const std::exception& e) {
    asw::util::abort_on_error(e.what());
  }

  return 0;
}
