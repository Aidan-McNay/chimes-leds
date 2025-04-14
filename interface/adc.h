#ifndef ADC_H
#define ADC_H
#include "utils/fix15.h"

class ADC {
 public:
  ADC(int gpio_pin);


  fix15 sample();
};


#endif