# esp32-spwm
SPWM (sinusoidal PWM) implementation for the ESP32 microcontroller

Inspired by https://github.com/Irev-Dev/Arduino-Atmel-sPWM, see there for more details about the technique.

The example code should produce a 1500 Hz sine wave on pin 12 of the board. That's because the lookup table consists of 32 values and the frequency at which we step through the table is 48000.

To smooth the PWM output into a sine wave and thus test the circuit, I used a low-pass RC filter with R = 2k and C = 100n.
