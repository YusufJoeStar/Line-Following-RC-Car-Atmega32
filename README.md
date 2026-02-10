

## Project Overview
This project is a **university group project** completed by a team of 4 members within a 1-week timeframe as part of the **Microcontrollers course**.  

The objective was to design and implement a **line-following robotic car** under strict hardware and software constraints imposed by the course.



---

## Design Constraints and Rules
The project was developed under the following enforced constraints:

- **Only the ATmega32 microcontroller was allowed**
- **No development boards** (e.g. Arduino, ESP boards, or similar) were permitted
- **No pre-assembled PCBs or motor driver modules** beyond basic H-bridge circuitry
- All logic and control had to be implemented **directly on the microcontroller**



---

## Sensors
- The car uses **TCRT5000 infrared digital line sensors**
- Sensors provide **digital HIGH/LOW output**, not analog values
- Line detection is based on reflection differences between the track and background
- Sensor readings are processed directly by the ATmega32 GPIO pins



---

## Motor Control Strategy
- The car relies **entirely on timer-based logic**
- **PWM (Pulse Width Modulation)** is generated using the ATmega32’s internal timers
- **PWM Configuration** was set on **Fast PWM** and **Inverting Mode**.
- Motor speed and steering adjustments are achieved through:
  - Duty cycle manipulation
  - Direction control via H-bridge outputs
- **No PID control algorithm was used**
- All movement decisions are based on:
  - Sensor states
  - Timer configuration
  - Fixed duty-cycle logic

| Sensor State | Action |
|-------------|--------|
| Center detected | Move forward |
| Left detected   | Turn left |
| Right detected  | Turn right |
| All directions detected  | Move forward | 
| No direction detected  | Stop |
<img width="1110" height="650" alt="image" src="https://github.com/user-attachments/assets/f1aad680-5846-40f2-9191-26b8745537ee" />
<img width="1467" height="270" alt="image" src="https://github.com/user-attachments/assets/ae39f27d-3c34-46e8-80c0-22ccdef0016c" />



![rc-car-sensor-test](https://github.com/user-attachments/assets/161cd790-c52e-41c2-bef5-7d475965b032)






---

## Clock Configuration and Programming
- An **external crystal oscillator** was used for accurate timing
- **Fuse bits were manually configured** to match the external oscillator frequency
- Programming and fuse configuration were done using:
  - **USBASP AVR programmer**
  - **AVRDUDE / AVRDUDEss application**
- The same toolchain was used to:
  - Burn the firmware
  - Configure clock source and frequency
  - Ensure correct timer behavior for PWM generation

<img width="367" height="335" alt="image" src="https://github.com/user-attachments/assets/555e9f3f-10fb-43c0-99fb-a82b3c1c71b3" />
<img width="415" height="354" alt="image" src="https://github.com/user-attachments/assets/25595668-82c4-46fc-9e21-a028ab756992" />




---

## Tech Stack
- **Microcontroller:** ATmega32  
- **Programming Language:** C (Microchip Studio IDE)  
- **Sensors:** TCRT5000 IR Digital Sensors  
- **Motor Control:** H-bridge (L298N Module)  
- **Clock Source:** External crystal oscillator (16MHz)  
- **Programmer:** USBASP avr programmer  
- **Programming Tool:** AVRDUDE / AVRDUDEss  
- **Simulation:** Proteus  

---

## Limitations, Flaws, and Possible Improvements

While the project successfully met all course requirements and functioned as intended within the limited timeframe, several limitations were identified in the final prototype. These limitations also highlight clear opportunities for future improvement.

### Hardware and Safety Limitations
- The initial design planned for the inclusion of a **Start button**, **Reset button** and **5 Sensors**; however, these were **not implemented in the final version** due to time constraints.
- The absence of these controls causes **potential safety concerns**, as the car begins operating immediately once power is applied.
- Without proper shutdown or reset mechanism, the system experiences **unnecessary battery drainage** when not in use.

### Mechanical and Wiring Limitations
- Due to the short work period, **wire management was not optimized**, resulting in a cluttered wiring layout.
- Apart from the basic chassis, **no additional mechanical structure or enclosure** was designed to properly secure and protect the electronic components.
- A more refined mechanical design would improve looks, safety, and overall system reliability.

### Control Algorithm Limitations
- The line-following behavior relies on **basic conditional logic combined with PWM duty cycle control**.
- **No PID control algorithm was implemented**, which limits the system’s ability to smoothly correct positional errors and handle sharp or complex path variations.
- This decision was intentional, as **PID control had not yet been covered in the coursework** at the time of project development.
- Implementing PID control in future iterations would significantly improve tracking accuracy, stability, and responsiveness.

### Future Improvements
- Add hardware **Start/Stop and Reset buttons** for improved safety and power management.
- Improve **wire organization and mechanical housing** to enhance durability and maintainability.
- Use **PID-based control** for smoother motion and better error correction once covered academically.


---


