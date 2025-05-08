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

CAN can_bus( CORE_ARBITRATION, NETWORK_BROADCAST, 16, 22);

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

  printf("Setting color to %d %d %d\n", c.red, c.green, c.blue);

  red_out.set_duty(c.red);
  green_out.set_duty(c.green);
  blue_out.set_duty(c.blue);

  unsigned short payload[4];

  payload[0] = SET_COLOR;
  payload[1] = c.red;
  payload[2] = c.green;
  payload[3] = c.blue;

  can_bus.set_payload(payload, 4);

  can_bus.set_arbitration(CLOCK_FACE_ARBITRATION[0]);
  can_bus.sendPacket();

  // for (short clock_face : CLOCK_FACE_ARBITRATION) {
  //   can_bus.set_arbitration(clock_face);
  //   can_bus.sendPacket();
  // }
}

void on_led_update(State *s) {
  bool on = s->get_led_on();

  unsigned short payload[2];

  payload[0] = SET_TOGGLE;
  payload[1] = on;
  can_bus.set_payload(payload, 2);
  
    can_bus.set_arbitration(CLOCK_FACE_ARBITRATION[0]);
    can_bus.sendPacket();
  // for (short clock_face : CLOCK_FACE_ARBITRATION) {
  //   can_bus.set_arbitration(clock_face);
  //   can_bus.sendPacket();
  // }
}

void on_mode_change(State *s) {
      unsigned short payload[1];
  switch (s->get_led_mode()) {
    case LIGHT:
      // Send light mode to light sensor
      payload[0] = SENSOR_LIGHT;
      break;
    case MUSIC:
      // Send music mode to sound sensor
      payload[0] = SENSOR_SOUND;
      break;
    case CONST:
      // Send const mode to clock face
      payload[0] = SET_TOGGLE;
      break;
  }
  
      can_bus.set_payload(payload, 1);
  can_bus.set_arbitration(SENSOR_ARBITRATION);
  can_bus.sendPacket();

  // If the mode is set to const, turn on the LED
  if (s->get_led_mode() == CONST) {
    s->set_led_on(true);
  }

  // Otherwise, this will be handled elsewhere
}

// Read sensor data
fix15 last_light;
fix15 last_sound;

#define DECIBEL_MAX int2fix15(100)
#define DECIBEL_MIN int2fix15(0)

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
      printf("Received sound data %f\n", fix2float15(last_sound));
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

      if (light_mode.enabled()) {
        core_state.set_led_mode(LIGHT);
      } else if (music_mode.enabled()) {
        core_state.set_led_mode(MUSIC);
      } else if (on_mode.enabled()) {
        core_state.set_led_mode(CONST);
      } else {
        printf("No modes selected.\n");
      }

      printf("Running in mode %d\n", core_state.get_led_mode());

      color_rgb c = color_in.sample();
      
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
          core_state.set_led_on(true);
          printf("Current volume %f\n", fix2float15(last_sound));

        
          // Set brightness based on volume
          c.red = (uint8_t) ((float) c.red * fix2float15(last_sound));
          c.green = (uint8_t) ((float) c.green * fix2float15(last_sound));
          c.blue = (uint8_t) ((float) c.blue * fix2float15(last_sound));

          break;
        case CONST:
          
          core_state.set_led_on(true);
          // Handle manual mode
          // Read red, green, and blue in values and set the color
          break;
      }
      
      core_state.set_led_color(Color(c));
    } else {
      // If the system is not enabled, turn off the LEDs
      core_state.set_led_on(false);
      printf("System disabled\n");
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

    printf("TX handler set up\n");
    
    pt_add_thread( protothread_core );

    // Start the threader
    pt_schedule_start ;
}

// Main for core 0
int main() {
  // Overclock to 160MHz (divides evenly to 1 megabaud) (160/5/32=1)
  set_sys_clock_khz(OVERCLOCK_RATE, true) ;

  // Initialize stdio
  stdio_init_all();
  sleep_ms(5000);

  printf("Launching...\n");
  
  // int rc = pico_led_init();
  // hard_assert(rc == PICO_OK);

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

  printf("RX handler set up\n");
  
  // pico_set_led(true);

  pt_schedule_start ;
}
