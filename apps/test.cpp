
// Standard C libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// High-level pico libraries
#include "pico/stdlib.h"
#include "pico/multicore.h"
// Interface library to sys_clock (SDK 2.0.0 )
#include "hardware/clocks.h"
// CAN driver
#include "can/can.h"
// Protothreads
#include "utils/pt_cornell_rp2040_v1.h"
#include "interface/color_led.h"

#define RED_PIN 19
#define GREEN_PIN 20
#define BLUE_PIN 26
#define WHITE_PIN 28

ColorLED led(RED_PIN, GREEN_PIN, BLUE_PIN, WHITE_PIN);

// Main for core 0
int main() {
    // Initialize stdio
    stdio_init_all();

    // Set can bus callback
    color_rgb c = {128, 128, 128};
}
