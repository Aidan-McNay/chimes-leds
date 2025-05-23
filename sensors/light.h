// =======================================================================
// imu.h
// =======================================================================
// Declarations of the IMU functions

#ifndef LIGHT_H
#define LIGHT_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "utils/fix15.h"

// ----------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------

class LightSensor {
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Constant defines
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  const static uint8_t         ADDRESS       = 0x48;
  constexpr static i2c_inst_t* I2C_CHAN      = i2c0;
  const static uint            I2C_BAUD_RATE = 400000;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Registers
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // See register map:
  // https://cdn.sparkfun.com/assets/d/7/4/2/9/veml6030_datasheet.pdf

  const static uint8_t ALS_CONF        = 0x6B;
  const static uint8_t ALS_WH  = 0x19;
  const static uint8_t ALS_WL  = 0x1B;

  const static uint8_t Power_saving = 0x1C;

  const static uint8_t ALS   = 0x37;
  const static uint8_t WHITE     = 0x38;
  const static uint8_t ALS_INT = 0x3B;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Public accessor functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 public:
  // Configuration types

  // Gain
  typedef enum {
    TIMES_2 = 0b01,
    TIMES_1 = 0b00,
    DIV_4 = 0b11,
    DIV_8 = 0b10,
  } gain_t;

  // Integration time (MS)
  typedef enum {
    MS_800 = 0b0011,
    MS_400 = 0b0010,
    MS_200 = 0b0001,
    MS_100 = 0b0000,
    MS_50  = 0b1000,
    MS_25  = 0b1100,
  } integration_time_t;

  // Power saving mode
  typedef enum  {
    LEAST = 0b001,
    LESS = 0b011,
    MORE = 0b101,
    MOST = 0b111,
    OFF = 0b000
  } power_save_mode_t;


  LightSensor( int gpio_sda, int gpio_scl );
  void fetch();
  void init();

  void configure(integration_time_t it, gain_t g);
  void set_power_mode(power_save_mode_t mode);

  // Get the light sensed in lux
  fix15 sample();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Protected helper functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 protected:
  int gpio_sda, gpio_scl;
  static void write( uint8_t addr, uint16_t data );
  static uint16_t read( uint8_t addr );

  // Gets the resolution of the sensor
  fix15 resolution();

  gain_t gain;
  integration_time_t integration_time;
};

#endif  // LIGHT_H
