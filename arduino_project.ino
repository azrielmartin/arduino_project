#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> //6.10.1
 
const char* ssid = "Azriel";
const char* password = "theresa72";
const char* host = "ojt-relay-switch-api.vercel.app";
const char* deviceID = "7198ec46-0d2e-49a9-a9d5-121058cfc425";
const int httpsPort = 443;
const int relay1Pin = D1;
const int relay2Pin = D2;
const int relay3Pin = D3;
const int relay4Pin = D4;
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
    Serial.println(jsonDocument);

    const char* name1;
    bool statusValue1;
    const char* date1;
    const char* time1;

    const char* name2;
    bool statusValue2;
    const char* date2;
    const char* time2;

    const char* name3;
    bool statusValue3;
    const char* date3;
    const char* time3;

    const char* name4;
    bool statusValue4;
    const char* date4;
    const char* time4;

    size_t jsonLength = measureJson(jsonDocument);

    for (int i = 0; i < 4; i++) {

      JsonObject status = statusArray[i];
      const char* name = status["name"];
      bool statusValue = status["status"];
      const char* date = status["timeout"]["date"];
      const char* time = status["timeout"]["time"];

      if (i == 0) {
        name1 = name;
        statusValue1 = statusValue;
        date1 = date;
        time1 = time;
      } else if (i == 1) {
        name2 = name;
        statusValue2 = statusValue;
        date2 = date;
        time2 = time;
      } else if (i == 2) {
        name3 = name;
        statusValue3 = statusValue;
        date3 = date;
        time3 = time;
      } else if (i == 3) {
        name4 = name;
        statusValue4 = statusValue;
        date4 = date;
        time4 = time;
      } else {
        // error
      }

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
    }

    // Update relay states based on channel status
    digitalWrite(relay1Pin, statusValue1);
    digitalWrite(relay2Pin, statusValue2);
    digitalWrite(relay3Pin, statusValue3);
    digitalWrite(relay4Pin, statusValue4);

    relay1State = statusValue1;
    relay2State = statusValue2;
    relay3State = statusValue3;
    relay4State = statusValue4;
  }

  Serial.println("Request complete.");
  client.stop();
}
