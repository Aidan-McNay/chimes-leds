# face

The PCB files for the control board at the playing stand level

## GPIO Mapping

**LED PWM GPIOs (high for on)**:
 - **28**: Red LED
 - **26**: Green LED
 - **20**: Blue LED
 - **18**: White LED

**Status LEDs**:
 - **21**: "On" LED (high when enabled)
 - **19**: Communication LED (On when connected to control panel)

**CAN connections**:
 - **16**: CAN TX
 - **17**: CAN RX
 - **22**: CAN Enable (active low)