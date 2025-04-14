#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "interface/adc.h"
#include "utils/fix15.h"

class Microphone {
 public:
  Microphone( int gpio_pin );
  fix15 sample();
 
 private:
  ADC adc;
};

#endif