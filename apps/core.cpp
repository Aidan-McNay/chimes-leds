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
#include "interface/adc.h"
#include "interface/led.h"
#include "interface/pwm.h"
#include "can/can.h"

State core_state();

CAN can_bus( CORE_ARBITRATION, NETWORK_BROADCAST, 16, 22);

Switch system_enable(4); // whether to be on
Switch wifi_enable(5);   // whether to use WiFi configs
Switch light_mode(7);
Switch music_mode(8);
Switch on_mode(9);

ADC red_in(28);
ADC green_in(27);
ADC blue_in(26);

LED is_on(10);
PWM red_out(11);
PWM green_out(12);
PWM blue_out(14);
LED wifi_on(15);

void on_color_update(State *s) {
    color_rgb c = s->get_led_color().get_rgb();

    red_out.set_duty(c.red * 5000 / 255);
    green_out.set_duty(c.green * 5000 / 255);
    blue_out.set_duty(c.blue * 5000 / 255);

    unsigned short payload[4];

    payload[0] = COLOR;
    payload[1] = c.red;
    payload[2] = c.green;
    payload[3] = c.blue;
    can_bus.set_payload(payload, 4);

    for (short clock_face : CLOCK_FACE_ARBITRATION) {
        can_bus.set_arbitration(clock_face);
        can_bus.sendPacket();
    }
}


/*

**System Configuration**:
 - **4**: System enable (whether to be on)
 - **5**: WiFi enable (whether to use WiFi configs)

**User Mode**:
 - **7**: "Light" mode (based on ambient light, set as pull-down)
 - **8**: "Music" mode (pulse with music, set as pull-down)
 - **9**: "On" mode (always on, set as pull-down)

**Color Potentiometer Inputs**
 - **28**: Red Potentiometer
 - **27**: Green Potentiometer
 - **26**: Blue Potentiometer

**Status LEDs**:
 - **10**: "On" LED (high when enabled)
 - **11**: Red LED (PWM for current color)
 - **12**: Green LED (PWM for current color)
 - **14**: Blue LED (PWM for current color)
 - **15**: WiFi LED (indicate where configs are from)

**Communication LEDs** *(whether we're communicating with a board)*:
 - **3**: Sensor Board
 - **21**: North Face
 - **20**: South Face
 - **19**: East Face
 - **18**: West Face

**CAN connections**:
 - **16**: CAN TX
 - **17**: CAN RX
 - **22**: CAN Enable (active low)

*/

// Sensors

void read_packet( const unsigned short* packet, const unsigned char len ) {
    
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

    // Set can bus callback
    can_bus.set_callback( read_packet );

    // Setup the CAN receiver on core 0
    can_bus.setupCANRX(rx_handler) ;

    pt_schedule_start ;
}
