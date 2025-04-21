
#ifndef HEARTBEAT_H
#define HEARTBEAT_H
#include "interface/led.h"

class Heartbeat {
 public:
  Heartbeat( int gpio_pin, int duration );

  void step();

  void ping();

 protected:
  LED status;
  int duration;
  volatile int counter;
};


#endif