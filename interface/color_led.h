#ifndef RGBW_LED_H
#define RGBW_LED_H

#include "interface/pwm.h"
#include "utils/color.h"

class ColorLED {
 public:
  ColorLED( bool rgbw, int red_pin, int green_pin, int blue_pin, int white_pin );

  void set_color(color_rgb c);

  void set_rgbw(int r, int g, int b, int w);

  void toggle(bool on);

 protected:
  bool rgbw;
  PWM red, green, blue, white;
};

#endif