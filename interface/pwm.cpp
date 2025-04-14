// =======================================================================
// pwm.cpp
// =======================================================================
// Definitions of the PWM functions

#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pwm.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

PWM::PWM( int gpio_num, float clkdiv, int wrap, int duty_count )
{
  gpio_set_function( gpio_num, GPIO_FUNC_PWM );

  // Get the slice for the particular GPIO
  slice_num = pwm_gpio_to_slice_num( gpio_num );

  // Initialize with wrap/duty_count
  pwm_set_wrap( slice_num, wrap );
  pwm_set_clkdiv( slice_num, clkdiv );
  pwm_set_chan_level( slice_num, PWM_CHAN_A, duty_count );

  // Invert the output (only channel A for now)
  pwm_set_output_polarity( slice_num, true, false );
}

void PWM::start()
{
  // Start the PWM
  pwm_set_enabled( slice_num, true );
}

// ----------------------------------------------------------------------
// set_duty
// ----------------------------------------------------------------------

void PWM::set_duty( int duty_count )
{
  pwm_set_chan_level( slice_num, PWM_CHAN_A, duty_count );
}
