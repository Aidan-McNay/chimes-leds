#include "interface/adc.h"
#include "sensors/microphone.h"
#include <cstdio>

// ----------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------

Microphone::Microphone( int gpio_pin )
    : adc( gpio_pin ) {}

// ----------------------------------------------------------------------
// Microphone::sample
// ----------------------------------------------------------------------

fix15 Microphone::sample() {
    // First find the value of the ADC
    fix15 adc_proportion = adc.sample() ;
    printf("Raw adc data: %f\n", fix2float15(adc_proportion));

    // Decibels are defined as 20*log10(V/Vref)
    // where V/Vref is adc_proportion
    return adc_proportion;

    // return multfix15( int2fix15(20), log10fix( adc_proportion ) );
}
