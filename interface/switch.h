#ifndef SWITCH_H
#define SWITCH_H

class Switch {
 public:
  Switch( int gpio_pin );

  bool enabled();

 protected:
  int gpio_pin;
};

#endif