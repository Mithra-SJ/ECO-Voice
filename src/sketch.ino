// Context-Aware Offline Voice-Based Smart Home System (Simulation Version)
// Components: PIR, LDR, Potentiometer, Button, 2 LEDs
// Simulation: Wokwi (ESP32 Dev Module)
// Voice input simulated through Serial Monitor

#include <Arduino.h>

#define LED_LIGHT 2       // Relay 1 → LED (Light)
#define LED_FAN 4         // Relay 2 → LED (Fan)
#define PIR_PIN 12        // PIR motion sensor
#define LDR_PIN 34        // LDR analog input
#define POT_PIN 35        // Potentiometer (simulated current sensor)
#define WAKE_BTN 13       // Wake button
#define LDR_DARK_THRESHOLD 2000     // below = dark
#define LOAD_HIGH_THRESHOLD 3000    // above = high load


String authorizedUser = "mithra"; // replace with your name
bool isAwake = false;
bool isAuthenticated = false;
unsigned long lastWakeTime = 0;
const unsigned long WAKE_TIMEOUT = 99999; // 10 seconds

void setup() {
  Serial.begin(115200);
  pinMode(LED_LIGHT, OUTPUT);
  pinMode(LED_FAN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(WAKE_BTN, INPUT_PULLUP);

  Serial.println("===== Context-Aware Smart Home Simulator =====");
  Serial.println("Commands:");
  Serial.println("wake → simulate wake word");
  Serial.println("user:<name> → authenticate");
  Serial.println("light_on, light_off, fan_on, fan_off");
  Serial.println("status → see sensor readings");
  Serial.println("===============================================");
}

void loop() {
  // Check button press for wake simulation
  if ((digitalRead(WAKE_BTN) == LOW || analogRead(PIR_PIN) > 700) && !isAwake) {
    wakeSystem();
}


  // If system awake, handle commands
  if (isAwake) {
    //if (millis() - lastWakeTime > WAKE_TIMEOUT) {
      //lockSystem();
    //} else {
      handleSerialInput();
    //}
  }

  delay(100);
}

// === Wake system ===
void wakeSystem() {
  isAwake = true;
  isAuthenticated = false;
  lastWakeTime = millis();
  Serial.println("[WAKE] System active. Say your username: (user:<name>)");
}

// === Lock system ===
void lockSystem() {
  isAwake = false;
  isAuthenticated = false;
  Serial.println("[LOCK] System timed out.");
}

// === Handle Serial Commands ===
void handleSerialInput() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    lastWakeTime = millis();

    if (input.length() == 0) return;

    if (input.equalsIgnoreCase("wake")) {
      wakeSystem();
      return;
    }

    if (input.equalsIgnoreCase("lock")) {
      lockSystem();
      return;
    }

    if (input.startsWith("user:")) {
      String name = input.substring(5);
      name.trim();
      if (name.equalsIgnoreCase(authorizedUser)) {
        isAuthenticated = true;
        Serial.println("[AUTH] Welcome " + name + "!");
      } else {
        Serial.println("[AUTH FAIL] Unauthorized user.");
        //lockSystem();
      }
      return;
    }

    if (input.equalsIgnoreCase("status")) {
      printStatus();
      return;
    }

    // Only allow control if authenticated
    if (!isAuthenticated) {
      Serial.println("[ERROR] Please authenticate first (user:<name>).");
      return;
    }

    // Voice command simulation
    if (input.equalsIgnoreCase("light_on")) controlAppliance("light", true);
    else if (input.equalsIgnoreCase("light_off")) controlAppliance("light", false);
    else if (input.equalsIgnoreCase("fan_on")) controlAppliance("fan", true);
    else if (input.equalsIgnoreCase("fan_off")) controlAppliance("fan", false);
    else Serial.println("[ERROR] Unknown command: " + input);
  }
}

// === Appliance control logic ===
void controlAppliance(String device, bool turnOn) {
  int ldrValue = analogRead(LDR_PIN);
  int pirValue = digitalRead(PIR_PIN);
  int potValue = analogRead(POT_PIN);

  if (device == "light") {
    // Light ON only if dark AND motion detected
    if (turnOn) {
      if (ldrValue > LDR_DARK_THRESHOLD) {
        Serial.println("[CTX] Light not needed (room is bright).");
        return;
      }
      if (pirValue == LOW) {
        Serial.println("[CTX] No motion detected. Light blocked.");
        return;
      }
    }
    digitalWrite(LED_LIGHT, turnOn ? HIGH : LOW);
    Serial.println("[ACTION] Light " + String(turnOn ? "ON" : "OFF"));
  }

  else if (device == "fan") {
    // Fan ON only if motion detected AND load is safe
    if (turnOn) {
      if (potValue > LOAD_HIGH_THRESHOLD) {
        Serial.println("[CTX] High load detected. Fan blocked.");
        return;
      }
      if (pirValue == LOW) {
        Serial.println("[CTX] No motion detected. Fan blocked.");
        return;
      }
    }
    digitalWrite(LED_FAN, turnOn ? HIGH : LOW);
    Serial.println("[ACTION] Fan " + String(turnOn ? "ON" : "OFF"));
  }
}


// === Show sensor readings ===
void printStatus() {
  int ldrValue = analogRead(LDR_PIN);
  int potValue = analogRead(POT_PIN);
  int pirValue = digitalRead(PIR_PIN);

  Serial.println("----- STATUS -----");
  Serial.println("LDR: " + String(ldrValue));
  Serial.println("PIR: " + String(pirValue));
  Serial.println("Pot (Load): " + String(potValue));
  Serial.print("Light: "); Serial.println(digitalRead(LED_LIGHT));
  Serial.print("Fan: "); Serial.println(digitalRead(LED_FAN));
  Serial.println("------------------");
}
