#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "adc/adc.h"
#include "utils/fix15.h"

class Microphone {
 public:
  Microphone( int gpio_pin = 26 );
  fix15 sample();
 
 private:
  ADC adc;
};

#endif