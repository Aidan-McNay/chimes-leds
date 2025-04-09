# control

The PCB files for the control board at the playing stand level

## GPIO Mapping

**System Configuration**:
 - **4**: System enable (whether to be on)
 - **5**: WiFi enable (whether to use WiFi configs)

**User Mode**:
 - **7**: "Light" mode (based on ambient light, set as pull-down)
 - **8**: "Music" mode (pulse with music, set as pull-down)
 - **9**: "On" mode (always on, set as pull-down)

**Color Potentiometer Inputs**
 - **26**: Red Potentiometer
 - **27**: Green Potentiometer
 - **28**: Blue Potentiometer

**Status LEDs**:
 - **10**: "On" LED (high when enabled)
 - **11**: Red LED (PWM for current color)
 - **12**: Green LED (PWM for current color)
 - **14**: Blue LED (PWM for current color)
 - **15**: WiFi LED (indicate where configs are from)

**Communication LEDs** *(whether we're communicating with a board)*:
 - **3**: Sensor Board
 - **18**: North Face
 - **19**: East Face
 - **20**: South Face
 - **21**: West Face

**CAN connections**:
 - **16**: CAN TX
 - **17**: CAN RX
 - **22**: CAN Enable (active low)