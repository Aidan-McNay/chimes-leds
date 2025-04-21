#include "interface/heartbeat.h"

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------
Heartbeat::Heartbeat( int gpio_pin, int duration )
    : status( gpio_pin ),
      duration( duration ),
      counter( 0 ) {}

// ----------------------------------------------------------------------
// Heartbeat::step
// ----------------------------------------------------------------------
// Represents one tick of time for the heartbeat without a ping

void Heartbeat::step() {
  if (counter == 0) {
    // Hasn't been pinged in too long
    status.toggle(0);
    
    // Turn off permanently
    counter = -1;
  } else if (counter > 0) {
    counter -= 1;
  }
}

// ----------------------------------------------------------------------
// Heartbeat::ping
// ----------------------------------------------------------------------
// Turn on the heartbeat and reset the counter

void Heartbeat::ping() {
  // Turn on the LED
  status.toggle(1);
  counter = duration;
}




