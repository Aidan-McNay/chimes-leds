// =======================================================================
// pwm.h
// =======================================================================
// Declarations of the PWM functions

#ifndef PWM_H
#define PWM_H

class PWM {
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Public accessor functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 public:
  // Default:
  //  - 125MHz clk / clkdiv -> 5MHz PWM clock
  //  - wrap = 5000 -> Start cycle over every 5000 clock cycles
  //  - duty_count: Number of cycles to be high for
  PWM( int gpio_num, float clkdiv = 25.0, int wrap = 5000,
       int duty_count = 0 );
  void start();

  // Change the duty cycle (0 - wrap)
  void set_duty( int duty_count );

  uint slice_num;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Protected attributes
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 protected:
  void ( *on_interrupt_func )();
};

#endif  // PWM_H