# ECU2 – RPM & Indicator Node (CAN Bus)

ECU2 is the second sensor node of a CAN-based automotive dashboard project. It runs on a PIC18 microcontroller with an on-chip ECAN module (built with MPLAB XC8). It reads **engine RPM** from an analog input, handles **left/right turn indicators** from push switches, and broadcasts both over the CAN bus. Another ECU (e.g. the dashboard/display ECU) receives and displays them.

ECU2 works together with **ECU1**, which sends speed and gear.

---

## Features

- **RPM sensing** – potentiometer on ADC channel 4 (AN4), scaled to 0–6000 RPM.
- **Turn indicators** – left, right and off selected by switches, with blinking LEDs.
- **CAN transmission** – RPM and indicator state are sent as separate frames with standard 11-bit IDs.
- **Engine temperature function** – implemented but not yet used or transmitted (see Known Issues).
- **Loop-back debug code** – commented-out blocks let you test RPM on a single board using the 4-digit seven-segment display.

---

## How It Works

### Main loop (`ecu2_main.c`)

1. Initialises LEDs (`led_config`), seven-segment display (`ssd_config`), ADC (`init_adc`), keypad (`init_digital_keypad`) and CAN (`init_can`).
2. In an infinite loop:
   - `process_indicator()` – sends the indicator state, drives the LEDs and reads the switches.
   - Updates the indicator state and the `flag` variable.
   - `get_rpm()` – reads the ADC, converts it and transmits the RPM frame.

### RPM (`get_rpm`)

- Reads the 10-bit ADC result (0–1023) from channel `RPM_ADC_CHANNEL` (`0x04`).
- Scales it to 0–6000: `rpm = (adc / 10.23) * 60`.
- Converts to four ASCII digits (e.g. `"3450"`).
- Transmits **4 bytes** with ID `RPM_MSG_ID` (`0x30`), then waits 80 ms.

### Indicator (`process_indicator`)

Each call does three things:

1. **Transmits** one ASCII byte with ID `INDICATOR_MSG_ID` (`0x50`), then waits 80 ms.
2. **Drives the LEDs** based on `flag`.
3. **Reads the switches** (state-change mode, so one press gives one action).

| Switch | Action | `flag` | CAN payload |
|--------|--------|--------|-------------|
| SW1 | Left indicator on | 2 | `'2'` |
| SW2 | All indicators off | 0 | `'0'` |
| SW3 | Right indicator on | 1 | `'1'` |
| SW4 / no key | No change (previous state kept) | – | – |

Selecting one side turns the other side off. The active indicator toggles every 3 calls of `process_indicator()`, which works out to roughly half a second per toggle given the 80 ms delays in the loop.

### Engine temperature (`get_engine_temp`)

Reads ADC channel 6 (`ENG_TEMP_ADC_CHANNEL`) and returns a scaled value. It is **not called** from `main` and nothing is transmitted with `ENG_TEMP_MSG_ID` yet.

---

## CAN Message Map

| Message | ID | Length | Payload | Sent by ECU2? |
|---------|------|--------|---------|---------------|
| Speed | `0x10` | 2 | ASCII digits | No (ECU1) |
| Gear | `0x20` | 4 | Text, e.g. `"G3 \0"` | No (ECU1) |
| RPM | `0x30` | 4 | ASCII digits, e.g. `'3','4','5','0'` | **Yes** |
| Engine temp | `0x40` | – | Reserved | No (not implemented) |
| Indicator | `0x50` | 1 | `'0'` off, `'1'` right, `'2'` left | **Yes** |

IDs are defined in `msg_id.h`.

---

## Hardware Connections

