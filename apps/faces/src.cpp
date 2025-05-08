#ifndef CLOCK_FACE_INDEX
#error "Cannot compile watch face without index"
#else

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
#include "interface/color_led.h"
#include "utils/color.h"
#include "interface/heartbeat.h"
#include "interface/pico_led.h"

#define RED_PIN 28
#define GREEN_PIN 26
#define BLUE_PIN 20
#define WHITE_PIN 18

#define ARBITRATION CLOCK_FACE_ARBITRATION[CLOCK_FACE_INDEX]

CAN can_bus(ARBITRATION, NETWORK_BROADCAST, 16, 22);

// us per sample (currently 100/sec)
#define HEARTBEAT_RATE 10000
Heartbeat heartbeat(19, 100);

ColorLED led(false, RED_PIN, GREEN_PIN, BLUE_PIN, WHITE_PIN);
LED status(21);

void read_packet( const unsigned short* packet, const unsigned char len ) {
    // The length should be 1 here - the only commands we should recieve
    // are to request sensor data of certain types

    color_rgb c;
    bool on;
    can_msg_t typ = (can_msg_t) packet[0];

    switch ( typ ) {
        case SET_COLOR:
            printf("Set color to %d %d %d\n", packet[1], packet[2], packet[3]);
            c.red = packet[1];
            c.green = packet[2];
            c.blue = packet[3];
            led.set_color(c);
            break;
        case SET_TOGGLE:
            // Set the LED off
            on = packet[1];
            led.toggle(on);
            status.toggle(on);

            break;
        default:
            printf("Invalid packet type\n") ;
            break;
    }
}

// us per sample
#define PARAM_SAMPLE_RATE 1000000


static PT_THREAD( protothread_core( struct pt *pt ) )
{
  PT_BEGIN( pt );
  static int begin_time, elapsed_time;

  while (1) {
    // Measure time at start of thread
    begin_time = time_us_32();
    
    
    unsigned short payload[1];

    payload[0] = 3;

    can_bus.set_payload(payload, 1);

    can_bus.set_arbitration(DUMMY_ARBITRATION);
    can_bus.sendPacket();
  

    elapsed_time = time_us_32() - begin_time;

    // yield for necessary amount of time
    PT_YIELD_usec( PARAM_SAMPLE_RATE - elapsed_time );
  }
  PT_END( pt );
}


static PT_THREAD( protothread_heartbeat( struct pt *pt ) )
{
  PT_BEGIN( pt );
  static int begin_time;

  while (1) {
    // Measure time at start of thread
    begin_time = time_us_32();

    // Step the heartbeat
    heartbeat.step();

    int elapsed_time = ( time_us_32() - begin_time );

    // yield for necessary amount of time
    PT_YIELD_usec( HEARTBEAT_RATE - elapsed_time );
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
  // Ping the heartbeat
  heartbeat.ping();
  can_bus.handle_rx() ;
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
    
  // int rc = pico_led_init();
  // hard_assert(rc == PICO_OK);
    
  // start core 1 threads
  multicore_reset_core1();
  multicore_launch_core1(&core1_main);
  
  // Set can bus callback
  can_bus.set_callback( read_packet );
  
  // Setup the CAN receiver on core 0
  can_bus.setupCANRX(rx_handler) ;
  
  // Initialize the heartbeat
  pt_add_thread( protothread_heartbeat );
  
  // pico_set_led(true);
  
  pt_schedule_start ;
}

#endif
