#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Crypto.h>
#include <SHA256.h>
#include <Base64.h>

//-----WIFI SETTINGS-----
const char* ssid = "Azriel";
const char* password = "theresa72";
const char* apiUrl = "https://ojt-relay-switch-api.vercel.app";
const char* apiKey = "14444878-632a-07be-67c5-703e3f566392";
const String apiSecret = "f2c24d11-a83e-dd9b-579c-9be7228a1a26";
String url = ""; // Replace this with the actual URL
String dataA = "";

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Generate the dataA string
  url = ""; // Replace this with the actual URL
  dataA = String(millis()) + "/" + url;

  // Generate the HMAC signature with SHA256 algorithm
  byte hmacSignature[SHA256_BLOCK_SIZE];
  HMACSHA256 hmac((const byte*)apiSecret.c_str(), apiSecret.length());
  hmac.doUpdate((const byte*)dataA.c_str(), dataA.length());
  hmac.doFinal(hmacSignature);

  // Convert the HMAC signature to a hexadecimal string
  String hmacSignatureHex = "";
  char hex[3];
  for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
    sprintf(hex, "%02x", hmacSignature[i]);
    hmacSignatureHex += hex;
  }

  Serial.println(dataA);
  Serial.println(hmacSignatureHex);

  // Make API request
  HTTPClient http;
  http.begin(apiUrl);
  http.addHeader("X-API-Key", apiKey);
  http.addHeader("X-HMAC-Signature", hmacSignatureHex);
  http.addHeader("X-Timestamp", String(millis()));
  int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println("API response:");
    Serial.println(response);

    // Check if the response indicates successful access
    if (response == "Access Granted") {
      Serial.println("Access granted. Welcome to the application!");
      // Perform the desired actions when access is granted
    } else {
      Serial.println("Access denied. Please check your API key.");
      // Perform any necessary actions when access is denied
    }
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop() {
  // Your code here
}
