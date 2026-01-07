# Flight Computer Firmware

This repository contains the **C++ firmware** and hardware design files for a custom **rocket flight computer** built around a Teensy development kit. The system is designed for my L1 model rocket and emphasizes low-level control and modular sensor drivers.

All firmware is written **from scratch in C++**, with no high-level sensor abstraction libraries, to allow full control over hardware configuration, timing, and data flow.

---

## System Overview

The flight computer integrates multiple sensing, estimation, and communication subsystems to estimate vehicle state and transmit telemetry during flight.

The current implementation focuses on:
- **Altitude estimation using an Extended Kalman Filter (EKF)**
- **Low-level sensor interfacing over I2C**
- **LoRa-based telemetry**
- **Modular and scalable firmware architecture**

---

## Project Structure

```text
src/
├── Controls/
│   ├── ekf.cpp
│   └── ekf.h
│
├── Sensing/
│   ├── imu.cpp
│   ├── imu.h
│   ├── baro.cpp
│   ├── baro.h
│   ├── mag.cpp
│   └── mag.h
│
├── Utilities/
│   ├── i2c.cpp
│   └── i2c.h
│
├── LoRa/
│   ├── transmitter.cpp
│   └── receiver.cpp
│
└── main.cpp
