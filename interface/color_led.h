#ifndef RGBW_LED_H
#define RGBW_LED_H

#include "interface/pwm.h"
#include "utils/color.h"

class ColorLED {
 public:
  ColorLED( int red_pin, int green_pin, int blue_pin, int white_pin );

  void set_color(color_rgb c);

  void toggle(bool on);

 protected:
  PWM red, green, blue, white;
};

#endif