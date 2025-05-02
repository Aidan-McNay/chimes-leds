#ifndef COLOR_ADC_H
#define COLOR_ADC_H
#include "hardware/dma.h"
#include "utils/color.h"

class ColorInput {
 public:
  ColorInput();


  color_rgb sample();
 protected:
  static const int red_chan = 2;
  static const int green_chan = 1;
  static const int blue_chan = 0;
  uint dma_chan;
  dma_channel_config cfg;
  uint16_t capture_buf[3] = {0};
};


#endif