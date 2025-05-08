Hardware Design
==========================================================================

Our system was composed of three types of board:

* A controller board (located at the main playing level, acting as the
  primary user interface)
* A sensor board (located in the belfry for collecting sensor measurements)
* Four face boards (located in each clock face for driving the LEDs)

All of these boards communicate over a shared CAN bus for reliable
communication of information

LED Setup
--------------------------------------------------------------------------

The clocktower currently uses strips of `RGBW LEDs <https://www.superbrightleds.com/led-strips-and-bars/led-strip-lights-by-voltage/12-volt-led-strip-lights/5m-rgbw-led-strip-light-4-in-1-chip-5050-color-changing-led-tape-light-12v-24v-ip20>`_
to light the clock faces. These LEDs have 5 pins: ``+12V``, ``R``, ``G``,
``B``, and ``W``. We suspected this meant that the non-power lines should
PWM'd to ground to adjust the amount of that color; after probing these
lines, we confirmed that this was the case, with the model below showing
the strips in beige (anticipating connecting them to some form of switch)

.. image:: img/hw_des/pins.png
   :align: center
   :width: 90%
   :class: bottompadding image-corners image-border

.. image:: img/hw_des/leds.png
   :align: center
   :width: 50%

Controller Board
--------------------------------------------------------------------------

Sensor Board
--------------------------------------------------------------------------

Face Board
--------------------------------------------------------------------------