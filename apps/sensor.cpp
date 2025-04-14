// Standard C libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// High-level pico libraries
#include "pico/stdlib.h"
#include "pico/multicore.h"
// Interface library to sys_clock (SDK 2.0.0 )
#include "hardware/clocks.h"
// Interface library to watchdog
#include "hardware/watchdog.h"
// CAN driver
#include "can/can.h"
// Protothreads
#include "utils/pt_cornell_rp2040_v1.h"
#include "identities.h"

// Sensors
#include "sensors/light.h"
#include "sensors/microphone.h"

#define GPIO_MIC 26
#define LIGHT_SDA 27
#define LIGHT_SCL 28
// Microphone voltage
#define MIC_VOLTAGE float2fix15(3.3)

typedef enum {
    LIGHT = 0x01, // Request light sensor data
    SOUND = 0x02, // Request sound sensor data
} sensor_read_type_t;


CAN can_bus( SENSOR_ARBITRATION, CORE_ARBITRATION, NETWORK_BROADCAST ) ;
Microphone mic( GPIO_MIC ) ;
LightSensor light_sensor( LIGHT_SDA, LIGHT_SCL ) ;

// Send a packet with a fix15 data (sound or light value)
void send_packet (sensor_read_type_t typ, fix15 value) {
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


void read_packet( const unsigned short* packet, const unsigned char len ) {
    // The length should be 1 here - the only commands we should recieve
    // are to request sensor data of certain types

    if ( len == 1 ) {
        fix15 value;
        sensor_read_type_t typ = (sensor_read_type_t) packet[0];
        switch ( typ ) {
            case LIGHT:
                // Send the light sensor
                value = light_sensor.sample() ;
                break;
            case SOUND:
                // Send the sound sensor
                value = mic.sample() ;
                break;
            default:
                printf("Invalid packet type\n") ;
                break;
        }

        send_packet(typ, value) ;
    }
    else {
        printf("Invalid packet length\n") ;
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

// Thread runs on core 0
static PT_THREAD (protothread_watchdog(struct pt *pt))
{
    PT_BEGIN(pt);

    // Enable the watchdog, requiring the watchdog to be updated every 1sec or the chip will reboot
    // second arg is pause on debug which means the watchdog will pause when stepping through code
    watchdog_enable(1000, 1);

      while(1) {
        sleep_ms(100) ;
        watchdog_update();
      } 
  PT_END(pt);
}

// Main for core 0
int main() {
    // Initialize stdio
    stdio_init_all();

    if (watchdog_caused_reboot()) {
        printf("Rebooted by Watchdog!\n");
    } else {
        printf("Clean boot\n");
    }

    // Set can bus callback
    can_bus.set_callback( read_packet );

    // Setup the CAN receiver on core 0
    can_bus.setupCANRX(rx_handler) ;

    // Add thread to scheduler, and start it
    pt_add_thread(protothread_watchdog) ;
    pt_schedule_start ;
}
