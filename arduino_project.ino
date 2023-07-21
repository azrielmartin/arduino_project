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
  } 
  
  
  else if (!jsonPayload.isEmpty()) {
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

    for (int i = 0; jsonDocument.length <4, i ++) {
      // Access individual elements of each "status" object
      const char* name = status["name"];
      bool statusValue = status["status"];
      const char* date = status["timeout"]["date"];
      const char* time = status["timeout"]["time"];


      if(i == 0){
      // put values to 1
      }else  if(i == 1){
      // put values to 2
      }else  if(i == 2){
      // put values to 3
      }else  if(i == 3){
      // put values to 4
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

      //Channel 1
      if (statusValue1 == true) {
        relay1State = true;
          // if(time is greater than time1){
          // relay1State = false;
          // }
        digitalWrite(relay1Pin, HIGH);
      } 
      else if (statusValue1 == false) {
        relay1State = false;
      }
      
      //Channel 2
      if (statusValue2 == true) {
        relay2State = true;
          // if(time is greater than time1){
          // relay2State = false;
          // }
        digitalWrite(relay2Pin, HIGH);
      } 
      else if (statusValue2 == false) {
        relay2State = false;
      }

      //Channel 3
      if (statusValue3 == true) {
        relay3State = true;
          // if(time is greater than time1){
          // relay3State = false;
          // }
        digitalWrite(relay3Pin, HIGH);
      } 
      else if (statusValue3 == false) {
        relay3State = false;
      }
      
      //Channel 4
      if (statusValue4 == true) {
        relay4State = true;
          // if(time is greater than time1){
          // relay3State = false;
          // }
        digitalWrite(relay4Pin, HIGH);
      } 
      else if (statusValue 1 == false) {
        relay1State = false;
      }

  }

  Serial.println("Request complete.");
  client.stop();
}

