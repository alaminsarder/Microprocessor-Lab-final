# Gas Leak Detector with Alarm

A simple safety project that detects LPG/smoke, shows the gas level on an OLED display, sounds a buzzer alarm, and automatically turns on a DC exhaust fan.

Built with **NodeMCU (ESP8266)** and an **MQ-2** gas sensor.

## Features

- Reads gas level from MQ-2 sensor (0–1023)
- Live gas value and bar graph on OLED
- Shows `NORMAL` or `GAS LEAK!` status
- Buzzer beeps when gas is high
- Exhaust fan turns on automatically through a relay
- Everything returns to normal when the air is clean

## Components

| Component | Purpose |
|---|---|
| NodeMCU (ESP8266) | Main controller |
| MQ-2 gas sensor | Detects LPG / smoke |
| 0.96" OLED (SSD1306, I2C) | Displays gas level and status |
| Buzzer | Alarm sound |
| 5V relay module | Switches the fan |
| DC exhaust fan + power supply | Removes gas from the room |
| Breadboard, jumper wires | Connections |

## Wiring

| Device | NodeMCU Pin |
|---|---|
| MQ-2 AO | A0 |
| Buzzer (+) | D5 |
| Relay IN | D6 |
| OLED SDA | D2 |
| OLED SCL | D1 |
| OLED / MQ-2 / Relay VCC | 3.3V or 5V (as per module) |
| All GND | GND (common ground) |

Fan connection: DC supply (+) → relay **COM**, relay **NO** → fan (+), fan (−) → supply (−).

## How It Works

| Condition | Gas Value | OLED | Buzzer | Fan |
|---|---|---|---|---|
| Normal | 650 or below | NORMAL | OFF | OFF |
| Gas leak | Above 650 | GAS LEAK! | Beeping | ON |

The relay is active-LOW. Because the NodeMCU gives 3.3V and the relay needs 5V, the relay pin is set to `OUTPUT LOW` for ON and `INPUT` (high impedance) for OFF, so the relay switches off fully.

## Setup

1. Install the **Arduino IDE** and add ESP8266 board support.
2. Install these libraries from Library Manager:
   - `Adafruit GFX Library`
   - `Adafruit SSD1306`
3. Select board **NodeMCU 1.0 (ESP-12E Module)**.
4. Open `gas_leak_detector.ino`, connect the board, and upload.
5. Open Serial Monitor at **115200 baud** to see live gas values.

## Tips

- Let the MQ-2 warm up for a few minutes before testing, since readings are unstable at first.
- Change `GAS_THRESHOLD` (default `650`) to suit your room.
- Test safely with a gas lighter (no flame) held near the sensor.
- This is a learning project, not a certified safety device.

## Team

| ID |
|---|
| 0322410105101009 |
| 0322410105101010 |
| 0322410105101011 |
