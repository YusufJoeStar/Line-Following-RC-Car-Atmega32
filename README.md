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

<img width="1151" height="652" alt="image" src="https://github.com/user-attachments/assets/926ea515-5b96-4a25-8167-320a47e14876" />


---

## Photos

<img width="859" height="610" alt="image" src="https://github.com/user-attachments/assets/0401ed6e-ff76-4c4f-9e92-c4e12fb85424" />
<img width="415" height="344" alt="image" src="https://github.com/user-attachments/assets/5fca012e-2e3a-453e-9ab3-c5bf62371d8c" />
<img width="604" height="509" alt="image" src="https://github.com/user-attachments/assets/40fe24b6-a8a3-4abd-9ca2-4602c2ea46dd" />


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
