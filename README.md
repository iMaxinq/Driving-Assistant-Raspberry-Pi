# Driving Assistant for Raspberry Pi

A Raspberry Pi–based driving assistant written in C that monitors **driver drowsiness**, **sharp steering changes**, and **following distance**, then activates visual alerts and a servo-based braking response. The project uses multiple concurrent threads with OpenMP and interfaces with hardware through WiringPi, SPI, and I2C.

## What this project does

This application combines sensor inputs and simple decision logic to act like a basic in-vehicle driver assistance prototype.

It continuously monitors:

- **Head tilt / drowsiness** using accelerometer tilt on the X and Y axes
- **Sharp turns** using rapid steering wheel angle changes
- **Distance to the obstacle ahead** using an ultrasonic sensor
- **Vehicle speed** from an analog input
- **Steering wheel angle** from an analog input

Based on those signals, it:

- lights LEDs as warnings
- classifies distance risk into multiple levels
- commands a **servo motor** to simulate increasing braking intensity

## Repository contents

```text
Driving-Assistant-Raspberry-Pi/
├── Makefile
├── devices.c
├── devices.h
├── threads.c
├── bdd.png
└── stateMachine.png
