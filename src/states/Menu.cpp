#include "Menu.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "../Controls.h"
#include "../Settings.h"
#include "../Textures.h"
#include "../adsgames/Api.h"
#include "../globals.h"
#include "../plugins/AdsGamesLoader.h"
#include "../plugins/MameLoader.h"

namespace {
// Built in entry that opens the joystick test
constexpr const char* JOYSTICK_PATH = "arcade://joystick";

// Built in entry that opens the settings
constexpr const char* SETTINGS_PATH = "arcade://settings";

// Press Start 2P is drawn on an 8 pixel grid, so sizes are multiples of 8
constexpr const char* FONT = "assets/fonts/PressStart2P-Regular.ttf";
constexpr float TITLE_SIZE = 48.0F;
constexpr float BODY_SIZE = 24.0F;
constexpr float SMALL_SIZE = 16.0F;

// Layout
constexpr float MARGIN = 96.0F;
constexpr float TITLE_Y = 96.0F;
constexpr float SUBTITLE_Y = 176.0F;

// Cards are 4:3, the shape of the art on A.D.S. Games
constexpr float CARD_W = 360.0F;
constexpr float CARD_H = 270.0F;
constexpr float CARD_SLOT = 470.0F;
constexpr float ROW_Y = 600.0F;
constexpr float FOCUS_SCALE = 1.4F;
constexpr float OUTLINE = 6.0F;
constexpr float OUTLINE_THICKNESS = 4.0F;
constexpr float SHADOW_OFFSET = 10.0F;
constexpr float DIM_ALPHA = 130.0F;

// Status under the focused card
constexpr float STATUS_GAP = 40.0F;
constexpr float PILL_PAD_X = 28.0F;
constexpr float PILL_PAD_Y = 16.0F;
constexpr float BAR_GAP = 24.0F;
constexpr float BAR_HEIGHT = 12.0F;
constexpr float PERCENT = 100.0F;

// Hints along the bottom
constexpr float HINTS_TOP = 1000.0F;
constexpr float HINTS_Y = 1032.0F;

// Motion, per second
constexpr float SCROLL_SPEED = 12.0F;
constexpr float FADE_SPEED = 3.0F;

// Holding a direction moves on once the row is this close
constexpr float SETTLE_DISTANCE = 0.25F;

// How strongly the focused game's art shows through
constexpr float BACKDROP_ALPHA = 0.3F;

// Colours, functions since asw::Color can not be constexpr
asw::Color white() {
  return {255, 255, 255};
}
asw::Color black() {
  return {0, 0, 0};
}
asw::Color muted() {
  return {190, 196, 210};
}
asw::Color card_bg() {
  return {16, 16, 24};
}
asw::Color failed() {
  return {255, 110, 110};
}

// Largest area with the texture's shape that fits inside an area, centred
asw::Quadf fit(const asw::Texture& texture, const asw::Quadf& area) {
  const auto size = asw::util::get_texture_size(texture);

  if (size.x <= 0 || size.y <= 0) {
    return area;
  }

  const float scale = std::min(area.size.x / size.x, area.size.y / size.y);
  const float width = size.x * scale;
  const float height = size.y * scale;

  return {area.position.x + ((area.size.x - width) / 2),
          area.position.y + ((area.size.y - height) / 2), width, height};
}

// Fill an area with a texture, cropping the edges that do not fit
void draw_cover(const asw::Texture& texture, const asw::Quadf& dest) {
  const auto size = asw::util::get_texture_size(texture);

  if (size.x <= 0 || size.y <= 0) {
    return;
  }

  const float dest_ratio = dest.size.x / dest.size.y;
  asw::Quadf source(0, 0, size.x, size.y);

  if (size.x / size.y > dest_ratio) {
    source.size.x = size.y * dest_ratio;
    source.position.x = (size.x - source.size.x) / 2;
  } else {
    source.size.y = size.x / dest_ratio;
    source.position.y = (size.y - source.size.y) / 2;
  }

  asw::draw::stretch_sprite_blit(texture, source, dest);
}

// Check if the left mouse button is down inside an area
bool area_clicked(const asw::Quadf& area) {
  return asw::input::get_mouse_button(asw::input::MouseButton::Left) &&
         area.contains(asw::input::get_mouse().position);
}
}  // namespace

