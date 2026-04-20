# Morse Code Arduino Project

## Description
This project converts input signals into Morse code using an Arduino Uno R3.  
The output is provided through a buzzer and displayed on an OLED screen.  
A push button is used for input.

---

## Components Used
- Arduino Uno R3
- I2C OLED Display (SSD1306)
- Buzzer
- Push Button
- Jumper Wires

---

## Circuit Connections

| Component        | Arduino Uno R3 Pin |
|-----------------|-------------------|
| OLED VCC        | 5V                |
| OLED GND        | GND               |
| OLED SDA        | A4                |
| OLED SCL        | A5                |
| Buzzer (+)      | D8                |
| Buzzer (-)      | GND               |
| Button (1 side) | D7                |
| Button (other)  | GND               |

---

## Working
- Button press generates Morse input  
- Buzzer outputs dot/dash sounds  
- OLED displays corresponding characters  

---

## Notes
- Button uses INPUT_PULLUP (no external resistor needed)  
- OLED uses I2C protocol (usually address 0x3C)  

---

## Future Improvements
- Add keyboard/text input  
- Improve decoding accuracy  
- Add LED indicators  
