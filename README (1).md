# 6-DOF Robotic Arm Control System — ATmega32

**Bare-Metal C • PCA9685 • I2C LCD • EEPROM • Proteus**

A complete embedded control system for a **6-DOF robotic arm** developed around the **ATmega32** using custom C drivers and a layered **MCAL/HAL** architecture.

The repository includes the firmware source code and the Proteus simulation used to validate the **I2C bus, LCD, keypad, PCA9685, EEPROM, and servo PWM outputs**.

---

## What It Does

- **6-DOF Arm Control** — controls six servo channels through the PCA9685.
- **Manual Joint Control** — keys `1–6` select a joint; `7` and `8` command movement.
- **Continuous Wrist Control** — J4 uses direction / stop pulse commands.
- **Pose Storage** — saves and restores positional joints using the ATmega32 internal EEPROM.
- **Home Command** — returns the arm to predefined safe positions.
- **LCD Menu System** — 16x2 LCD through PCF8574 over I2C.
- **Proteus Validation** — LCD, keypad, I2C and PWM behavior verified in simulation.

---

## Hardware

| Component | Role / Configuration |
|---|---|
| **ATmega32** | Main MCU @ 8 MHz |
| **PCA9685** | 16-channel PWM servo driver, I2C address `0x40` |
| **PCF8574** | I2C LCD expander, address `0x27` |
| **16x2 LCD** | Menu and joint-state display |
| **3x3 Keypad** | Manual control and command input |
| **Servo Channels CH0–CH5** | Five positional joints + one continuous-rotation wrist |
| **Internal EEPROM** | Pose save / load |
| **4.7 kΩ Resistors** | I2C pull-ups for SDA and SCL |

---

## Servo Mapping

| Channel | Joint | Type |
|---|---|---|
| `CH0` | J1 — Base | Positional Servo |
| `CH1` | J2 — Shoulder | Positional Servo |
| `CH2` | J3 — Elbow | Positional Servo |
| `CH3` | J4 — Wrist 1 | Continuous Rotation Servo |
| `CH4` | J5 — Wrist 2 | Positional Servo |
| `CH5` | J6 — Gripper | Positional Servo |

---

## I2C Connections

| Signal | ATmega32 Pin |
|---|---|
| `SCL` | `PC0` |
| `SDA` | `PC1` |

Both lines use **4.7 kΩ pull-up resistors to +5 V**.

### Device Addresses

| Device | Address |
|---|---|
| PCA9685 | `0x40` |
| PCF8574 | `0x27` |

---

## Keypad Wiring

| Keypad Line | ATmega32 |
|---|---|
| Column 1 | `PB0` |
| Column 2 | `PB1` |
| Column 3 | `PB2` |
| Row A | `PB4` |
| Row B | `PB5` |
| Row C | `PB6` |

---

## User Interface

### Startup

```text
WELCOME TO
ARM ROBOT
```

### Main Menu

```text
1:MANUAL OP
2:SAVED POS
```

### Manual Mode

| Key | Function |
|---|---|
| `1` | Select J1 — Base |
| `2` | Select J2 — Shoulder |
| `3` | Select J3 — Elbow |
| `4` | Select J4 — Wrist 1 |
| `5` | Select J5 — Wrist 2 |
| `6` | Select J6 — Gripper |
| `7` | Move / Rotate Negative |
| `8` | Move / Rotate Positive |
| `9` | Open Command Menu |

### Command Menu

| Key | Function |
|---|---|
| `1` | Save Pose |
| `2` | Load Pose |
| `3` | Home Position |
| `9` | Back |

---

## PWM Control

The PCA9685 runs at approximately **50 Hz**.

### Positional Servo Calibration

| Position | Pulse Width |
|---|---|
| `0°` | ~450 µs |
| `90°` | ~1500 µs |
| `180°` | ~2550 µs |

### Continuous Rotation Wrist

| Command | Pulse Width |
|---|---|
| Direction 1 | 1740 µs |
| Stop | 1800 µs |
| Direction 2 | 1950 µs |

---

## EEPROM Pose Storage

The ATmega32 internal EEPROM stores one arm pose.

Stored joints:

- Base
- Shoulder
- Elbow
- Wrist 2
- Gripper

> J4 is not stored as an absolute angle because it is a continuous-rotation servo.

---

## Software Architecture

```text
APP
│
├── HAL
│   ├── ROBOT_ARM
│   ├── PCA9685
│   ├── LCD_I2C
│   └── KPD
│
├── MCAL
│   ├── DIO
│   ├── TWI
│   └── EEPROM
│
└── LIB
    ├── STD_TYPES
    └── BIT_MATH
```

---

## Repository Structure

```text
ITI-Arm-Robot/
│
├── Firmware/
│   ├── APP/
│   ├── HAL/
│   │   ├── KPD/
│   │   ├── LCD_I2C/
│   │   ├── PCA9685/
│   │   └── ROBOT_ARM/
│   ├── MCAL/
│   │   ├── DIO/
│   │   ├── TWI/
│   │   └── EEPROM/
│   └── LIB/
│
├── Proteus/
│   └── ITI_Arm_Robot.pdsprj
│
├── README.md
└── .gitignore
```

---

## Proteus Simulation

The Proteus project includes:

- ATmega32
- PCA9685
- PCF8574
- 16x2 LCD
- 3x3 keypad
- Six servo channels
- I2C pull-up resistors
- Digital oscilloscope for PWM verification

> **Simulation note:** Proteus' standard PWM servo model behaves as a positional servo.  
> J4 continuous-rotation behavior is therefore verified by its PWM waveform.

---

## Development Tools

- C
- AVR-GCC 7.3.0
- Eclipse AVR
- Proteus 8 Professional
- Git / GitHub

**Target MCU:** ATmega32 @ 8 MHz

---

## Current Status

- [x] ATmega32 firmware
- [x] PCA9685 communication
- [x] Servo PWM generation
- [x] Manual keypad control
- [x] LCD interface
- [x] EEPROM pose save / load
- [x] Home position
- [x] Proteus simulation
- [x] PWM verification using oscilloscope

---

## Author

**Ziad Abosalem**

Embedded Systems Project  
**Information Technology Institute — ITI**