void Menu::init() {
  cursor = asw::assets::load_texture("assets/images/cursor.png");
  fade = textures::create_fade();
  title_font = asw::assets::load_font(FONT, TITLE_SIZE, asw::FontStyle::Pixel);
  body_font = asw::assets::load_font(FONT, BODY_SIZE, asw::FontStyle::Pixel);
  small_font = asw::assets::load_font(FONT, SMALL_SIZE, asw::FontStyle::Pixel);

  main_bg.setPreset({
      .color = asw::Vec3f(0.0F, 100.0F, 200.0F),
      .color_direction = asw::Vec3f(1.0F, 1.0F, 0.0F),
      .overlay_image = asw::assets::load_texture("assets/images/overlay.png"),
  });

  // Load games
  directory = GameDirectory();
  directory.loadPlugin(std::make_shared<AdsGamesLoader>());

  // Scans in the background when the roms changed, the list fills in after
  mame_library.refresh(Settings::load().mame_rom_path);
  directory.loadPlugin(std::make_shared<MameLoader>(mame_library));

  directory.addGame({
      .name = "Joystick",
      .path = JOYSTICK_PATH,
      .icon =
          asw::assets::load_texture("assets/images/icons/icon_joystick.png"),
      .release = std::nullopt,
      .subtitle = "Controller test",
      .backdrop = nullptr,
      .command = {},
  });
  directory.addGame({
      .name = "Settings",
      .path = SETTINGS_PATH,
      .icon = nullptr,
      .release = std::nullopt,
      .subtitle = "MAME rom folder",
      .backdrop = nullptr,
      .command = {},
  });

  scroll = 0;
  setFocus(0);
}

void Menu::setFocus(int focus) {
  const auto& games = directory.listGames();

  if (games.empty()) {
    return;
  }

  game_focus = std::clamp(focus, 0, static_cast<int>(games.size()) - 1);
  backdrop_fade = 0;

  const auto& release = games.at(game_focus).release;
  focused_installed =
      release && std::filesystem::exists(adsgames::install_dir(*release));
}

void Menu::update(float dt) {
  // The menu sits hidden while a game runs
  if (game_running) {
    if (game_process.isRunning()) {
      return;
    }

    game_running = false;
    restoreWindow();
  }

  // Downloaded game is ready
  if (auto executable = installer.takeExecutable()) {
    focused_installed = true;

    // Games find their assets next to the executable, run from there anyway
    runGame({executable->string()}, executable->parent_path());
    return;
  }

  // Games found in the background, keep the same game focused
  const auto& before = directory.listGames();
  const auto focused_name =
      before.empty() ? std::string() : before.at(game_focus).name;

  if (directory.update()) {
    const auto& games = directory.listGames();
    const auto found = std::ranges::find(games, focused_name, &Game::name);
    const int index = found != games.end()
                          ? static_cast<int>(found - games.begin())
                          : game_focus;

    scroll += static_cast<float>(index - game_focus);
    setFocus(index);
  }

  // Ease the row towards the focused game
  const auto focus = static_cast<float>(game_focus);
  scroll += (focus - scroll) * std::min(1.0F, SCROLL_SPEED * dt);
  backdrop_fade = std::min(1.0F, backdrop_fade + (FADE_SPEED * dt));

  const auto game_count = static_cast<int>(directory.listGames().size());
  const bool settled = std::abs(focus - scroll) < SETTLE_DISTANCE;

  if (game_count > 0 && !installer.isBusy()) {
    const bool card_clicked =
        asw::input::get_mouse_button_down(asw::input::MouseButton::Left) &&
        focusedCard().contains(asw::input::get_mouse().position);

    if (card_clicked || asw::input::get_action_down(controls::UI_CONFIRM)) {
      launchFocusedGame();
    }

    // Holding a direction keeps browsing once the row catches up
    const float side = SCREEN_W / 4;

    if (settled && game_focus > 0 &&
        (asw::input::get_action(controls::UI_LEFT) ||
         area_clicked(asw::Quadf(0, 0, side, SCREEN_H)))) {
      setFocus(game_focus - 1);
    } else if (settled && game_focus < game_count - 1 &&
               (asw::input::get_action(controls::UI_RIGHT) ||
                area_clicked(asw::Quadf(SCREEN_W - side, 0, side, SCREEN_H)))) {
      setFocus(game_focus + 1);
    }
  }

  // Change background colour
  main_bg.changeColors(dt);

  if (asw::input::get_action_down(controls::UI_SETTINGS)) {
    manager.set_next_scene(StateId::Settings);
  }

  if (asw::input::get_action_down(controls::UI_BACK)) {
    asw::core::exit();
  }
}

void Menu::launchFocusedGame() {
  const auto& focused_game = directory.listGames().at(game_focus);

  if (focused_game.path == JOYSTICK_PATH) {
    manager.set_next_scene(StateId::Joystick);
    return;
  }

  if (focused_game.path == SETTINGS_PATH) {
    manager.set_next_scene(StateId::Settings);
    return;
  }

  if (focused_game.release) {
    asw::log::info("Installing game: {}", focused_game.name);
    installer.start(*focused_game.release);
    return;
  }

  if (!focused_game.command.empty()) {
    runGame(focused_game.command, {});
  }
}

