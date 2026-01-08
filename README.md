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
```
## Folder Descriptions

### **Controls**
Contains control and estimation algorithms.

- **`ekf.cpp / ekf.h`**  
  Implements an **Extended Kalman Filter (EKF)** used to estimate the vehicle’s **Z-axis state (altitude)** during flight.  
  The EKF fuses inertial and barometric measurements and is designed to be expanded to full **3-axis state estimation**.

---

### **Sensing**
Contains low-level sensor drivers, each split into a header and implementation file.

- **IMU (`imu.cpp / imu.h`)**  
  Provides accelerometer and gyroscope data for motion sensing.

- **Barometer (`baro.cpp / baro.h`)**  
  Provides pressure-based altitude measurements used by the EKF.

All sensors are configured and read at the **register level**, without relying on vendor abstraction libraries.

---

### **Utilities**
Contains reusable hardware communication helpers.

- **I2C (`i2c.cpp / i2c.h`)**  
  Implements low-level I2C read/write utilities, multi-byte transactions, and device probing.  
  This layer is shared across all sensor drivers.

---

## **LoRa**
Contains long-range wireless communication code used for flight telemetry and ground station reception.

The LoRa subsystem is designed to transmit **compact, fixed-length binary packets** for reliability, low latency, and ease of parsing on resource-constrained hardware.

---

### **Telemetry Packet Format**

Each LoRa transmission consists of a **fixed 11-byte packet** with the following structure:

| Byte Index | Field       | Size (bytes) | Type      | Description |
|-----------:|-------------|--------------|-----------|-------------|
| 0          | Packet ID   | 1            | `uint8_t` | Reserved for packet type / sync|
| 1–4        | Timestamp   | 4            | `uint32_t`| System timestamp in **milliseconds** |
| 5–6        | Temperature | 2            | `int16_t` | Temperature in **°C × 100** (fixed-point) |
| 7–10       | Pressure    | 4            | `int32_t` | Pressure in **Pascals (Pa)** |

**Total packet size:** `11 bytes`

---

### **Encoding Details**

- **Timestamp**
  - Big-endian format
  - Represents system uptime in milliseconds
  - Used for packet ordering and time correlation

- **Temperature**
  - Signed 16-bit fixed-point value  
  - Conversion:  
    ```
    temperature (°C) = int16_value / 100.0
    ```
  - Example: `2534 → 25.34 °C`

- **Pressure**
  - Signed 32-bit integer
  - Units: **Pascals (Pa)**
  - Intended for altitude calculation and EKF validation on the ground station

---

### **`transmitter.cpp`**
Packages sensor telemetry into the fixed 11-byte binary packet and transmits it over LoRa at **915 MHz**.

Responsibilities include:
- Packing sensor values into fixed-point binary fields
- Enforcing a fixed packet length
- Periodic telemetry transmission during flight

The packet structure is intentionally minimal to:
- Reduce airtime
- Improve link reliability
- Avoid dynamic memory allocation

---

### **`receiver.cpp`**
Receives and validates incoming LoRa packets on the ground station.

Responsibilities include:
- Verifying packet length (`11 bytes`)
- Parsing binary fields into native types
- Converting fixed-point values to floating-point
- Displaying telemetry via **Serial** and **OLED**

Packets with unexpected lengths are discarded to ensure data integrity.

---

### **Design**
Contains the hardware design files for the flight computer PCB.

This includes:
- PCB layout files  
- Schematics  
- Manufacturing and fabrication outputs  

These files document the **complete electrical design** of the flight computer hardware.

---

## `main.cpp`

`main.cpp` serves as the system entry point and integrates all subsystems. It:

- Initializes I2C and sensors  
- Initializes the EKF  
- Continuously reads sensor data  
- Runs the EKF update loop  
- Monitors the **Z-axis (altitude) state estimate** in real time  
- Interfaces with the LoRa telemetry system  

---

## Design Goals

- C++ firmware for embedded flight systems  
- Register-level sensor control  
- Clear separation between sensing, utilities, control, and communication  
- EKF-based state estimation  
- Reliable long-range telemetry  
- Scalable architecture for future expansion  

---

## Build Environment

- **Language:** C++  
- **Framework:** Arduino (STM32 core)  
- **Build System:** PlatformIO  
- **Target:** Custom STM32-based flight computer hardware  

