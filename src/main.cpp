#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"

// DHT Sensor configuration
#define DHTPIN 4        // ESP32 GPIO pin connected to DHT11
#define DHTTYPE DHT11   // DHT sensor type (DHT11 or DHT22)


DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi credentials
#define WIFI_SSID "SuvasisPhone"      
#define WIFI_PASSWORD "Subhasish"      

// Firebase settings
#define FIREBASE_HOST "https://suvasis-suvasis-default-rtdb.firebaseio.com"  
#define FIREBASE_AUTH "94rW0ck5O4zlve9PK7TJUAzKrAXo9RqiDMHQkptK"             

#define LED_PIN 2

// Firebase Objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
//int sensorValue = 5;


void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  //Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    digitalWrite(LED_PIN, LOW); //off led if wifi connected
  }
  //Serial.println("\nConnected to Wi-Fi");
  digitalWrite(LED_PIN, HIGH); //on led if wifi connected

  // Set Firebase Config
  config.host = FIREBASE_HOST;                          // Set Firebase URL
  config.signer.tokens.legacy_token = FIREBASE_AUTH;    // Used the legacy token for auth

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);                         // Auto-reconnect on Wi-Fi failure
  Serial.println("Connected to Firebase");
}

void loop() {

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  // Get the time in milliseconds since boot
  unsigned long timestamp = millis() / 1000;  // Convert to seconds
  Serial.print("Sending data to Firebase: ");
   // Create a JSON object to store multiple values
  FirebaseJson json;

  json.set("temperature", temperature);
  json.set("humidity", humidity);
  json.set("timestamp", timestamp);

  Serial.println("Sending data to Firebase...");

  // Send data to Firebase
  if (Firebase.pushJSON(firebaseData, "/sensor/data", json)) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Firebase Error: ");
    Serial.println(firebaseData.errorReason());
  }
  delay(5000); // Wait for 5 seconds
}

