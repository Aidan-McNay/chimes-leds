Program Design
==========================================================================
We designed the system around the core control board, which performed the data processing and control of the face boards. The system is modular and supports any number of face boards, and all communication goes through the core board.

Communication System
--------------------------------------------------------------------------

Our boards communicate to one another using CAN bus messages. The sensor board does not communicate with the face boards, and both communicate through the core. The sensor board reports raw sensor data to the core, which then converts this to a light color to send to the face boards (dependent on the mode of the system).

Every CAN message can be represented as an array of shorts (16 bit values). The first 16 bits represents the arbitration ID, which indicates the board that the message is intended for. Next, the length of the message (in shorts) is encoded in a byte (along with one byte of padding).

Finally, this is followed by the data payload, and terminated with a CRC checksum. The payload starts with a single short representing the message type, then followed by raw data. There are $4$ different message types, which are:

1. ``SENSOR_LIGHT`` represents the transferring of a light sensor *request* to the sensor board. This toggles the sensor board into light sensor mode, which will cause it to repeatedly send light sensor data from the sensor board to the core. Data messages also use this same message type. The data payload is empty for the request, and contains a ``32-bit fix15`` value for the response (packed into two shorts).
2. ``SENSOR_SOUND`` does the same as above, but requesting sound data instead.
3. ``SET_COLOR`` is used to set the color of the face boards. The data payload contains 3 shorts, each containing a byte value from 0-255 representing the red, green, and blue values of the color.
4. ``SET_TOGGLE`` toggles the sensor and face boards on and off. For the face boards, this turns off the lights, and for the sensor board this disables the data messages.

Controller Board
--------------------------------------------------------------------------

The controller board is the core of the system, and controls the rest of the boards. It receives user input via the physical switches on the board, and uses these to control the rest of the system. 

The system has three main modes, selected by a rotary switch:

1. Light mode, where the system is turned on or off based on whether ambient light is detected below 1 lux.
2. Sound mode, where the intensity of the light pulsates with the level of sound detected by the microphone.
3. Manual mode, where the light is always on.

The color of the light is set using the three physical sliders, allowing the user to control the color of the light in RGB. This is the base light color shown in all 3 modes.

The controller board has a main thread managing the system running at a rate of 100Hz. This thread reads sensor data from the sensor board, and sends update messages to the face boards. It also reads values from input switches in order to control the system.


Sensor Board
--------------------------------------------------------------------------

Face Board
--------------------------------------------------------------------------