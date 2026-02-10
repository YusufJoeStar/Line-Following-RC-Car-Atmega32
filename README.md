# Line Following RC Car using ATmega32

This project is a line-following RC car built using an ATmega32 microcontroller,
three IR sensors, and an L298N H-Bridge motor driver.  
The car follows a black line on a white surface using simple digital logic and PWM-based motor control.

---

## Project Overview

The system reads three IR sensors (Left, Center, Right) mounted at the front of the car.
Based on which sensor detects the black line, the microcontroller adjusts motor speeds
to steer the car left, right, or forward.

The design prioritizes simplicity, reliability, and real-time response without using
PID control.

---

## Hardware Components

- ATmega32 microcontroller (16 MHz external crystal)
- L298N dual H-Bridge motor driver
- 3× IR reflective sensors (TCRT5000 modules)
- 4× TT DC motors (paired as left and right)
- 16 MHz crystal oscillator + 2× 22pF capacitors
- 5V voltage regulator (7805)
- Rechargeable battery pack
- Breadboard and jumper wires
- RC car chassis

Full list available in:  
`hardware/Components_List.md`

---

## Firmware

- Language: C (AVR-GCC)
- MCU Frequency: 16 MHz
- PWM: Timer1 (Fast PWM mode)
- Sensor Logic: Digital (active HIGH)

Main firmware file:  
`firmware/main.c`

---

## Control Logic

| Sensor State | Action |
|-------------|--------|
| Center only | Move forward |
| Left only   | Turn left |
| Right only  | Turn right |
| No sensors  | Stop |

---

## Proteus Simulation

The complete Proteus schematic and simulation files are included.

- Proteus project: `proteus/LineFollower.pdsprj`
- Schematic image: `docs/Proteus_Schematic.png`

---

## Photos

Real-life photos of the assembled car and wiring are available in:

`hardware/Car_Photos/`

---

## Tools & Tech Stack

- Microcontroller: ATmega32
- Programming: AVR-GCC
- Simulation: Proteus
- Programmer: USBasp
- IDE: Atmel Studio / VS Code
- Version Control: Git & GitHub

---

## Notes

- Sensors are calibrated using onboard potentiometers.
- Motors are powered separately from logic supply.
- Common ground is shared between all modules.

---

## License

This project is licensed under the MIT License.
