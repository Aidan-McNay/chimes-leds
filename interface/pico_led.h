

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif
#include "pico/stdlib.h"


// Interact with the default LED on the Pico
int pico_led_init( void );

// Turn the led on or off
void pico_set_led( bool led_on );