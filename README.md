# DashboardDisplay
Atmel SAMV71 display driver and controller. The display is mounted in the steering wheel in the Formula Student car.

## Interfacing
Current software version is configured to support 1Mbit/s CAN 2.0 8 byte frames.
Can address: 1000
Frame content:
  - byte 0: throttle (downscaled by 50)
  - byte 1: ECT (engine coolant temperature)
  - byte 2: battery voltage (scaled up by 10)
  - byte 3: TPS (throttle position sensor) in %
  - byte 4: 1 - neutral, 0 - gear
  
## Design expandability
The PCB has footprints for adding extra headers to support I2C / SPI / UART / GPIO and analog/interrupt input. Please refer to the PCB pdf for the pin mapping.
