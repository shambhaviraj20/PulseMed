#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
#define WIFI_SSID "EZ"
#define WIFI_PASSWORD "qwertyuiop"

// Firebase credentials
#define FIREBASE_HOST "smart-box-4da35-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "O8cQE2x77lwTNpunPMNt5GImDQbe2Eq433t0v17u"

// DS18B20 Temperature Sensor
#define ONE_WIRE_BUS D5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Pulse Sensor
#define PULSE_PIN A0
int Signal;
int Threshold = 512;
int BPM;
int lastBPM = 0;
unsigned long lastBeatTime = 0;
unsigned long sampleCounter = 0;
bool pulseDetected = false;

// Moving Average Filter for Heart Rate
const int avgSize = 10;
int readings[avgSize] = {0};
int readIndex = 0;
long total = 0;
int smoothedSignal = 0;
int previousSignal = 0; // Declaration of previousSignal

// Touch Sensor for Session Management
const int touchSensorPin = D6;
bool sessionActive = false;
unsigned long sessionStartTime = 0;

// Data arrays to store 20 values
const int numValues = 20;
int bpmValues[numValues] = {0};
float tempValues[numValues] = {0.0};
int valueIndex = 0;

// Firebase configuration
FirebaseConfig config;
FirebaseAuth auth;

// Firebase Data object
FirebaseData firebaseData;

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Initialize WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to Wi-Fi");

    // Firebase configuration
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Initialize sensors
    sensors.begin();
    pinMode(PULSE_PIN, INPUT);
    pinMode(touchSensorPin, INPUT);

    // Initialize moving average filter
    for (int i = 0; i < avgSize; i++) {
        readings[i] = 0;
    }
}

void loop() {
    // Toggle session based on touch sensor input
    if (digitalRead(touchSensorPin) == HIGH) {
        sessionActive = !sessionActive;
        delay(500); // Debounce delay
        if (sessionActive) {
            Serial.println("Session started...");
            sessionStartTime = millis();
            valueIndex = 0;
        } else {
            Serial.println("Session stopped...");
            uploadDataToFirebase();  // Upload stored data to Firebase when session stops
        }
    }

    if (sessionActive) {
        // Read pulse sensor and apply moving average filter
        Signal = analogRead(PULSE_PIN);
        total -= readings[readIndex];
        readings[readIndex] = Signal;
        total += readings[readIndex];
        readIndex = (readIndex + 1) % avgSize;
        smoothedSignal = total / avgSize;

        sampleCounter = millis();

        // Detect heartbeat
        if (smoothedSignal > Threshold && previousSignal <= Threshold) {
            if (sampleCounter - lastBeatTime > 300) { // Debounce noise
                unsigned long timeBetweenBeats = sampleCounter - lastBeatTime;
                lastBeatTime = sampleCounter;
                BPM = 60000 / timeBetweenBeats;

                // Adaptive Threshold Adjustment
                Threshold = (Threshold * 3 + smoothedSignal) / 4;

                // Noise Filtering (Valid BPM range: 50-160)
                if (BPM > 50 && BPM < 160) {
                    lastBPM = BPM;
                    Serial.print("Heartbeat: ");
                    Serial.print(BPM);
                    Serial.println(" BPM");
                }
            }
            pulseDetected = true;
        }

        if (smoothedSignal < Threshold) {
            pulseDetected = false;
        }

        previousSignal = smoothedSignal;

        // Store data every second
        if (millis() - lastBeatTime > 1000) {
            lastBeatTime = millis(); // Update last beat time to manage 1-second interval

            // Read temperature from DS18B20
            sensors.requestTemperatures();
            float temperatureC = sensors.getTempCByIndex(0);

            // Store values
            if (valueIndex < numValues) {
                bpmValues[valueIndex] = lastBPM;
                tempValues[valueIndex] = temperatureC;
                valueIndex++;
                Serial.print("Stored BPM: ");
                Serial.print(lastBPM);
                Serial.print(" / Stored Temperature: ");
                Serial.println(temperatureC);
            }

            // If 20 values are collected, upload them
            if (valueIndex >= numValues) {
                uploadDataToFirebase();
                sessionActive = false;
            }
        }

        delay(10);
    }
}

void uploadDataToFirebase() {
    for (int i = 0; i < numValues; i++) {
        String pathBPM = "/patient/heartRate/" + String(i);
        String pathTemp = "/patient/temperature/" + String(i);
        Firebase.setFloat(firebaseData, pathBPM, bpmValues[i]);
        Firebase.setFloat(firebaseData, pathTemp, tempValues[i]);
        delay(50);  // Slight delay to ensure data is sent properly
    }
    Serial.println("Uploaded 20 values to Firebase.");
}
