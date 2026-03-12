// ===== SIGN LANGUAGE GLOVE — MULTI-FINGER + BLYNK NOTIFICATIONS =====
// Kartika Porwal - ESP32 with flex sensors and Blynk push alerts

#define BLYNK_TEMPLATE_ID "TMPL3QIo17PYc"
#define BLYNK_TEMPLATE_NAME "SignTalk"
#define BLYNK_AUTH_TOKEN "ymvLyYQGyhIldEUKcsSlBa8MnQfIhNE2"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = "ymvLyYQGyhIldEUKcsSlBa8MnQfIhNE2";
char ssid[] = "WaterBubble";
char pass[] = "ANUSHA69";

const int flexPins[5] = {32, 33, 34, 35, 39};   // Thumb → Pinky
const int numFlex = 5;

// --- Calibrated sensor values (use your measured ranges) ---
int flatVal[numFlex] = {845, 185, 559, 525, 960};
int bentVal[numFlex] = {470, 175, 495, 430, 910};

// --- Thresholds ---
float threshold[numFlex];
bool increasesOnBend[numFlex] = {false, false, false, false, false};

// --- Blynk Event IDs & Messages ---
const char* eventIDs[numFlex] = {"hungry", "food", "water", "washroom", "help"};
const char* messages[numFlex] = {
  "🍽 I am hungry",                  
  "🍴 I need food",                  
  "💧 I need water",                 
  "🚻 I need to go to the washroom", 
  "🖐 I need help"                  
};

bool messageSent[numFlex] = {false, false, false, false, false};

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n✅ SignTalk Glove — multi-finger + Blynk active");

  // Setup Blynk WiFi
  Blynk.begin(auth, ssid, pass);
  Serial.println("Connecting to Blynk...");

  // Compute thresholds for each finger
  for (int i = 0; i < numFlex; i++) {
    threshold[i] = (flatVal[i] + bentVal[i]) / 2.0;
  }
  threshold[1] += 10; // tweak index finger threshold
}

void loop() {
  Blynk.run();

  int flexVals[numFlex];
  bool anyDetected = false;

  // --- Read all sensors ---
  for (int i = 0; i < numFlex; i++) {
    flexVals[i] = analogRead(flexPins[i]);
  }

  // --- Detect gestures per finger ---
  for (int i = 0; i < numFlex; i++) {
    bool isBent = (flexVals[i] < threshold[i]);

    if (isBent && !messageSent[i]) {
      Serial.println(messages[i]);
      Blynk.logEvent(eventIDs[i], messages[i]);  // Send notification to Blynk
      messageSent[i] = true;
      anyDetected = true;
    } else if (!isBent) {
      messageSent[i] = false; // reset trigger when finger unbends
    }
  }

  // --- If no gesture detected ---
  if (!anyDetected) {
    Serial.println("MSG,No gesture detected");
  }

  // --- Print raw sensor data ---
  Serial.print("Raw: ");
  for (int i = 0; i < numFlex; i++) {
    Serial.print(flexVals[i]);
    if (i < numFlex - 1) Serial.print(",");
  }
  Serial.println();

  delay(3000); // stable 3-sec update
}