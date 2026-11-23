/*
 * ==========================================================
 *  Gas Leak Detector with Alarm
 * ==========================================================
 *  Board   : NodeMCU (ESP8266)
 *  Sensor  : MQ-2 (LPG / smoke)
 *  Display : 0.96" OLED (SSD1306, I2C, address 0x3C)
 *  Output  : Buzzer + 5V Relay (controls DC exhaust fan)
 *
 *  Working:
 *    - Read gas level from MQ-2 (0 - 1023)
 *    - Show level + bar graph on OLED
 *    - If level > threshold -> show "GAS LEAK!", beep buzzer,
 *      turn ON the exhaust fan
 *    - Otherwise -> show "NORMAL", buzzer OFF, fan OFF
 * ==========================================================
 */

#include <Wire.h>              // I2C communication (for OLED)
#include <Adafruit_GFX.h>      // Graphics library (text, shapes)
#include <Adafruit_SSD1306.h>  // OLED driver

// ---------- OLED settings ----------
#define SCREEN_WIDTH  128      // OLED width in pixels
#define SCREEN_HEIGHT 64       // OLED height in pixels
#define OLED_RESET    -1       // No reset pin used

// Create the display object (uses I2C / Wire)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Pin definitions (NodeMCU) ----------
const int MQ2_PIN    = A0;     // MQ-2 analog output (AO)
const int BUZZER_PIN = D5;     // Buzzer
const int RELAY_PIN  = D6;     // Relay module IN pin (fan control)

// ---------- Settings ----------
const int GAS_THRESHOLD = 650; // Above this value = gas leak
                               // (change it to match your environment)

// ----------------------------------------------------------
//  Relay control
//  The relay module works at 5V but NodeMCU gives only 3.3V.
//  So a HIGH signal may not switch the relay fully OFF.
//  Trick:
//    ON  -> pin as OUTPUT and LOW  (active-LOW relay)
//    OFF -> pin as INPUT (high impedance), relay turns fully OFF
// ----------------------------------------------------------
void turnRelayON() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);   // Relay ON  -> Fan ON
}

void turnRelayOFF() {
  pinMode(RELAY_PIN, INPUT);      // Relay OFF -> Fan OFF
}

// ----------------------------------------------------------
//  setup() runs only once when the board starts
// ----------------------------------------------------------
void setup() {
  Serial.begin(115200);           // Serial monitor for debugging

  pinMode(BUZZER_PIN, OUTPUT);    // Buzzer is an output
  pinMode(MQ2_PIN, INPUT);        // Sensor is an input

  digitalWrite(BUZZER_PIN, LOW);  // Buzzer OFF at start
  turnRelayOFF();                 // Fan OFF at start

  Wire.begin(D2, D1);             // I2C pins: SDA = D2, SCL = D1

  // Start the OLED. If it is not found, stop here.
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED detect hoyni! Wiring check korun."));
    for (;;);                     // Infinite loop (stop program)
  }

  // ----- Welcome screen -----
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE); // Border
  display.setTextSize(1);
  display.setCursor(16, 20);
  display.println("GAS LEAK DETECTOR");
  display.setCursor(28, 38);
  display.println("Warming up...");
  display.display();              // Send everything to the screen
  delay(2500);                    // Give the sensor some warm-up time
}

// ----------------------------------------------------------
//  loop() runs again and again
// ----------------------------------------------------------
void loop() {
  // 1) Read gas level from the sensor (0 - 1023)
  int gasValue = analogRead(MQ2_PIN);

  // Also print it on the Serial Monitor
  Serial.print("Gas Level: ");
  Serial.println(gasValue);

  // 2) Draw the common part of the screen
  display.clearDisplay();
  display.drawRoundRect(0, 0, 128, 64, 4, SSD1306_WHITE); // Border

  // Gas level text and number
  display.setTextSize(1);
  display.setCursor(8, 8);
  display.print("GAS LEVEL:");
  display.setTextSize(2);
  display.setCursor(72, 5);
  display.print(gasValue);

  // Bar graph: convert 0-1023 into 0-108 pixels wide
  int barWidth = map(constrain(gasValue, 0, 1023), 0, 1023, 0, 108);
  display.drawRect(8, 26, 112, 8, SSD1306_WHITE);   // Bar outline
  display.fillRect(10, 28, barWidth, 4, SSD1306_WHITE); // Filled part

  // 3) Decide: leak or normal?
  if (gasValue > GAS_THRESHOLD) {
    // ----- GAS LEAK -----
    // White box with black text so the warning stands out
    display.fillRect(6, 40, 116, 18, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(2);
    display.setCursor(12, 42);
    display.print("GAS LEAK!");
    display.setTextColor(SSD1306_WHITE);   // Reset text color
    display.display();

    turnRelayON();                // Exhaust fan ON

    // Beep the buzzer (200 ms ON, 150 ms OFF)
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(150);
  }
  else {
    // ----- NORMAL -----
    display.setTextSize(2);
    display.setCursor(26, 42);
    display.print("NORMAL");
    display.display();

    turnRelayOFF();               // Exhaust fan OFF
    digitalWrite(BUZZER_PIN, LOW);// Buzzer OFF
    delay(300);                   // Small delay between readings
  }
}
