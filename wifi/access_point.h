// =======================================================================
// access_point.h
// =======================================================================
// Declarations for our access point

#ifndef WIFI_ACCESS_POINT_H
#define WIFI_ACCESS_POINT_H

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "utils/state.h"

class AccessPoint {
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Public accessor functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 public:
  AccessPoint( State* state );

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Protected attributes
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 protected:
  State* state;
};

#endif  // WIFI_ACCESS_POINT_H