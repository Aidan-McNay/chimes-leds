#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "adc/adc.h"
#include "utils/fix15.h"

// ----------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------

ADC::ADC( int gpio_pin )
{
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Setup Analog pin/ADC
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  adc_gpio_init( gpio_pin );

  // Initialize the ADC harware
  // (resets it, enables the clock, spins until the hardware is ready)
  adc_init();

  // Select analog mux input (0...3 are GPIO 26, 27, 28, 29; 4 is temp
  // sensor)
  switch ( gpio_pin ) {
    case 26:
      adc_select_input( 0 );
      break;
    case 27:
      adc_select_input( 1 );
      break;
    case 28:
      adc_select_input( 2 );
      break;
    case 29:
      adc_select_input( 3 );
      break;
    default:
      adc_select_input( 4 );
      break;
  }

  // Start freerunning - we don't want to have to wait for sample
  // conversion
  adc_run( true );
}

// ----------------------------------------------------------------------
// ADC::sample
// ----------------------------------------------------------------------

fix15 ADC::sample() {
  // Get the latest conversion (don't use adc_read, may be blocking)
  uint16_t adc_val = (uint16_t) adc_hw->result;  // Max of 4095 (12b ADC)

  int adc_val_int = (int)(adc_val);
  return divfix15(int2fix15(adc_val_int), int2fix15(4095));
}
