#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "Azriel";
const char* password = "theresa72";
const char* host = "ojt-relay-switch-api.vercel.app";
const int httpsPort = 443;
const int relay1Pin = D1;
const int relay2Pin = D2;
const int relay3Pin = D3;
const int relay4Pin = D4;

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

  while (!client.available()) {
    delay(1);
  }

  getAmIActive();
  
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/relay1on") != -1) {
    relay1State = true;
    digitalWrite(relay1Pin, HIGH);
  } else if (request.indexOf("/relay1off") != -1) {
    relay1State = false;
    digitalWrite(relay1Pin, LOW);
  } else if (request.indexOf("/relay2on") != -1) {
    relay2State = true;
    digitalWrite(relay2Pin, HIGH);
  } else if (request.indexOf("/relay2off") != -1) {
    relay2State = false;
    digitalWrite(relay2Pin, LOW);
  } else if (request.indexOf("/relay3on") != -1) {
    relay3State = true;
    digitalWrite(relay3Pin, HIGH);
  } else if (request.indexOf("/relay3off") != -1) {
    relay3State = false;
    digitalWrite(relay3Pin, LOW);
  } else if (request.indexOf("/relay4on") != -1) {
    relay4State = true;
    digitalWrite(relay4Pin, HIGH);
  } else if (request.indexOf("/relay4off") != -1) {
    relay4State = false;
    digitalWrite(relay4Pin, LOW);
  }

  String response = "HTTP/1.1 200 OK\r\n\r\n";
  response += "Relay 1: " + String(relay1State) + "<br>";
  response += "Relay 2: " + String(relay2State) + "<br>";
  response += "Relay 3: " + String(relay3State) + "<br>";
  response += "Relay 4: " + String(relay4State) + "<br>";

  client.print(response);
  delay(1);
  Serial.println("Client disconnected");

  client.stop();

  delay(5000);
}

void getAmIActive() {
  client.println("GET /api/devices/check-activation?deviceId=7198ec46-0d2e-49a9-a9d5-121058cfc425 HTTP/1.1");
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
  } else {
    // Process channel status JSON data here
    // Implement your logic based on the response
    // Example: Check if channel_time is greater than current time and turn off the channel if required
    // Remember to update the corresponding relay state variables and control the relays accordingly
  }

  Serial.println("Request complete.");
  client.stop();
}
