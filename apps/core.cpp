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
#include "utils/can_interface.h"
#include "utils/state.h"
#include "interface/switch.h"
#include "interface/color_adc.h"
#include "interface/led.h"
#include "interface/pwm.h"
#include "can/can.h"
#include "interface/pico_led.h"
#include "utils/fix15.h"

State core_state = State();

CAN can_bus( CORE_ARBITRATION, NETWORK_BROADCAST, 18, 22);

Switch system_enable(4); // whether to be on
Switch wifi_enable(5);   // whether to use WiFi configs
Switch light_mode(7);
Switch music_mode(8);
Switch on_mode(9);

ColorInput color_in;

LED is_on(10);
PWM red_out(11, 125.0f, 255, 0);
PWM green_out(12, 125.0f, 255, 0);
PWM blue_out(14, 125.0f, 255, 0);
LED wifi_on(15);

void on_color_update(State *s) {
  color_rgb c = s->get_led_color().get_rgb();

  red_out.set_duty(c.red);
  green_out.set_duty(c.green);
  blue_out.set_duty(c.blue);

  unsigned short payload[4];

  payload[0] = SET_COLOR;
  payload[1] = c.red;
  payload[2] = c.green;
  payload[3] = c.blue;
  can_bus.set_payload(payload, 4);

  for (short clock_face : CLOCK_FACE_ARBITRATION) {
    can_bus.set_arbitration(clock_face);
    can_bus.sendPacket();
  }
}

void on_led_update(State *s) {
  bool on = s->get_led_on();

  unsigned short payload[2];

  payload[0] = SET_TOGGLE;
  payload[1] = on;
  can_bus.set_payload(payload, 2);
  for (short clock_face : CLOCK_FACE_ARBITRATION) {
    can_bus.set_arbitration(clock_face);
    can_bus.sendPacket();
  }
}

void on_mode_change(State *s) {
  // If the mode is set to const, turn on the LED
  if (s->get_led_mode() == CONST) {
    s->set_led_on(true);
  }

  // Otherwise, this will be handled elsewhere
}

// Read sensor data
fix15 last_light;
fix15 last_sound;

void read_packet( const unsigned short* packet, const unsigned char len ) {
  switch (packet[0]) {
    case SENSOR_LIGHT:
      // Right the light sensor data
      if (len != 3) {
        printf("Error: light packet length %d\n", len);
        return;
      }

      last_light = (packet[1] << 16) + packet[2];
      break;

    case SENSOR_SOUND:
      // Right the sound sensor data
      if (len != 3) {
        printf("Error: sound packet length %d\n", len);
        return;
      }

      last_sound = (packet[1] << 16) + packet[2];
      break;
    default:
      // Nothing to do here
      break;
  }
}

// us per sample
#define PARAM_SAMPLE_RATE 10000

// Lux cap for light sensor below which to turn on LEDs
#define LUX_CAP int2fix15(1)

static PT_THREAD( protothread_core( struct pt *pt ) )
{
  PT_BEGIN( pt );
  static int begin_time, elapsed_time;

  while (1) {
    // Measure time at start of thread
    begin_time = time_us_32();
    
    is_on.toggle(system_enable.enabled());
    wifi_on.toggle(wifi_enable.enabled());

    if (system_enable.enabled()) {

      switch (core_state.get_led_mode()) {
        case LIGHT:
          // Send light sensor data to watch faces
          if (last_light > LUX_CAP) {
            // Set the color to the light sensor value
            core_state.set_led_on(true);
          } else {
            // Turn off the LEDs
            core_state.set_led_on(false);
          }
          break;
        case MUSIC:
          break;
        case CONST:
          core_state.set_led_on(true);
          // Handle manual mode
          // Read red, green, and blue in values and set the color
          color_rgb c = color_in.sample();
          core_state.set_led_color(Color(c));
          break;
      }
    } else {
    // If the system is not enabled, turn off the LEDs
    core_state.set_led_on(false);
  }

    elapsed_time = time_us_32() - begin_time;

    // yield for necessary amount of time
    PT_YIELD_usec( PARAM_SAMPLE_RATE - elapsed_time );
  }
  PT_END( pt );
}

//                        USER INTERRUPT SERVICE ROUTINES
//
// ISR entered at the end of packet transmit.
void tx_handler() {
  can_bus.handle_tx() ;
}
// ISR entered when a packet is available for attempted receipt.
void rx_handler() {
  can_bus.handle_rx() ;
}

//                             MAIN FOR CORES 0 AND 1
//
// Main for core 1
void core1_main() {
    // CAN transmitter will run on core 1
    can_bus.setupCANTX(tx_handler) ;
    // Start the threader
    pt_schedule_start ;
}

// Main for core 0
int main() {
  // Initialize stdio
  stdio_init_all();
  
  int rc = pico_led_init();
  hard_assert(rc == PICO_OK);

  // Initialize state callbacks
  core_state.add_led_on_callback( on_led_update );
  core_state.add_led_mode_callback( on_mode_change );
  core_state.add_led_color_callback( on_color_update );

  red_out.toggle(true);
  green_out.toggle(true);
  blue_out.toggle(true);

  // start core 1 threads
  multicore_reset_core1();
  multicore_launch_core1(&core1_main);

  // Set can bus callback
  can_bus.set_callback( read_packet );

  // Setup the CAN receiver on core 0
  can_bus.setupCANRX(rx_handler) ;

  pt_add_thread( protothread_core );
  
  pico_set_led(true);

  pt_schedule_start ;
}
