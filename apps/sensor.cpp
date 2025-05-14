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

// Sensors
#include "sensors/light.h"
#include "sensors/microphone.h"

// Microphone voltage
#define MIC_VOLTAGE float2fix15(3.3)

CAN can_bus( SENSOR_ARBITRATION, NETWORK_BROADCAST, 16, 22 ) ;
Microphone mic( 26 ) ;
LightSensor light_sensor( 20, 21 ) ;

// Send a packet with a fix15 data (sound or light value)
void send_packet (can_msg_t typ, fix15 value) {
  // Create a packet to send sound data
  unsigned short packet[3];

  packet[0] = typ; // Indicate sound data

  // Read the sound sensor
  packet[1] = value >> 16; // High short
  packet[2] = value & 0xFFFF; // Low short

  can_bus.set_payload(packet, 3) ;
  can_bus.set_arbitration(CORE_ARBITRATION) ;

  // Send the packet
  can_bus.sendPacket() ;
}

typedef enum {
  LIGHT,
  SOUND,
  DISABLED
} system_mode_t;

system_mode_t system_mode = SOUND;


void read_packet( const unsigned short* packet, const unsigned char len ) {
  // The length should be 1 here - the only commands we should recieve
  // are to request sensor data of certain types

  if ( len == 1 ) {
    switch ( packet[0] ) {
      case SENSOR_LIGHT:
        // Set the mode to light
        system_mode = LIGHT;
        break;
      case SENSOR_SOUND:
        // Set the mode to sound
        system_mode = SOUND;
        break;
      case SET_TOGGLE:
        system_mode = DISABLED;
        break;
      default:
        printf("Invalid packet type\n") ;
        system_mode = DISABLED;
        break;
    }
  }
  else {
    printf("Invalid packet length %d\n", len) ;
  }
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

#define PARAM_SAMPLE_RATE 10000
#define POLL_RATE  100
#define BUFFER_SIZE 1000

fix15 buffer[BUFFER_SIZE];

static PT_THREAD( protothread_poll( struct pt *pt) ) 
{
  PT_BEGIN( pt );

  static int begin_time, elapsed_time;
  static int poll_i = 0;

  while (1) {
    // Measure time at start of thread
    begin_time = time_us_32();

    fix15 value = mic.sample() ;
    
    buffer[poll_i] = value;
    poll_i = (poll_i + 1) % BUFFER_SIZE;

    elapsed_time = time_us_32() - begin_time;

    // yield for necessary amount of time
    PT_YIELD_usec( POLL_RATE - elapsed_time );
  }

  PT_END( pt );
}

static PT_THREAD( protothread_core( struct pt *pt ) )
{
  PT_BEGIN( pt );
  static int begin_time, elapsed_time;

  while (1) {
    // Measure time at start of thread
    begin_time = time_us_32();

    fix15 low;
    fix15 value = 0;

    printf("Running in mode %d\n", system_mode);
    
    switch (system_mode) {
      case LIGHT:
        // Send the light sensor
        value = light_sensor.sample() ;
        printf("Read light lux: %f\n", fix2float15(value));
        send_packet(SENSOR_LIGHT, value) ;
        break;
      case SOUND:
        // Send the sound sensor
        low = int2fix15(1);
        for (int i = 0; i < BUFFER_SIZE; ++i) {
          if (buffer[i] < low) {
            low = buffer[i];
          }
          if (buffer[i] > value) {
            value = buffer[i];
          }
        }

        value -= low;

        printf("Read mic amp: %f\n", fix2float15(value));
        send_packet(SENSOR_SOUND, value) ;
        break;
      default:
        printf("Sensor disabled\n");
        break;
    }
  

    elapsed_time = time_us_32() - begin_time;

    // yield for necessary amount of time
    PT_YIELD_usec( PARAM_SAMPLE_RATE - elapsed_time );
  }
  PT_END( pt );
}


//                             MAIN FOR CORES 0 AND 1
//
// Main for core 1
void core1_main() {
  // CAN transmitter will run on core 1
  can_bus.setupCANTX(tx_handler) ;

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

  light_sensor.init();

  printf("Launching...\n");
  
  // start core 1 threads
  multicore_reset_core1();
  multicore_launch_core1(&core1_main);

  // Set can bus callback
  can_bus.set_callback( read_packet );

  // Setup the CAN receiver on core 0
  can_bus.setupCANRX(rx_handler) ;
  
  pt_add_thread( protothread_poll );

  pt_schedule_start ;
}


