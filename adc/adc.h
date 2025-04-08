#ifndef ADC_H
#define ADC_H
#include "utils/fix15.h"

class ADC {
 public:
  ADC(int gpio_pin = 26);


  fix15 sample();
};


#endif