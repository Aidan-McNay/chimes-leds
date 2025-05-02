/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "interface/pico_led.h"

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

int main()
{
  int rc = pico_led_init();
  hard_assert( rc == PICO_OK );
  while ( true ) {
    pico_set_led( true );
    sleep_ms( LED_DELAY_MS );
    pico_set_led( false );
    sleep_ms( LED_DELAY_MS );
  }
}
