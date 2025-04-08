// =======================================================================
// state.h
// =======================================================================
// A representation of the current state of the LED system

#ifndef UTILS_STATE_H
#define UTILS_STATE_H

#include "utils/color.h"
#include <vector>

// -----------------------------------------------------------------------
// LED Modes
// -----------------------------------------------------------------------

enum LedMode {
  CONST,  // Always on
  LIGHT,  // Depend on ambient light
  MUSIC   // Pulse with the music
};

// -----------------------------------------------------------------------
// State
// -----------------------------------------------------------------------

class State {
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Public accessor functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 public:
  State()
      : led_on( false ),
        led_mode( CONST ),
        led_color(),
        led_on_callbacks(),
        led_mode_callbacks(),
        led_color_callbacks() {};

  // Getters
  bool get_led_on()
  {
    return led_on;
  }
  LedMode get_led_mode()
  {
    return led_mode;
  }
  Color get_led_color()
  {
    return led_color;
  }

  // Setters
  void set_led_on( bool new_led_on );
  void set_led_mode( LedMode new_led_mode );
  void set_led_color( Color new_led_color );

  // Callbacks
  //  - Called when one of the given attributes is changed
  typedef void ( *callback )( State* state );

  void add_led_on_callback( callback new_callback );
  void add_led_mode_callback( callback new_callback );
  void add_led_color_callback( callback new_callback );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Protected attributes
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 protected:
  bool    led_on;
  LedMode led_mode;
  Color   led_color;

  std::vector<callback> led_on_callbacks;
  std::vector<callback> led_mode_callbacks;
  std::vector<callback> led_color_callbacks;
};

#endif  // UTILS_STATE_H