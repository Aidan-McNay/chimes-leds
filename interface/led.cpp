#include "interface/led.h"
#include "hardware/gpio.h"

LED::LED ( int gpio_pin ): gpio_pin( gpio_pin )
{
    // Initialize GPIO pin
    gpio_init( gpio_pin );
    gpio_set_dir( gpio_pin, GPIO_OUT );
    gpio_put( gpio_pin, 0 );  // Set to low
}

void LED::toggle(bool state)
{
    gpio_put( gpio_pin, state );
}