void Menu::runGame(std::vector<std::string> args,
                   const std::filesystem::path& working_dir) {
  asw::log::info("Launching game: {}", args.back());

  if (!game_process.start(std::move(args), working_dir)) {
    return;
  }

  // Step aside so the game has the screen
  game_running = true;
  asw::display::set_fullscreen(false);
  SDL_HideWindow(asw::display::get_window());
}

void Menu::restoreWindow() {
  auto* window = asw::display::get_window();
  SDL_ShowWindow(window);
  asw::display::set_fullscreen(true);
  SDL_RaiseWindow(window);
}

asw::Quadf Menu::focusedCard() const {
  const float offset = static_cast<float>(game_focus) - scroll;
  const float width = CARD_W * FOCUS_SCALE;
  const float height = CARD_H * FOCUS_SCALE;

  return {(SCREEN_W / 2) + (offset * CARD_SLOT) - (width / 2),
          ROW_Y - (height / 2), width, height};
}

void Menu::drawBackdrop() const {
  const auto& games = directory.listGames();

  if (!games.empty()) {
    const auto& backdrop = games.at(game_focus).backdrop;

    if (backdrop) {
      asw::draw::set_alpha(backdrop, backdrop_fade * BACKDROP_ALPHA);
      draw_cover(backdrop, asw::Quadf(0, 0, SCREEN_W, SCREEN_H));
      asw::draw::set_alpha(backdrop, 1.0F);
    }
  }

  // Keep the text readable over any art
  constexpr float TOP_BAND = 320.0F;
  constexpr float BOTTOM_BAND_START = 360.0F;
  constexpr float TOP_ALPHA = 0.75F;
  constexpr float BOTTOM_ALPHA = 0.9F;

  const auto fade_size = asw::util::get_texture_size(fade);
  const asw::Quadf fade_source(0, 0, fade_size.x, fade_size.y);

  // Dark at the top edge, clear below
  asw::draw::set_alpha(fade, TOP_ALPHA);
  asw::draw::stretch_sprite_rotate_blit(
      fade, fade_source, asw::Quadf(0, 0, SCREEN_W, TOP_BAND), 0, false, true);

  // Clear in the middle, dark at the bottom edge
  asw::draw::set_alpha(fade, BOTTOM_ALPHA);
  asw::draw::stretch_sprite(fade, asw::Quadf(0, BOTTOM_BAND_START, SCREEN_W,
                                             SCREEN_H - BOTTOM_BAND_START));
}

void Menu::drawHeader() const {
  const auto& games = directory.listGames();

  if (games.empty()) {
    asw::draw::text(title_font, "No games", asw::Vec2f(MARGIN, TITLE_Y),
                    white());
    return;
  }

  const auto& game = games.at(game_focus);

  asw::draw::text_shadow(title_font, game.name, asw::Vec2f(MARGIN, TITLE_Y),
                         white(), black(), asw::Vec2f(4, 4));

  std::string subtitle = game.subtitle;
  if (focused_installed) {
    subtitle += "  Installed";
  }

  asw::draw::text_shadow(body_font, subtitle, asw::Vec2f(MARGIN, SUBTITLE_Y),
                         muted(), black(), asw::Vec2f(2, 2));

  const auto counter =
      std::to_string(game_focus + 1) + " / " + std::to_string(games.size());
  asw::draw::text_shadow(body_font, counter,
                         asw::Vec2f(SCREEN_W - MARGIN, TITLE_Y), muted(),
                         black(), asw::Vec2f(2, 2), asw::TextJustify::Right);

  if (mame_library.isScanning()) {
    asw::draw::text_shadow(small_font, "Scanning MAME roms",
                           asw::Vec2f(SCREEN_W - MARGIN, SUBTITLE_Y), muted(),
                           black(), asw::Vec2f(2, 2), asw::TextJustify::Right);
  }
}

