# ECU1 – Speed & Gear Sensor Node (CAN Bus)

ECU1 is the **sensor node** of a CAN-based automotive dashboard project. It runs on a PIC18 microcontroller with an on-chip ECAN module (built with MPLAB XC8). It reads vehicle **speed** from an analog input and **gear position** from a matrix keypad, then broadcasts both over the CAN bus every loop iteration so another ECU (e.g. the dashboard/display ECU) can show them.

---

## Features

- **Speed sensing** – potentiometer on ADC channel 4 (AN4), scaled to 0–99.
- **Gear selection** – keypad switches for gear up, gear down and collision.
- **CAN transmission** – speed and gear are sent as separate frames with standard 11-bit IDs.
- **Loop-back debug code** – commented-out blocks in `ecu1_sensor.c` let you test on a single board using the CLCD.

---

## How It Works

### Main loop (`ecu1_main.c`)

1. Initialises the keypad (`matrix_config`), ADC (`init_adc`) and CAN (`init_can`).
2. In an infinite loop:
   - `get_speed()` – reads ADC, converts, transmits the speed frame.
   - `get_gear_pos()` – reads keypad, updates gear, transmits the gear frame.

### Speed (`get_speed`)

- Reads the 10-bit ADC result (0–1023) from channel `SPEED_ADC_CHANNEL` (`0x04`).
- Scales it to 0–99: `speed = adc / 10.33`.
- Converts to two ASCII digits (e.g. `"42"`).
- Transmits **2 bytes** with ID `SPEED_MSG_ID` (`0x10`), then waits 80 ms.

### Gear (`get_gear_pos`)

Uses the keypad in *state-change* mode, so one key press gives one action.

| Key | Action |
|-----|--------|
| 1 (`GEAR_UP`) | Gear up. Above G5 it jumps to Reverse (`MAX_GEAR` = 7). |
| 2 (`GEAR_DOWN`) | Gear down, stopping at 0 (`ON`). |
| 3 (`COLLISION`) | Sets gear to 8 (`CO`, collision). |

Gear table (`gear_array`):

| Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|-------|----|----|----|----|----|----|----|----|----|
| Text | `ON` | `NE` | `G1` | `G2` | `G3` | `G4` | `G5` | `R` | `CO` |

The 4-byte string for the current gear is transmitted with ID `GEAR_MSG_ID` (`0x20`), then waits 80 ms.

---

## CAN Message Map

| Message | ID | Length | Payload |
|---------|------|--------|---------|
| Speed | `0x10` | 2 | ASCII digits, e.g. `'4','2'` |
| Gear | `0x20` | 4 | 3-char text + `'\0'`, e.g. `"G3 \0"` |
| RPM | `0x30` | – | Reserved (not sent by ECU1) |
| Engine temp | `0x40` | – | Reserved (not sent by ECU1) |
| Indicator | `0x50` | – | Reserved (not sent by ECU1) |

IDs are defined in `msg_id.h`.

---

## Hardware Connections

| Function | Pin | Notes |
|----------|-----|-------|
| Speed input (potentiometer) | AN4 | ADC channel 4 |
| CAN TX | RB2 | Via CAN transceiver |
| CAN RX | RB3 | Via CAN transceiver |
| Keypad rows | RB5, RB6, RB7 | Outputs |
| Keypad column 1 | RB1 | Input, PORTB pull-ups enabled |
| CLCD data | PORTD | Used for debug output |
| CLCD RS / RW / EN | RC1 / RC0 / RC2 | |

Only column 1 is scanned, so there are three switches: SW1, SW2, SW3.

**Clock:** 20 MHz (`_XTAL_FREQ` in `clcd.h`).

---

## Project Structure

| File | Purpose |
|------|---------|
| `ecu1_main.c` | Entry point and main loop |
| `ecu1_sensor.c/.h` | Speed and gear logic |
| `adc.c/.h` | ADC init and channel read |
| `can.c/.h` | ECAN init, transmit and receive |
| `matrix_keypad.c/.h` | Keypad scanning, level and state-change reading |
| `clcd.c/.h` | Character LCD driver (debug / loop-back use) |
| `msg_id.h` | CAN message ID definitions |
| `Makefile` | Build configuration |

---

## Building

1. Open the project in **MPLAB X IDE** with the **XC8** compiler, or build with the provided `Makefile`.
2. Select your PIC18 target device (one with an ECAN module).
3. Build and program the board.

---

## Testing in Loop-back Mode

`ecu1_sensor.c` contains commented-out loop-back code for single-board testing:

1. In `can.c` (`init_can`), switch the operation mode to `e_can_op_mode_loop`.
2. Call `init_clcd()` in `main`.
3. Uncomment the loop-back blocks in `get_speed()` and `get_gear_pos()` to read back the frames and print them on the CLCD.

---

## Known Issues / Notes

- **Shadowed `gear` variable:** `get_gear_pos()` declares `static unsigned char gear`, which hides the global `gear`. The `gear = 0` reset in `main` after a collision has no effect, so ECU1 stays in `CO` until a gear key is pressed (gear up goes to `R`, gear down goes to `R` as well since 8 → 7). Remove the local static, or return to `ON` explicitly, to fix this.
- **Gear-up after G5:** the check `gear > 6` jumps straight to Reverse (7) rather than stopping at G5. Confirm this is the intended behaviour.
- **Pin sharing:** `matrix_config()` sets `TRISB = 0x1E`, then `init_can()` changes RB2 to output and RB3 to input. Columns 2 and 3 (RB2, RB3) conflict with CAN pins, which is fine only because they are unused.
- **Receive filters:** `init_can()` configures RXB0 to accept all messages (`RXM0 = RXM1 = 1`), and no filters or masks are set.
- **CAN bit timing:** the comment in `can.c` says "8 MHz" while the clock is 20 MHz. Verify `BRGCON1/2/3` give the bit rate you expect, and that all ECUs on the bus use the same timing.
- **`can_transmit`** has no error handling, so the length must be 8 or less, and it does not check whether the buffer is free before writing.
- **`init_can()`** is called without `init_clcd()`, so the CLCD isn't initialised unless you enable debug code.
- The 80 ms `__delay_ms` after each transmit limits the update rate to roughly 6 frames per second.

---

## Author

chandeesh
