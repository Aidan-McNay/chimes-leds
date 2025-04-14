#include "interface/switch.h"
#include "hardware/gpio.h"

Switch::Switch ( int gpio_pin ): gpio_pin( gpio_pin )
{
    // Initialize GPIO pin
    gpio_init( gpio_pin );
    gpio_set_dir( gpio_pin, GPIO_IN );
    gpio_pull_down( gpio_pin );
}

bool Switch::enabled()
{
    return gpio_get( gpio_pin );
}