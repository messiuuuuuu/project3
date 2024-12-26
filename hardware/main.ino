#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"


// Insert your network credentials
#define WIFI_SSID "TDuong"
#define WIFI_PASSWORD "lmaoezzz"
#define sensor_pin 35   /* Soil moisture sensor O/P pin */

// Insert Firebase project API Key
#define API_KEY "AIzaSyANJkWsIhBPVADfgwCsNNHod3G3rSDxeS4"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://plant-watering-a9d08-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

int humidity, value;

void setup() {
  // Set up pins

  Serial.begin(9600);  // Help test code
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// Handheld game console actives
void loop() {
  value = analogRead(sensor_pin);
  humidity = (100 - ((value / 4095.00) * 100));
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    //since we want the data to be updated every second
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.setFloat(&fbdo, "Plant_Watering/Devices/keyId1/Humidity", humidity)){
      Serial.print("Humidity : ");
      Serial.print(humidity);
    }
    else {
      Serial.println("Failed to Read from the Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
}
  }
}
