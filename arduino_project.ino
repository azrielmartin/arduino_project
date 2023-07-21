#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> //6.10.1
 
const char* ssid = "Azriel";
const char* password = "theresa72";
const char* host = "ojt-relay-switch-api.vercel.app";
const char* deviceID = "7198ec46-0d2e-49a9-a9d5-121058cfc425";
const int httpsPort = 443;
const int relay1Pin = 5;
const int relay2Pin = 4;
const int relay3Pin = 0;
const int relay4Pin = 2;
// char* data_channel[3];

bool relay1State = false;
bool relay2State = false;
bool relay3State = false;
bool relay4State = false;

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);
}

void loop() {
  client.setInsecure();

  Serial.print("Connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed.");
    return;
  }

  getAmIActive();
  delay(5000);
}

void getAmIActive() {
  client.println("GET /api/devices/check-activation?deviceId=7198ec46-0d2e-49a9-a9d5-121058cfc425 HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  Serial.println("Request sent.");
  while (client.connected()){
    String line = client.readStringUntil('\n');
    Serial.println(line);

    if (line == "\r") {
      break;
    }
  }

  String jsonPayload;
  while (client.available()) {
    jsonPayload = client.readStringUntil('\n');
    Serial.println(jsonPayload);
  }

  StaticJsonDocument<256> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, jsonPayload);

  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
  } else {
    char* data;
    

    if (jsonDocument.containsKey("error")) {
      data = strdup(jsonDocument["error"].as<char*>());
    } else if (jsonDocument.containsKey("message")) {
      data = strdup(jsonDocument["message"].as<char*>());
    } else if (jsonDocument.containsKey("activated")) {
      data = strdup(jsonDocument["activated"].as<char*>());
      if (strcmp(data, "true") == 0) {
        // If active, get channel status
        getMyChannelStatus();
      }
    } else {
      data = nullptr;
    }

    if (data != nullptr) {
      Serial.print("Data: ");
      Serial.println(data);
      free(data);
    }
  }

  Serial.println("Request complete.");
  client.stop();
}

void getMyChannelStatus() {
  client.println("GET /api/devices/get-channel-status?deviceId=7198ec46-0d2e-49a9-a9d5-121058cfc425 HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  Serial.println("Request sent.");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);

    if (line == "\r") {
      break;
    }
  }

  String jsonPayload;
  while (client.available()) {
    jsonPayload = client.readStringUntil('\n');
    Serial.println(jsonPayload);
  }

  StaticJsonDocument<256> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, jsonPayload);

  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
  } else if (!jsonPayload.isEmpty()) {
    Serial.println("Channel Status:");

    JsonArray statusArray = jsonDocument["status"].as<JsonArray>();

    for (int i = 0; i < statusArray.size(); i++) {
      JsonObject status = statusArray[i];
      const char* name = status["name"];
      bool statusValue = status["status"];
      const char* date = status["timeout"]["date"];
      const char* time = status["timeout"]["time"];

      // Print the channel status data
      Serial.print("Name: ");
      Serial.println(name);
      Serial.print("Status: ");
      Serial.println(statusValue);
      Serial.print("Date: ");
      Serial.println(date ? date : "null");
      Serial.print("Time: ");
      Serial.println(time ? time : "null");
      Serial.println();

      // Update relay states based on channel status
      if (i == 0) {
        relay1State = statusValue;
        digitalWrite(relay1Pin, statusValue ? HIGH : LOW);
      } else if (i == 1) {
        relay2State = statusValue;
        digitalWrite(relay2Pin, statusValue ? HIGH : LOW);
      } else if (i == 2) {
        relay3State = statusValue;
        digitalWrite(relay3Pin, statusValue ? HIGH : LOW);
      } else if (i == 3) {
        relay4State = statusValue;
        digitalWrite(relay4Pin, statusValue ? HIGH : LOW);
      } else {
        // Handle an unexpected number of channels (if needed)
      }
    }

    // Print the JSON document in a pretty format
    String jsonPretty;
    serializeJsonPretty(jsonDocument, jsonPretty);
    Serial.println("JSON Document:");
    Serial.println(jsonPretty);
  }

  Serial.println("Request complete.");
  client.stop();
}