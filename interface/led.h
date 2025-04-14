#ifndef LED_H
#define LED_H

class LED {
 public:
  LED( int gpio_pin );

  void toggle(bool state);

 protected:
  int gpio_pin;
};

#endif