void Menu::drawCards() const {
  const auto& games = directory.listGames();
  const int count = static_cast<int>(games.size());

  // Nearest last, so the focused card sits on top
  const int nearest = std::clamp(static_cast<int>(std::lround(scroll)), 0,
                                 std::max(0, count - 1));
  const auto draw_card = [&](int i) {
    const float offset = static_cast<float>(i) - scroll;
    const float x = (SCREEN_W / 2) + (offset * CARD_SLOT);

    if (std::abs(x - (SCREEN_W / 2)) > (SCREEN_W / 2) + CARD_W) {
      return;
    }

    // 1 when centred, 0 a card away or more
    const float closeness = std::max(0.0F, 1.0F - std::abs(offset));
    const float scale = 1.0F + ((FOCUS_SCALE - 1.0F) * closeness);
    const float width = CARD_W * scale;
    const float height = CARD_H * scale;
    const asw::Quadf card(x - (width / 2), ROW_Y - (height / 2), width, height);

    asw::draw::rect_fill(
        asw::Quadf(card.position.x + SHADOW_OFFSET,
                   card.position.y + SHADOW_OFFSET, width, height),
        asw::Color(0, 0, 0, 120));
    asw::draw::rect_fill(card, card_bg());

    const auto& icon = games.at(i).icon;
    if (icon) {
      asw::draw::stretch_sprite(icon, fit(icon, card));
    } else {
      // No art, show the name instead
      const auto centre = card.get_center();
      asw::draw::text(body_font, games.at(i).name,
                      asw::Vec2f(centre.x, centre.y - (BODY_SIZE / 2)), white(),
                      asw::TextJustify::Center);
    }

    // Cards away from the centre fade back
    const auto dim = static_cast<uint8_t>(DIM_ALPHA * (1.0F - closeness));
    asw::draw::rect_fill(card, asw::Color(0, 0, 0, dim));

    if (closeness > 0) {
      const auto outline = static_cast<uint8_t>(255.0F * closeness);
      asw::draw::rect(
          asw::Quadf(card.position.x - OUTLINE, card.position.y - OUTLINE,
                     width + (OUTLINE * 2), height + (OUTLINE * 2)),
          asw::Color(255, 255, 255, outline), OUTLINE_THICKNESS);
    }
  };

  for (int i = 0; i < count; i++) {
    if (i != nearest) {
      draw_card(i);
    }
  }

  if (count > 0) {
    draw_card(nearest);
  }
}

void Menu::drawStatus() const {
  using State = adsgames::Installer::State;

  const auto& games = directory.listGames();
  if (games.empty()) {
    return;
  }

  const auto card = focusedCard();
  const float top = card.position.y + card.size.y + STATUS_GAP;
  const float centre = SCREEN_W / 2;
  const auto state = installer.getState();

  if (state == State::Downloading) {
    const float progress = installer.getProgress();
    const auto percent = static_cast<int>(progress * PERCENT);

    asw::draw::text_shadow(body_font,
                           "Downloading " + std::to_string(percent) + "%",
                           asw::Vec2f(centre, top), white(), black(),
                           asw::Vec2f(2, 2), asw::TextJustify::Center);

    const float bar_top = top + BODY_SIZE + BAR_GAP;
    asw::draw::rect_fill(
        asw::Quadf(card.position.x, bar_top, card.size.x, BAR_HEIGHT),
        asw::Color(0, 0, 0, 160));
    asw::draw::rect_fill(asw::Quadf(card.position.x, bar_top,
                                    card.size.x * progress, BAR_HEIGHT),
                         white());
    return;
  }

  if (state == State::Installing) {
    asw::draw::text_shadow(body_font, "Installing", asw::Vec2f(centre, top),
                           white(), black(), asw::Vec2f(2, 2),
                           asw::TextJustify::Center);
    return;
  }

  if (state == State::Failed) {
    asw::draw::text_shadow(body_font, "Download failed",
                           asw::Vec2f(centre, top), failed(), black(),
                           asw::Vec2f(2, 2), asw::TextJustify::Center);
    return;
  }

  // What confirm does, as a button
  const auto& game = games.at(game_focus);
  std::string action = "Play";
  if (game.path == JOYSTICK_PATH || game.path == SETTINGS_PATH) {
    action = "Open";
  } else if (game.release && !focused_installed) {
    action = "Download";
  }

  const auto size = asw::util::get_text_size(body_font, action);
  const float pill_w = static_cast<float>(size.x) + (PILL_PAD_X * 2);
  const float pill_h = static_cast<float>(size.y) + (PILL_PAD_Y * 2);

  asw::draw::rect_fill(asw::Quadf(centre - (pill_w / 2), top, pill_w, pill_h),
                       white());
  asw::draw::text(body_font, action, asw::Vec2f(centre, top + PILL_PAD_Y),
                  card_bg(), asw::TextJustify::Center);
}

void Menu::drawHints() const {
  asw::draw::rect_fill(asw::Quadf(0, HINTS_TOP, SCREEN_W, SCREEN_H - HINTS_TOP),
                       asw::Color(0, 0, 0, 150));

  const char* hints =
      controls::using_controller()
          ? "D-Pad Browse   A Play   Start Settings   Back Quit"
          : "Arrows Browse   Enter Play   Tab Settings   Esc Quit";

  asw::draw::text(small_font, hints, asw::Vec2f(SCREEN_W / 2, HINTS_Y), muted(),
                  asw::TextJustify::Center);
}

void Menu::draw() {
  main_bg.draw();
  drawBackdrop();
  drawHeader();
  drawCards();
  drawStatus();
  drawHints();

  // When using joystick, dont show mouse
  if (!controls::using_controller()) {
    asw::draw::sprite(cursor, asw::input::get_mouse().position);
  }
}