| Function | Pin | Notes |
|----------|-----|-------|
| RPM input (potentiometer) | AN4 | ADC channel 4 |
| Engine temp input | AN6 | ADC channel 6 (not used yet) |
| CAN TX | RB2 | Via CAN transceiver |
| CAN RX | RB3 | Via CAN transceiver |
| Right indicator LEDs | RB6, RB7 | `RIGHT_IND_*` macros (mask `0xC0`) |
| Left indicator LEDs | RB0, RB1 | `LEFT_IND_*` macros (mask `0x03`) |
| Switches SW1–SW4 | RC0–RC3 | Active low, digital keypad |
| Seven-segment data | PORTD | Used for loop-back display |
| Seven-segment digit select | RA0–RA3 | Used for loop-back display |

**Clock:** 20 MHz (`_XTAL_FREQ` is defined in `ecu2_sensor.c` and `clcd.h`).

---

## Project Structure

| File | Purpose |
|------|---------|
| `ecu2_main.c` | Entry point, main loop, global state |
| `ecu2_sensor.c/.h` | RPM, engine temperature and indicator logic |
| `adc.c/.h` | ADC init and channel read |
| `can.c/.h` | ECAN init, transmit and receive |
| `digital_keypad.c/.h` | Push-switch reading (level and state-change) |
| `config.c/.h` | LED and seven-segment setup, `display()`, digit codes |
| `clcd.c/.h` | Character LCD driver (not used in the current `main`) |
| `msg_id.h` | CAN message ID definitions |
| `Makefile` | Build configuration |

---

## Building

1. Open the project in **MPLAB X IDE** with the **XC8** compiler, or build with the provided `Makefile`.
2. Select your PIC18 target device (one with an ECAN module).
3. Build and program the board.

---

## Testing in Loop-back Mode

`ecu2_sensor.c` contains commented-out loop-back code for single-board testing:

1. In `can.c` (`init_can`), switch the operation mode to `e_can_op_mode_loop`.
2. Uncomment the loop-back block in `get_rpm()`. It receives the RPM frame and shows it on the 4-digit seven-segment display via `display()`.
3. For the indicator, uncomment the receive block in `process_indicator()`.

---

## Known Issues / Notes

- **Engine temperature always returns 0:** `500UL/1023` is integer division and evaluates to 0, so `get_engine_temp()` always gives 0. Use a floating-point or reordered expression (e.g. `temp * 500UL / 1023`).
- **Engine temperature not wired in:** it is never called or transmitted, so `ENG_TEMP_MSG_ID` is unused.
- **Wrong array type in `process_indicator`:** `index` is declared as `unsigned index[4]` (16-bit on PIC18), but its address is passed as a `uint8_t *` to `can_transmit`. It works only because the low byte is read first. Declare it as `unsigned char`.
- **Seven-segment ghosting:** `display()` sets the digit-select bits in `PORTA` but never clears the previous ones, and its inner loop reuses the name `i`. Clear the previous digit before enabling the next one.
- **Mismatched `flag` type:** it is `char` in `ecu2_main.c` but declared `extern unsigned char` in `ecu2_sensor.c`.
- **Enum vs. flag values differ:** `e_ind_left = 1` and `e_ind_right = 2`, but `flag = 1` means right and `flag = 2` means left. It is consistent in the code, but easy to trip over when editing.
- **LED updates use `PORTB`:** read-modify-write on `PORT` registers can behave unexpectedly on PIC18, so writing to `LATB` is safer.
- **Pin sharing:** `led_config()` leaves RB2 and RB3 untouched because they are the CAN pins. The `RIGHT_IND`/`LEFT_IND` masks must not include them.
- **Receive filters:** `init_can()` configures RXB0 to accept all messages (`RXM0 = RXM1 = 1`), with no filters or masks.
- **CAN bit timing:** the comment in `can.c` says "8 MHz" while the clock is 20 MHz. Verify `BRGCON1/2/3` give the bit rate you expect, and that all ECUs on the bus use the same timing.
- **`can_transmit`** has no error handling, so the length must be 8 or less, and it does not check whether the buffer is free before writing.
- **Update rate:** each loop includes two 80 ms delays (RPM and indicator), so frames go out at about 6 per second each.

---

## Author

chandeesh
