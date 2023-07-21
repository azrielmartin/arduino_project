#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h> //6.10.1
#include <NTPClient.h>
#include <WiFiUdp.h>

const int timeZoneOffset = 8;

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
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", timeZoneOffset);

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  timeClient.begin();

  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(relay3Pin, OUTPUT);
  pinMode(relay4Pin, OUTPUT);
}

void loop() {
  timeClient.update();
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

  while (client.connected() && !client.available()){
    delay(10);
  }

  // Read the response
  String jsonPayload;
  while (client.available()) {
    char c = client.read();
    jsonPayload += c;
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
    char c = client.read();
    jsonPayload += c;
  }

  // Print the received JSON response
  Serial.println("Received JSON response:");
  Serial.println(jsonPayload);

  // Parse the JSON response
  StaticJsonDocument<256> jsonDocument;
  DeserializationError error = deserializeJson(jsonDocument, jsonPayload);

  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
  } 
  
  
  else if (!jsonPayload.isEmpty()) {
    Serial.println("Channel Status:");

    int currentHour = timeClient.getHours();
    int currentMinute = timeClient.getMinutes();

    JsonArray statusArray = jsonDocument[jsonPayload].as<JsonArray>();
    // JsonArray dataArray = jsonDocument.createNestedArray("data");
    // Serial.println(jsonDocument);

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

    // size_t jsonLength = measureJson(jsonDocument);


    for (int i = 0; i < statusArray.size(); i++) {
      JsonObject status = statusArray[i];

      // Access individual elements of each "status" object
      const char* name = status["name"].as<const char*>();
      bool statusValue = status["status"];

      JsonObject timeoutObject = status["timeout"]; // Access the "timeout" object within the status object
      const char* date = timeoutObject["date"].as<const char*>();
      const char* time = timeoutObject["time"].as<const char*>();

      if(i == 0){
        // put values to 1
        name1 = name;
        statusValue1 = statusValue;
        date1 = date;
        time1 = time;
      }
      else  if(i == 1){
        // put values to 2
        name2 = name;
        statusValue2 = statusValue;
        date2 = date;
        time2 = time;
      }
      else  if(i == 2){
        // put values to 3
        name3 = name;
        statusValue3 = statusValue;
        date3 = date;
        time3 = time;
      }
      else  if(i == 3){
        name4 = name;
        statusValue4 = statusValue;
        date4 = date;
        time4 = time;
      } 
      else {
        Serial.print("Error");
        client.stop();
      }

      // Print the channel status data
      Serial.print("Name: ");
      Serial.println(name);
      Serial.print("Status: ");
      Serial.println(statusValue);
      Serial.print("Date: ");
      Serial.println(date);
      Serial.print("Time: ");
      Serial.println(time);
      Serial.println();
    }

      //Channel 1
      if (statusValue1 == true) {
        relay1State = true;
        if(currentHour > atoi(time1) || (currentHour == atoi(time1) && currentMinute >= atoi(time1 + 3))){
          relay1State = false;
          digitalWrite(relay1Pin, LOW);
        }
        digitalWrite(relay1Pin, HIGH); //transfer to pins as signals
      } 
      else if (statusValue1 == false) {
        relay1State = false;
      }
      

      //Switch logic
      //Channel 2
      if (statusValue2 == true) {
        relay2State = true;
        if(currentHour > atoi(time2) || (currentHour == atoi(time2) && currentMinute >= atoi(time2 + 3))){
          relay2State = false;
          digitalWrite(relay2Pin, LOW);
        }
        digitalWrite(relay2Pin, HIGH); //transfer to pins as signals
      } 
      else if (statusValue2 == false) {
        relay2State = false;
      }

      //Channel 3
      if (statusValue3 == true) {
        relay3State = true;
        if(currentHour > atoi(time3) || (currentHour == atoi(time3) && currentMinute >= atoi(time3 + 3))){
          relay3State = false;
          digitalWrite(relay3Pin, LOW);
        }
        digitalWrite(relay3Pin, HIGH); //transfer to pins as signals
      } 
      else if (statusValue3 == false) {
        relay3State = false;
      }
      
      //Channel 4
      if (statusValue4 == true) {
        relay4State = true;
        if(currentHour > atoi(time4) || (currentHour == atoi(time4) && currentMinute >= atoi(time4 + 3))){
          relay4State = false;
          digitalWrite(relay4Pin, LOW);
        }
        digitalWrite(relay4Pin, HIGH); //transfer to pins as signals
      } 
      else if (statusValue4 == false) {
        relay4State = false;
      }
  }

  Serial.println("Request complete.");
  client.stop();
}