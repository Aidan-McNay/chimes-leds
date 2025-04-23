#include "interface/color_led.h"
#include "utils/color_correction.h"
#include <algorithm>

// --------------------------------------------------------
// Constructor
// --------------------------------------------------------

ColorLED::ColorLED(
    int red_pin, int green_pin, int blue_pin, int white_pin)
    :   red(red_pin, 125.0f, 255, 0),
        green(green_pin, 125.0f, 255, 0),
        blue(blue_pin, 125.0f, 255, 0),
        white(white_pin, 125.0f, 255, 0) {}

// --------------------------------------------------------
// Set color
// --------------------------------------------------------

void ColorLED::set_color(const color_rgb c) {
    // Gamma correction
    int r = gamma_lut[c.red];
    int g = gamma_lut[c.green];
    int b = gamma_lut[c.blue];

    // Convert to RGBW
    int w = std::min(r, std::min(g, b));

    r -= w;
    g -= w;
    b -= w;

    // White balance
    r = r * white_balance.red / 128;
    g = g * white_balance.green / 128;
    b = b * white_balance.blue / 128;
    w = w * white_correction / 128;

    set_rgbw(r, g, b, w);
}

void ColorLED::set_rgbw(int r, int g, int b, int w) {
    // Set the duty cycles
    red.set_duty(r);
    green.set_duty(g);
    blue.set_duty(b);
    white.set_duty(w);
}

// --------------------------------------------------------
// Toggle the LED
// --------------------------------------------------------
void ColorLED::toggle(bool on) {
    red.toggle(on);
    green.toggle(on);
    blue.toggle(on);
    white.toggle(on);
}