// =======================================================================
// imu.cpp
// =======================================================================
// Definitions of the IMU functions

#include "sensors/light.h"
#include "hardware/i2c.h"

// ----------------------------------------------------------------------
// I2C Helper functions
// ----------------------------------------------------------------------

inline void LightSensor::write( uint8_t addr, uint16_t data )
{
  uint8_t buf[] = { addr, data >> 8, data & 0xFF };
  i2c_write_blocking( I2C_CHAN, ADDRESS, buf, 3, false );
}

inline void LightSensor::read( uint8_t addr, uint16_t* dest )
{
  i2c_write_blocking( I2C_CHAN, ADDRESS, &addr, 1, true );
  static uint8_t buf[2];
  i2c_read_blocking( I2C_CHAN, ADDRESS, buf, 2, false );

  *dest = ( buf[0] << 8 ) | buf[1];
}

// ----------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------

LightSensor::LightSensor( int gpio_sda, int gpio_scl )
{
  // Initialize I2C Channel
  i2c_init( I2C_CHAN, I2C_BAUD_RATE );

  // Configure GPIOs for I2C
  gpio_set_function( gpio_sda, GPIO_FUNC_I2C );
  gpio_set_function( gpio_scl, GPIO_FUNC_I2C );

  configure( MS_100, TIMES_1 );
  set_power_mode( OFF );
}

void LightSensor::configure( integration_time it, gain g )
{
  // Bits 12:11 are reserved for the gain
  // Bits 9:6 are reserved for the integration time
  // All other bits can be 0
  uint16_t config = ( g << 11 ) | ( it << 6 );

  write( ALS_CONF, config );
}

void LightSensor::set_power_mode( power_save_mode mode )
{
  // Bits 2:0 are reserved for the power save mode
  // All other bits must be 0
  uint16_t config = mode;

  write( Power_saving, config );
}