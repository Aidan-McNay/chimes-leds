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
#include "utils/can_interface.h"

// Sensors
#include "sensors/light.h"
#include "sensors/microphone.h"

#define GPIO_MIC 26
#define LIGHT_SDA 27
#define LIGHT_SCL 28
#define CAN_TX 5
#define CAN_TRANSCEIVER 22
// Microphone voltage
#define MIC_VOLTAGE float2fix15(3.3)


CAN can_bus( SENSOR_ARBITRATION, NETWORK_BROADCAST, CAN_TX, CAN_TRANSCEIVER ) ;
Microphone mic( GPIO_MIC ) ;
LightSensor light_sensor( LIGHT_SDA, LIGHT_SCL ) ;

// Send a packet with a fix15 data (sound or light value)
void send_packet (sensor_msg_t typ, fix15 value) {
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
        sensor_msg_t typ = (sensor_msg_t) packet[0];
        switch ( typ ) {
            case LIGHT:
                // Send the light sensor
                value = light_sensor.sample() ;
                send_packet(typ, value) ;
                break;
            case SOUND:
                // Send the sound sensor
                value = mic.sample() ;
                send_packet(typ, value) ;
                break;
            default:
                printf("Invalid packet type\n") ;
                break;
        }
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

    pt_schedule_start ;
}
