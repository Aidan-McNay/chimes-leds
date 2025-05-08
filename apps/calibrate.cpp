
// Standard C libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// High-level pico libraries
#include "pico/stdlib.h"
#include "pico/multicore.h"
// Interface library to sys_clock (SDK 2.0.0 )
#include "hardware/clocks.h"
// Protothreads
#include "utils/pt_cornell_rp2040_v1.h"
#include "interface/color_led.h"
#include "interface/led.h"
#include "interface/pico_led.h"

#include <stdio.h>

#define RED_PIN 18
#define GREEN_PIN 20
#define BLUE_PIN 26
#define WHITE_PIN 28

ColorLED led(true, RED_PIN, GREEN_PIN, BLUE_PIN, WHITE_PIN);

color_rgb balanced = {128, 128, 128};


static PT_THREAD( protothread_input( struct pt *pt ) )
{
  PT_BEGIN( pt );
  while (1) {
    sprintf(pt_serial_out_buffer, "Enter new RGB values (r, g, b): ");
    serial_write;
    serial_read;
    // Read %d, %d, %d as the new RGB values
    int r, g, b;
    if (!sscanf(pt_serial_in_buffer, "%d, %d, %d", &r, &g, &b)) {
      sprintf(pt_serial_out_buffer, "Invalid input. Please enter three integers separated by commas.\n");
      serial_write;
    } else {
      // If the input is invalid, print an error message
      balanced.red = r;
      balanced.green = g;
      balanced.blue = b;
      sprintf(pt_serial_out_buffer, "New RGB values: %d, %d, %d\n", balanced.red, balanced.green, balanced.blue);
      serial_write;
    }

  }
  PT_END( pt );
}

static PT_THREAD( protothread_blink (struct pt *pt) )
{
  PT_BEGIN( pt );
  
  while (1) {
    // Blink the LED
    led.set_rgbw(balanced.red, balanced.green, balanced.blue, 0);
    PT_YIELD_usec(500000);
    led.set_rgbw(0, 0, 0, 128);
    PT_YIELD_usec(500000);
  }
  PT_END( pt );
}

// Main for core 0
int main() {
  // Initialize stdio
  stdio_init_all();

  int rc = pico_led_init();
  hard_assert( rc == PICO_OK );
  pico_set_led( true );

  gpio_set_function( 0, UART_FUNCSEL_NUM( uart0, 0 ) );
  gpio_set_function( 1, UART_FUNCSEL_NUM( uart0, 1 ) );

  uart_init( uart0, 115200 );

  uart_puts( uart0, "Initialized calibration.\n" );

  pt_add_thread(protothread_input);
  pt_add_thread(protothread_blink);
  
  pt_schedule_start ;
}
