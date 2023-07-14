#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <map>
#include <Crypto.h>
#include <Hash.h>
#include <SHA256.h>

#define WIFI_SSID  "Azriel"   
#define WIFI_PASS  "theresa72"

// Enter the device IDs here: can be generated then make a qr code about this one.
#define device_ID_1   "SWITCH_ID_NO_1_HERE"
#define device_ID_2   "SWITCH_ID_NO_2_HERE"
#define device_ID_3   "SWITCH_ID_NO_3_HERE"
#define device_ID_4   "SWITCH_ID_NO_4_HERE"

// Define the GPIO connected with Relays and switches
#define RelayPin1 5  // D1
#define RelayPin2 4  // D2
#define RelayPin3 14 // D5
#define RelayPin4 12 // D6

#define SwitchPin1 10  // SD3
#define SwitchPin2 0   // D3 
#define SwitchPin3 13  // D7
#define SwitchPin4 3   // RX

#define wifiLed   16   // D0

// Comment the following line if you use toggle switches instead of tactile buttons
//#define TACTILE_BUTTON 1

#define BAUD_RATE   9600

#define DEBOUNCE_TIME 250

typedef struct {      // Struct for the std::map below
  int relayPIN;
  int flipSwitchPIN;
} deviceConfig_t;

// This is the main configuration
// Please put in your deviceId, the PIN for Relay and PIN for flipSwitch
// This can be up to N devices...depending on how much pin's available on your device ;)
// Right now we have 4 devicesIds going to 4 relays and 4 flip switches to switch the relay manually
std::map<String, deviceConfig_t> devices = {
    //{deviceId, {relayPIN,  flipSwitchPIN}}
    {device_ID_1, {  RelayPin1, SwitchPin1 }},
    {device_ID_2, {  RelayPin2, SwitchPin2 }},
    {device_ID_3, {  RelayPin3, SwitchPin3 }},
    {device_ID_4, {  RelayPin4, SwitchPin4 }}     
};

typedef struct {      // Struct for the std::map below
  String deviceId;
  bool lastFlipSwitchState;
  unsigned long lastFlipSwitchChange;
} flipSwitchConfig_t;

std::map<int, flipSwitchConfig_t> flipSwitches;    // This map is used to map flipSwitch PINs to deviceId and handle debounce and last flipSwitch state checks
                                                  // It will be set up in the "setupFlipSwitches" function, using information from the devices map

void setupRelays() { 
  for (auto &device : devices) {           // For each device (relay, flipSwitch combination)
    int relayPIN = device.second.relayPIN; // Get the relay pin
    pinMode(relayPIN, OUTPUT);             // Set relay pin to OUTPUT
    digitalWrite(relayPIN, HIGH);
  }
}

void setupFlipSwitches() {
  for (auto &device : devices)  {                     // For each device (relay / flipSwitch combination)
    flipSwitchConfig_t flipSwitchConfig;              // Create a new flipSwitch configuration

    flipSwitchConfig.deviceId = device.first;         // Set the deviceId
    flipSwitchConfig.lastFlipSwitchChange = 0;        // Set debounce time
    flipSwitchConfig.lastFlipSwitchState = true;      // Set lastFlipSwitchState to false (LOW)

    int flipSwitchPIN = device.second.flipSwitchPIN;  // Get the flipSwitchPIN

    flipSwitches[flipSwitchPIN] = flipSwitchConfig;   // Save the flipSwitch config to flipSwitches map
    pinMode(flipSwitchPIN, INPUT_PULLUP);                   // Set the flipSwitch pin to INPUT
  }
}

bool onPowerState(String deviceId, bool &state)
{
  Serial.printf("%s: %s\r\n", deviceId.c_str(), state ? "on" : "off");
  int relayPIN = devices[deviceId].relayPIN; // Get the relay pin for the corresponding device
  digitalWrite(relayPIN, !state);             // Set the new relay state
  return true;
}

String generateHMACSignature(String data, String secret) {
  SHA256 sha256;
  uint8_t hmacResult[32];
  char hmacSignature[65];

  sha256.reset();
  sha256.update((uint8_t*)secret.c_str(), secret.length());
  sha256.update((uint8_t*)data.c_str(), data.length());
  sha256.finalize(hmacResult);

  for (int i = 0; i < sizeof(hmacResult); ++i) {
    sprintf(&hmacSignature[i * 2], "%02x", hmacResult[i]);
  }

  return String(hmacSignature);
}

void setupWiFi()
{
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf(".");
    delay(250);
  }
  digitalWrite(wifiLed, LOW);
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

void setup()
{
  Serial.begin(BAUD_RATE);

  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, HIGH);

  setupRelays();
  setupFlipSwitches();
  setupWiFi();

  // Generate required values
  unsigned long currentTimestamp = millis();
  String url = "YOUR_REQUEST_URL"; // Replace with the actual value
  String dataA = String(currentTimestamp) + "/" + url;
  String apiSecret = "f2c24d11-a83e-dd9b-579c-9be7228a1a26"; // Replace with the actual value
  String hmacSignature = generateHMACSignature(dataA, apiSecret);

  // Print dataA and hmacSignature
  Serial.println(dataA);
  Serial.println(hmacSignature);

  // Set request headers
  Serial.print("X-API-Key: ");
  Serial.println("14444878-632a-07be-67c5-703e3f566392");
  Serial.print("X-HMAC-Signature: ");
  Serial.println(hmacSignature);
  Serial.print("X-Timestamp: ");
  Serial.println(currentTimestamp);
}

void loop()
{
  handleFlipSwitches();
}
