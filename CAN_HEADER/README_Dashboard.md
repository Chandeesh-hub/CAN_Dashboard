# Main Dashboard ECU (CAN Bus Receiver)

The main dashboard is the **display node** of a CAN-based automotive dashboard project. It runs on a PIC18 microcontroller with an on-chip ECAN module (built with MPLAB XC8). It listens on the CAN bus for frames sent by the sensor ECUs, shows **speed, gear and RPM** on a 16x2 character LCD, and drives the **left/right turn-indicator LEDs**.

It works together with:

- **ECU1** – sends speed (`0x10`) and gear (`0x20`).
- **ECU2** – sends RPM (`0x30`) and indicator state (`0x50`).

The dashboard only receives; it never transmits.

---

## Features

- Receives CAN frames and routes them by message ID.
- Shows speed, gear, RPM and indicator state on the CLCD.
- Blinks the left or right indicator LEDs depending on the received indicator state.
- Handler stub for engine temperature, ready to be filled in.

---

## How It Works

### Startup (`main.c`)

1. `init_clcd()` – initialises the 16x2 LCD.
2. `init_leds()` – configures PORTB (RB3 input for CAN RX, all others output) and clears it.
3. `init_can()` – configures the ECAN module (normal mode, accepts all messages in RXB0).
4. Sets `PEIE` and `GIE`. No interrupt service routine is used in this project.
5. Prints the heading `SD EV RPM  IND  ` on line 1.

### Main loop

1. `process_canbus_data()` – receives one frame (if any) and dispatches it.
2. Blinks the active indicator based on `flag`:

| `flag` | Meaning | Behaviour |
|--------|---------|-----------|
| 0 | Off | Both indicators off |
| 1 | Right | Right LEDs toggle every 25000 loop passes |
| 2 | Left | Left LEDs toggle every 25000 loop passes |

The blink rate is software-timed, so it depends on how long each loop pass takes.

### Message dispatch (`message_handler.c`)

`process_canbus_data()` calls `can_receive()`, null-terminates the payload, and then checks `msg_id`:

| Message | ID | Handler | Action |
|---------|------|---------|--------|
| Speed | `0x10` | `handle_speed_data` | Prints 2 ASCII digits at `LINE2(0)` |
| Gear | `0x20` | `handle_gear_data` | Prints gear text (e.g. `G3`) at `LINE2(3)` |
| RPM | `0x30` | `handle_rpm_data` | Prints 4 ASCII digits at `LINE2(6)` |
| Engine temp | `0x40` | `handle_engine_temp_data` | Empty stub, not dispatched yet |
| Indicator | `0x50` | `handle_indicator_data` | Sets `flag`, prints `OFF`, `-->` or `<--` at `LINE2(11)` |

Indicator payload values (sent by ECU2):

| Payload | State | `flag` | LCD text | LEDs |
|---------|-------|--------|----------|------|
| `'0'` | Off | 0 | `OFF` | Both off |
| `'1'` | Right | 1 | `-->` | Left off, right blinks |
| `'2'` | Left | 2 | `<--` | Right off, left blinks |

### LCD layout

```
Column:  0123456789012345
Line 1:  SD EV RPM  IND
Line 2:  42 G3  3450 -->
```

| Field | Columns |
|-------|---------|
| Speed | 0–1 |
| Gear | 3–5 |
| RPM | 6–9 |
| Indicator | 11–13 |

Possible gear texts from ECU1: `ON`, `NE`, `G1`–`G5`, `R`, `CO` (collision).

---

## Hardware Connections

| Function | Pin | Notes |
|----------|-----|-------|
| CAN TX | RB2 | Via CAN transceiver |
| CAN RX | RB3 | Via CAN transceiver |
| Right indicator LEDs | RB6, RB7 | `RIGHT_IND_*` macros (mask `0xC0`) |
| Left indicator LEDs | RB0, RB1 | `LEFT_IND_*` macros (mask `0x03`) |
| CLCD data | PORTD (RD0–RD7) | RD7 is also the busy flag |
| CLCD RW / RS / EN | RC0 / RC1 / RC2 | |

**Clock:** 20 MHz (`_XTAL_FREQ` in `clcd.h`).

---

## Project Structure

| File | Purpose |
|------|---------|
| `main.c` | Entry point, peripheral setup, indicator blinking |
| `message_handler.c/.h` | CAN frame dispatch and per-message handlers, LED macros |
| `can.c/.h` | ECAN init, transmit and receive |
| `clcd.c/.h` | 16x2 character LCD driver |
| `msg_id.h` | CAN message ID definitions (shared by all ECUs) |
| `Makefile` | Build configuration |

---

## Building

1. Open the project in **MPLAB X IDE** with the **XC8** compiler, or build with the provided `Makefile`.
2. Select your PIC18 target device (one with an ECAN module).
3. Build and program the board.

All ECUs on the bus must use the same message IDs (`msg_id.h`) and the same CAN bit timing.

---

## Known Issues / Notes

- **Stale `msg_id`:** `msg_id` is a global that is only updated when a frame arrives. When no new frame is waiting (`len == 0`), the last ID is still matched, so the handler runs again on empty data every loop pass. This wastes LCD writes and slows the loop. Process data only when `len > 0`.
- **Possible buffer overflow:** `data` holds 8 bytes, but the code writes `data[len] = '\0'`. A full 8-byte frame would write to `data[8]`. Make the buffer 9 bytes.
- **Frames may be lost:** only receive buffer RXB0 is used, with no rollover to RXB1. Each LCD write waits on the busy flag, so a slow loop can miss frames from the two ECUs. Check the overflow flag or enable buffer rollover if you see dropped data.
- **Indicator blink timing:** blinking is based on loop iterations (25000), so the rate changes with LCD activity. A timer would give a fixed rate.
- **Engine temperature not handled:** `handle_engine_temp_data` is empty, and `process_canbus_data` doesn't check `ENG_TEMP_MSG_ID`. No ECU sends it yet either.
- **Unused declarations:** `message_handler.h` declares `extern volatile unsigned char led_state, status;`, which are not defined anywhere. The `IndicatorStatus` enum is not used here either.
- **Interrupts enabled without an ISR:** `GIE` and `PEIE` are set but no interrupt is used. This is harmless now, but any interrupt enabled later would jump to an undefined vector.
- **Heading label:** the line-1 heading `EV` sits above the gear column. It may be meant to read `GR`.
- **Receive filters:** `init_can()` configures RXB0 to accept all messages (`RXM0 = RXM1 = 1`), with no filters or masks.
- **CAN bit timing:** the comment in `can.c` says "8 MHz" while the clock is 20 MHz. Verify `BRGCON1/2/3` give the bit rate you expect, and that all ECUs use the same values.
- **Pin sharing:** the LED masks must not include RB2 or RB3, since those are the CAN pins.

---

## Author

chandeesh
