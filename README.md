# CAN-Based Automotive Dashboard

A multi-ECU embedded project built on **PIC18 microcontrollers** (with the on-chip ECAN module) and programmed in **C using MPLAB XC8**. Two sensor ECUs read vehicle inputs and broadcast them over a **CAN bus**. A third node, the main dashboard, receives the messages and shows them on a 16x2 character LCD and indicator LEDs.

---

## System Overview

```
 +---------------------+                          +----------------------+
 |        ECU1         |                          |         ECU2         |
 |  Speed (pot, AN4)   |                          |  RPM (pot, AN4)      |
 |  Gear (keypad)      |                          |  Indicator switches  |
 +----------+----------+                          +----------+-----------+
            | 0x10 speed, 0x20 gear                          | 0x30 RPM, 0x50 indicator
            |                                                |
 ===========+================== CAN BUS (CANH / CANL) =======+==============
                                    |
                         +----------+-----------+
                         |   MAIN DASHBOARD     |
                         |  16x2 CLCD display   |
                         |  Indicator LEDs      |
                         +----------------------+
```

| Node | Role | Sends | Receives |
|------|------|-------|----------|
| **ECU1** | Speed and gear sensor | Speed (`0x10`), Gear (`0x20`) | – |
| **ECU2** | RPM and indicator sensor | RPM (`0x30`), Indicator (`0x50`) | – |
| **Dashboard** | Display node | – | All of the above |

---

## CAN Message Map

All frames use standard 11-bit IDs, defined in `msg_id.h` (identical in every project).

| Message | ID | Length | Payload | Sender |
|---------|------|--------|---------|--------|
| Speed | `0x10` | 2 | ASCII digits `00`–`99` | ECU1 |
| Gear | `0x20` | 4 | Text + `'\0'`: `ON`, `NE`, `G1`–`G5`, `R`, `CO` | ECU1 |
| RPM | `0x30` | 4 | ASCII digits `0000`–`6000` | ECU2 |
| Engine temp | `0x40` | – | Reserved, not implemented | – |
| Indicator | `0x50` | 1 | `'0'` off, `'1'` right, `'2'` left | ECU2 |

---

## Repository Structure

```
can-dashboard/
├── README.md              (this file)
├── ecu1/                  Speed and gear node
│   ├── README.md
│   └── ecu1_main.c, ecu1_sensor.c/.h, adc, can, clcd, matrix_keypad, msg_id.h, Makefile
├── ecu2/                  RPM and indicator node
│   ├── README.md
│   └── ecu2_main.c, ecu2_sensor.c/.h, adc, can, clcd, digital_keypad, config, msg_id.h, Makefile
└── dashboard/             Display node
    ├── README.md
    └── main.c, message_handler.c/.h, can, clcd, msg_id.h, Makefile
```

Each folder has its own README with pin connections, code walkthrough and known issues:

- [ECU1](ecu1/README.md)
- [ECU2](ecu2/README.md)
- [Dashboard](dashboard/README.md)

---

## Hardware

Each node needs:

- A PIC18 microcontroller with an ECAN module, clocked at **20 MHz**.
- A CAN transceiver connected to **RB2 (TX)** and **RB3 (RX)**.

Per-node parts:

| Node | Parts |
|------|-------|
| ECU1 | Potentiometer on AN4 (speed), matrix keypad switches (gear up, gear down, collision) |
| ECU2 | Potentiometer on AN4 (RPM), 4 push switches on RC0–RC3, indicator LEDs on RB0/RB1 and RB6/RB7, 4-digit seven-segment display (loop-back testing) |
| Dashboard | 16x2 character LCD (PORTD data, RC0–RC2 control), indicator LEDs on RB0/RB1 and RB6/RB7 |

Connect all transceivers on the same CANH/CANL pair. A CAN bus normally needs a termination resistor at each end of the line.

---

## Getting Started

1. Clone the repository and open one node's folder in **MPLAB X IDE** (XC8 compiler), or build it with that folder's `Makefile`.
2. Select your PIC18 target device and build.
3. Program each of the three boards with its own project.
4. Wire the three boards to the same CAN bus and power them up.

You should see speed, gear and RPM on the LCD, and the indicator arrows `-->` or `<--` with matching LEDs blinking.

### Testing a single node

Each CAN driver has a loop-back mode for testing without the bus. Set `e_can_op_mode_loop` in `init_can()` and uncomment the loop-back blocks in the sensor files. See each folder's README for details.

---

## Known Issues (Project-Wide)

- **CAN bit timing:** `BRGCON1/2/3` are the same in all three projects, but the comment in `can.c` says "8 MHz" while the clock is 20 MHz. Check that the settings give the bit rate you expect. All nodes must use identical timing.
- **Receive filters:** every node accepts all messages in RXB0, with no filters or masks.
- **No CAN error handling:** `can_transmit()` doesn't check whether the transmit buffer is free or whether transmission succeeded.
- **Engine temperature:** the ID is reserved and ECU2 has an unfinished function for it (it always returns 0), but nothing is transmitted or displayed.
- **Duplicated code:** `adc`, `can`, `clcd` and `msg_id.h` are copied into each project. Consider moving them into a shared `common/` folder.

See each node's README for the node-specific bugs.

---

## Author

chandeesh
