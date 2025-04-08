// =======================================================================
// color.h
// =======================================================================
// Declarations for the color utilities

#ifndef UTILS_COLOR_H
#define UTILS_COLOR_H

#include <cstdint>
#include <string>

// -----------------------------------------------------------------------
// Define common color structures
// -----------------------------------------------------------------------

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} color_rgb;

class Color {
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Public accessor functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 public:
  Color();  // Will default to white
  Color( color_rgb color );
  Color( std::string hex );

  // Get the stored color
  color_rgb   get_rgb();
  std::string get_hex();

  // Set the stored color
  void set_color( color_rgb new_color );
  void set_color( std::string new_color );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Protected attributes
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 protected:
  color_rgb color;
};

#endif  // UTILS_COLOR_H