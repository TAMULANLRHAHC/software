# SOFTWARE

The software consists of 2 components:
- The arduino hand controller code
- The operator console project

## Hand Controller
The hand controller consists of an arduino microcontroller that runs a modbus server.

The hand controller is responsible for the following: 
- Relay commanding (1-8 relays) with state feedback
- 16bit ADC (8 channels)
  - Last 4 dont work properly for some reason
- Arduino ADC (3 channels)
  - AIN0-2
 
## Operator Console

A DARTWIC project that connects to the hand controller via the modbus tcp client package. 

The operator console is respobsilbe for the following
- Showing system status
- Ingesting telemetry from the hand controller
- Sending commands to the hand controller
