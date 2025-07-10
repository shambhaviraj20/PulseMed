#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi credentials
#define WIFI_SSID "EZ"
#define WIFI_PASSWORD "qwertyuiop"

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
int previousSignal = 0;

// Touch Sensor for Session Management
const int touchSensorPin = D6;
bool sessionActive = false;
unsigned long sessionStartTime = 0;
bool dataCollectionComplete = false;

// Data arrays to store 20 values
const int numValues = 20;
int bpmValues[numValues] = {0};
float tempValues[numValues] = {0.0};
int valueIndex = 0;

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
        if (!sessionActive && !dataCollectionComplete) {
            sessionActive = true;
            Serial.println("Session started...");
            sessionStartTime = millis();
            valueIndex = 0;
            dataCollectionComplete = false;
        } else {
            sessionActive = false;
            Serial.println("Session stopped...");
        }
        delay(500); // Debounce delay
    }

    if (sessionActive && !dataCollectionComplete) {
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
                if (BPM > 50 && BPM < 200) {
                    lastBPM = BPM;
                    Serial.print("Current BPM: ");
                    Serial.println(BPM);
                }
            }
            pulseDetected = true;
        }

        if (smoothedSignal < Threshold) {
            pulseDetected = false;
        }

        previousSignal = smoothedSignal;

        // Store data every second
        static unsigned long lastStoreTime = 0;
        if (millis() - lastStoreTime >= 1000) {
            lastStoreTime = millis();

            // Read temperature
            sensors.requestTemperatures();
            float temperatureC = sensors.getTempCByIndex(0);

            // Store values if we have valid readings
            if (valueIndex < numValues && lastBPM > 0) {
                bpmValues[valueIndex] = lastBPM;
                tempValues[valueIndex] = temperatureC;
                Serial.print("BPM: ");
                Serial.print(lastBPM);
                Serial.print(", Temperature: ");
                Serial.println(temperatureC);
                valueIndex++;

                // Check if we've collected all values
                if (valueIndex >= numValues) {
                    dataCollectionComplete = true;
                    sessionActive = false;
                    Serial.println("Data collection complete!");
                    uploadDataToSerial();
                }
            }
        }

        delay(10);
    }
}

void uploadDataToSerial() {
    Serial.println("START_DATA");
    for (int i = 0; i < numValues; i++) {
        Serial.print("BPM: ");
        Serial.print(bpmValues[i]);
        Serial.print(", Temperature: ");
        Serial.println(tempValues[i]);
    }
    Serial.println("END_DATA");
}
