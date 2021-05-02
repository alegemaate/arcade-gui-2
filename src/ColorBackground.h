#ifndef COLORBACKGROUND_H_
#define COLORBACKGROUND_H_

#include <allegro5/allegro.h>
#include <string>

#define BG_VIBRANT 0
#define BG_GRAYSCALE 1
#define BG_PASTEL 2
#define BG_BALANCED 3

class ColorBackground {
 public:
  ColorBackground(const std::string& overlay,
                  int preset_name = BG_BALANCED,
                  double speed = 0);
  virtual ~ColorBackground() {}

  void setupColors(int preset_name, double newSpeed);
  void changeColors();

  void draw();

 protected:
 private:
  // Background variables
  double background_r, background_g, background_b;
  bool background_r_up, background_g_up, background_b_up;
  double background_speed;

  ALLEGRO_BITMAP* overlay_image;
};

#endif  // COLORBACKGROUND_H_
