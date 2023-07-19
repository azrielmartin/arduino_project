#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>  // Library for parsing JSON data
//j
const char* ssid = "Azriel";
const char* password = "theresa72";
const char* host = "ojt-relay-switch-api.vercel.app";
const int httpsPort = 443;

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}
WiFiClientSecure client;

void loop() {

  client.setInsecure();  // Disable certificate verification (INSECURE)

  Serial.print("Connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed.");
    return;
  }



  //if active is false loop infinite 2 other functions
  getAmIActive();

  //if active is false loop infinite 2 other functions
  //getCurrentTime. response = date and time

  //if active is true,
  //getMyChannelStatus
  //inside here create a check if channel_time is greater than current time
  //example channel 1 time is 13:45, then if current_time is 13:46 turn off that channel channel_time < current_time else off channel



  delay(5000);  // Wait for 5 seconds before making another request
}


void getAmIActive() {
  // Make the HTTPS request
  client.println("GET /api/devices/check-activation?deviceId=7198ec46-0d2e-49a9-a9d5-121058cfc425 HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  Serial.println("Request sent.");

  // Wait for the response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);

    if (line == "\r") {
      // Empty line indicates the end of the headers, the JSON data will follow
      break;
    }
  }

  // Process JSON response
  String jsonPayload;
  while (client.available()) {
    jsonPayload = client.readStringUntil('\n');
    Serial.println(jsonPayload);
  }

  // Parse JSON data
  StaticJsonDocument<256> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, jsonPayload);

  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
  } else {
    // Access the "message" field from the JSON data
    char* data;


    if (jsonDocument.containsKey("error")) {
      data = strdup(jsonDocument["error"].as<char*>());
    } else if (jsonDocument.containsKey("message")) {
      data = strdup(jsonDocument["message"].as<char*>());
    } else if (jsonDocument.containsKey("activated")) {
      data = strdup(jsonDocument["activated"].as<char*>());
    } else {
      data = nullptr;  // Set data to nullptr if none of the keys are present
    }

    if (data != nullptr) {
      Serial.print("Data: ");
      Serial.println(data);
      // STORE IT IN BOOL activated = data// false/true
      free(data);  // Free the dynamically allocated memory
    }
  }

  Serial.println("Request complete.");
  client.stop();
}