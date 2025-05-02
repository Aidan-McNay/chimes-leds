#include "pico/stdlib.h"
#include "interface/color_adc.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

// Switch calibrations
static const int color_min = 16;
static const int color_max = 3530;

// ----------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------

ColorInput::ColorInput()
{
  //
  // Setup Analog pin/ADC
  //
  adc_gpio_init( red_chan + 26 );
  adc_gpio_init( green_chan + 26 );
  adc_gpio_init( blue_chan + 26 );


  adc_init();

  int round_robin_mask = ( 1 << red_chan ) | ( 1 << green_chan ) | ( 1 << blue_chan );
  adc_set_round_robin( round_robin_mask );

  adc_fifo_setup(
      true,    // Write each completed conversion to the sample FIFO
      true,    // Enable DMA data request (DREQ)
      1,       // DREQ (and IRQ) asserted when at least 1 sample present
      false,   // We won't see the ERR bit because of 8 bit reads; disable.
      false     // Keep samples at 12 bits
  );

  adc_set_clkdiv(0);

  dma_chan = dma_claim_unused_channel(true);
  cfg = dma_channel_get_default_config(dma_chan);

  // Reading from constant address, writing to incrementing 2 byte addresses
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, true);

  // Pace transfers based on availability of ADC samples
  channel_config_set_dreq(&cfg, DREQ_ADC);
}

// ----------------------------------------------------------------------
// ColorInput::sample
// ----------------------------------------------------------------------

color_rgb ColorInput::sample() {
  // Set starting ADC channel 
  adc_select_input(blue_chan);

  // Set/Reset DMA transfer
  dma_channel_configure(dma_chan, &cfg,
      capture_buf,    // dst
      &adc_hw->fifo,  // src
      3,  // transfer count
      true            // start immediately
  );

  // printf("Starting capture\n");
  adc_run(true);

  // Once DMA finishes, stop any new conversions from starting, and clean up
  // the FIFO in case the ADC was still mid-conversion.
  dma_channel_wait_for_finish_blocking(dma_chan);
  adc_run(false);
  adc_fifo_drain();

  // printf("Captured %u %u %u\n", capture_buf[red_chan], capture_buf[green_chan], capture_buf[blue_chan]);

  // Use the calibration values to convert the ADC values to RGB values
  int red = capture_buf[red_chan] - color_min;
  int green = capture_buf[green_chan] - color_min;
  int blue = capture_buf[blue_chan] - color_min;
  red = (red * 255) / (color_max - color_min);
  green = (green * 255) / (color_max - color_min);
  blue = (blue * 255) / (color_max - color_min);

  // Clamp the values for safety
  if (red < 0) red = 0;
  if (green < 0) green = 0;
  if (blue < 0) blue = 0;

  if (red > 255) red = 255;
  if (green > 255) green = 255;
  if (blue > 255) blue = 255;

  color_rgb c;
  c.red = red;
  c.green = green;
  c.blue = blue;

  return c;
}