Conclusions
==========================================================================

This project built a LED controller system for Cornell’s clocktower using 
Raspberry Pi Pico W microcontrollers. Our system successfully implemented 
a modular control architecture based on three types of boards (controller, 
sensor and face boards) and a custom CAN bus communciation, providing a 
user-friendly control panel for Chimesmasters to set and adjust lighting 
behavior of the colcktower.

Our fianl system achieved all of the core goals we set in the proposal:

* The CAN bus supported data sending and receiving among boards based on Pico 
  W’s PIO and DMA
* On the core board, one switch determined LEDs to be turned on or off, one 
  rotary switch determined the LED modes, and three potentiometer sliders 
  controlled the brightness of LED's R/G/B channels using PWM
* The face boards responded accurately to R/G/B color updates when sliding 
  the potentiometers on the core board
* The sensor board monitored and returned sound data to the core board in 
  the ``SOUND`` mode, and then achieving the control of lighting brightness 
  by the external sound level

While the system performed well during demonstration, there are two aspects 
for improvement the next time. For example, one of the face boards suffered 
from an internal short, which did not impact overall demonstration but it was 
necessary to be fixed for full deployment. Additionally, the start up of the 
CAN bus needs a manual short between the differential lines after powered on, 
which might due to the initial deadlock in CAN bus' PIO implementation, and 
would be addressed in a future revision. 

Looking forward, our system provides a foundation for future expansion like 
WiFi/Bluetooth enables remoting control interfaces, and also the distributed 
hardware design allows more clock faces or connecting other buildings or devices.

In the end, there were no external standards governing our project. However, 
we reused Prof. Adams’ `CAN bus implementation <https://vanhunteradams.com/Pico/CAN/CAN.html>`_,
and our team's PWM implementation used in lab3.
