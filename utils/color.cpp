// =======================================================================
// color.cpp
// =======================================================================
// Definitions for the color utilities

#include "utils/color.h"

// -----------------------------------------------------------------------
// Helper functions for color conversion
// -----------------------------------------------------------------------

inline color_rgb hex_to_rgb( std::string hex )
{
  if ( hex[0] == '#' ) {
    hex = hex.substr( 1 );
  }
  color_rgb color;
  sscanf( hex.c_str(), "%2hhx%2hhx%2hhx", &color.red, &color.green,
          &color.blue );
  return color;
};

inline std::string rgb_to_hex( color_rgb color )
{
  char buf[8];
  sprintf( buf, "#%02x%02x%02x", color.red, color.green, color.blue );
  return buf;
}

// -----------------------------------------------------------------------
// Constructors
// -----------------------------------------------------------------------

Color::Color() : color( { 255, 255, 255 } ) {};

Color::Color( color_rgb color ) : color( color ) {};

Color::Color( std::string hex ) : color( hex_to_rgb( hex ) ) {};

// -----------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------

color_rgb Color::get_rgb()
{
  return color;
}

std::string Color::get_hex()
{
  return rgb_to_hex( color );
}

// -----------------------------------------------------------------------
// Setters
// -----------------------------------------------------------------------

void Color::set_color( color_rgb new_color )
{
  color = new_color;
}

void Color::set_color( std::string new_color )
{
  color = hex_to_rgb( new_color );
}