#include "SettingsMenu.h"

#include <utility>

#include "../Controls.h"
#include "../globals.h"
#include "../mame/Library.h"

namespace {
constexpr const char* FONT = "assets/fonts/PressStart2P-Regular.ttf";
constexpr float TITLE_SIZE = 48.0F;
constexpr float BODY_SIZE = 24.0F;
constexpr float SMALL_SIZE = 16.0F;

constexpr float PANEL_W = 1280.0F;
constexpr float PANEL_H = 440.0F;
constexpr float PANEL_PADDING = 48.0F;
constexpr float CONTENT_W = PANEL_W - (PANEL_PADDING * 2);
constexpr float INPUT_H = 64.0F;
constexpr float BUTTON_H = 64.0F;
constexpr float BUTTON_PADDING = 20.0F;
constexpr float GAP = 20.0F;
constexpr float BUTTON_GAP = 24.0F;

constexpr uint8_t PANEL_ALPHA = 200;
}  // namespace

void SettingsMenu::init() {
  settings = Settings::load();

  cursor = asw::assets::load_texture("assets/images/cursor.png");
  title_font = asw::assets::load_font(FONT, TITLE_SIZE, asw::FontStyle::Pixel);
  body_font = asw::assets::load_font(FONT, BODY_SIZE, asw::FontStyle::Pixel);
  small_font = asw::assets::load_font(FONT, SMALL_SIZE, asw::FontStyle::Pixel);

  main_bg.setPreset({
      .color = asw::Vec3f(40.0F, 0.0F, 120.0F),
      .color_direction = asw::Vec3f(1.0F, 0.0F, 1.0F),
      .overlay_image = asw::assets::load_texture("assets/images/overlay.png"),
  });

  // Widgets are made again each time the screen opens
  ui.root.clear_children();
  ui.clear_focus();
  ui.ctx.theme.font = body_font;
  ui.ctx.theme.button.bg = asw::Color(255, 255, 255, 30);
  ui.ctx.theme.button.bg_hover = asw::Color(255, 255, 255, 90);
  ui.ctx.theme.button.bg_pressed = asw::Color(255, 255, 255, 140);

  ui.ctx.navigation.up = controls::UI_UP;
  ui.ctx.navigation.down = controls::UI_DOWN;
  ui.ctx.navigation.left = controls::UI_LEFT;
  ui.ctx.navigation.right = controls::UI_RIGHT;
  ui.ctx.navigation.activate = controls::UI_CONFIRM;
  ui.ctx.navigation.back = controls::UI_BACK;
  ui.on_back = [this] { close(); };

  auto& panel = ui.root.add_child<asw::ui::Panel>();
  panel.bg = asw::Color(0, 0, 0, PANEL_ALPHA);
  panel.transform.size = asw::Vec2f(PANEL_W, PANEL_H);
  panel.anchor = asw::ui::Anchor::Center;

  auto& column = panel.add_child<asw::ui::Stack>();
  column.gap = GAP;
  column.padding = PANEL_PADDING;
  column.transform.size = asw::Vec2f(PANEL_W, PANEL_H);

  // Positions are absolute, so the column follows the panel by anchoring too.
  // Clicks only reach widgets inside their parent.
  column.anchor = asw::ui::Anchor::Center;

  auto& title = column.add_child<asw::ui::Label>();
  title.font = title_font;
  title.set_text("Settings", true);

  auto& heading = column.add_child<asw::ui::Label>();
  heading.set_text("MAME rom folder", true);

  auto& input = column.add_child<asw::ui::InputBox>();
  input.transform.size = asw::Vec2f(CONTENT_W, INPUT_H);
  input.value = settings.mame_rom_path;
  input.placeholder = "Empty uses MAME's own rompath";
  input.on_change = [this](const std::string& value) {
    settings.mame_rom_path = value;
  };
  rom_input = &input;

  auto& hint = column.add_child<asw::ui::Label>();
  hint.font = small_font;
  hint.color = asw::Color(190, 196, 210);
  hint.set_text(
      "Where your rom zips are, e.g. ~/mame/roms. Separate folders "
      "with ;",
      true);

  auto& row = column.add_child<asw::ui::Stack>();
  row.direction = asw::ui::Direction::Horizontal;
  row.align = asw::ui::Align::Start;
  row.gap = BUTTON_GAP;
  row.transform.size = asw::Vec2f(CONTENT_W, BUTTON_H);

  const auto add_button = [&row](const std::string& text,
                                 std::function<void()> on_click) {
    auto& button = row.add_child<asw::ui::Button>();
    button.padding = BUTTON_PADDING;
    button.set_text(text, true);
    button.on_click = std::move(on_click);
    return &button;
  };

  auto* browse_button = add_button("Browse", [this] { browse(); });
  add_button("Clear", [this] {
    rom_input->value.clear();
    settings.mame_rom_path.clear();
  });
  add_button("Rescan roms", [this] {
    mame::Library::clearCache();
    setStatus("Roms will be scanned when you go back");
  });
  add_button("Back", [this] { close(); });

  auto& status_label = column.add_child<asw::ui::Label>();
  status_label.font = small_font;
  status_label.color = asw::Color(190, 196, 210);
  status_label.transform.size = asw::Vec2f(CONTENT_W, SMALL_SIZE);
  status = &status_label;

  // Controllers start on the buttons, typing needs a keyboard anyway
  ui.focus(*browse_button, controls::using_controller());
}

void SettingsMenu::update(float dt) {
  {
    const std::scoped_lock lock(chosen_mutex);
    if (chosen_folder) {
      rom_input->value = *chosen_folder;
      settings.mame_rom_path = *chosen_folder;
      chosen_folder.reset();
    }
  }

  ui.update();
  main_bg.changeColors(dt);
}

void SettingsMenu::draw() {
  main_bg.draw();
  ui.draw();

  if (!controls::using_controller()) {
    asw::draw::sprite(cursor, asw::input::get_mouse().position);
  }
}

void SettingsMenu::close() {
  if (!settings.save()) {
    asw::log::error("Could not save settings");
  }
  manager.set_next_scene(StateId::Menu);
}

void SettingsMenu::setStatus(const std::string& text) {
  status->set_text(text);
}

void SettingsMenu::browse() {
  const auto start = expand_home(settings.mame_rom_path);
  SDL_ShowOpenFolderDialog(onFolderChosen, this, asw::display::get_window(),
                           start.empty() ? nullptr : start.c_str(), false);
}

void SettingsMenu::onFolderChosen(void* userdata,
                                  const char* const* filelist,
                                  int /*filter*/) {
  if (filelist == nullptr || *filelist == nullptr) {
    return;
  }

  auto* menu = static_cast<SettingsMenu*>(userdata);
  const std::scoped_lock lock(menu->chosen_mutex);
  menu->chosen_folder = std::string(*filelist);
}
