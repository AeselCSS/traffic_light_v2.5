# Traffic Light Control System for Arduino

An AVR-based traffic light controller system for a single intersection with pedestrian crossings. 
The system manages both vehicle traffic lights and pedestrian signals, including button-activated pedestrian crossing requests.

## Features

- Full traffic light cycle for North-South and East-West directions
- Pedestrian lights with button activation
- Visual feedback for pedestrian button press (white LED)
- Safety features including all-red phases
- Flashing white LED during pedestrian green phase

## Hardware Requirements

- ATmega328P microcontroller
- LEDs:
  - 6x Traffic lights (2x Red, Yellow, Green)
  - 6x Pedestrian lights (2x Red, Green, White)
- Resistors:
  - 12x 220Ω resistors (for LEDs)
  - 2x 10kΩ resistors (for button pull-down)
- 2x Push buttons for pedestrian crossing requests

## Pin Configuration

### Traffic Lights (PORTB)

- `PB5`: North-South Red
- `PB4`: North-South Yellow
- `PB3`: North-South Green
- `PB2`: East-West Red
- `PB1`: East-West Yellow
- `PB0`: East-West Green

### Pedestrian Lights (PORTD)

- `PD7`: North-South Pedestrian Red
- `PD6`: North-South Pedestrian Green
- `PD5`: North-South Pedestrian White
- `PD4`: East-West Pedestrian Red
- `PD3`: East-West Pedestrian Green
- `PD2`: East-West Pedestrian White

### Pedestrian Buttons (PORTC)

- `PC0`: North-South Button
- `PC1`: East-West Button

## Timing Configuration

- Green Light: 15 seconds
- Shortened Green Light: 8 seconds
- Yellow Light: 2 seconds
- All Red Safety Phase: 3 seconds
- Red+Yellow Phase: 2 seconds

## State Machine

The traffic light system cycles through the following states:

1. NS: Red, EW: Green
2. NS: Red, EW: Yellow
3. Both: Red (safety phase)
4. NS: Red+Yellow, EW: Red
5. NS: Green, EW: Red
6. NS: Yellow, EW: Red
7. Both: Red (safety phase)
8. NS: Red, EW: Red+Yellow

## Building and Uploading

The project uses a Makefile for compilation. To build and upload:

```sh
make compile  # Compile the project
make upload   # Upload to Arduino
make clean    # Clean build files
```
or simply use:
```sh
make all # Compiles the project, Uploads to Arduino and Cleans build files
```

## Dependencies

- `avr-gcc`
- `avrdude`
- `make`