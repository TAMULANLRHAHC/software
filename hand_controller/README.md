# hand_controller

Simple Arduino Modbus TCP hand controller for the Mega + W5500 Ethernet setup.

This version removes the old JSON socket protocol and uses the Arduino `ArduinoModbus` library in the same style as `C:\Users\saint\Documents\Code\Github\GSE-Miscellaneous\arduinomodbus.ino`.

## What it does

- Hosts a Modbus TCP server on port `502`
- Writes ADS1115 voltages into input registers
- Writes actual relay states into input registers
- Reads relay command coils and drives the relay outputs
- Reads watchdog fallback coils and uses them if Modbus traffic stops for 10 seconds

## Modbus map

### Coils

- `0-7`: live relay commands for relays `1-8`
- `8-15`: watchdog fallback states for relays `1-8`

### Input registers

- `0-7`: ADS1115 analog voltages for channels `AIN_0` through `AIN_7`
- `8-10`: Arduino internal ADC voltages for channels `AIN_0` through `AIN_2`
- `11-18`: actual relay states for relays `1-8`

## Input register units

- Voltage registers are stored in millivolts
- Example: register value `3300` means `3.300 V`
- Relay state registers are `0` for off and `1` for on
- Internal ADC voltages are calculated against a `5.0 V` reference

## Watchdog behavior

- Any successful Modbus request refreshes the watchdog timer
- If no Modbus traffic is seen for `10000 ms`, the controller applies the watchdog coil states
- When Modbus traffic resumes, live relay command coils take control again

## Hardware assumptions

- Board: Arduino Mega 2560
- Ethernet: W5500 on SPI CS pin `10`
- ADS1115 devices:
- `0x49`
- `0x48`
- Arduino onboard ADC pins:
- `AIN_8`: `A0`
- `AIN_9`: `A1`
- `AIN_10`: `A2`
- Relay pins:
- Relay 1: pin `49`
- Relay 2: pin `47`
- Relay 3: pin `45`
- Relay 4: pin `43`
- Relay 5: pin `41`
- Relay 6: pin `39`
- Relay 7: pin `37`
- Relay 8: pin `35`

## Network settings

- Static IP: `192.168.1.116`
- Modbus TCP port: `502`
- MAC: `DE:AD:BE:EF:FE:EE`

## Build

This project uses PlatformIO.

```ini
pio run
pio run -t upload
pio device monitor
```
