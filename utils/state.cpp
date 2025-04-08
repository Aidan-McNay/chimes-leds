// =======================================================================
// state.cpp
// =======================================================================
// Definitions for our system state

#include "utils/state.h"

// -----------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------

void State::set_led_on( bool new_led_on )
{
  led_on = new_led_on;
  for ( State::callback callback : led_on_callbacks ) {
    callback( this );
  }
}

void State::set_led_mode( LedMode new_led_mode )
{
  led_mode = new_led_mode;
  for ( State::callback callback : led_mode_callbacks ) {
    callback( this );
  }
}

void State::set_led_color( Color new_led_color )
{
  led_color = new_led_color;
  for ( State::callback callback : led_color_callbacks ) {
    callback( this );
  }
}

// -----------------------------------------------------------------------
// Callbacks
// -----------------------------------------------------------------------

void State::add_led_on_callback( State::callback new_callback )
{
  led_on_callbacks.push_back( new_callback );
}

void State::add_led_mode_callback( State::callback new_callback )
{
  led_mode_callbacks.push_back( new_callback );
}

void State::add_led_color_callback( State::callback new_callback )
{
  led_color_callbacks.push_back( new_callback );
}