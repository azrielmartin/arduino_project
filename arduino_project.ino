#include <Arduino.h>
#include <ArduinoCrypto.h>
//Azi Gwapo

//CLIENT SIDE HMAC Generation method
unsigned long currentTimestamp = millis();
const char* apiKey = "14444878-632a-07be-67c5-703e3f566392";
const char* apiSecret = "f2c24d11-a83e-dd9b-579c-9be7228a1a26";
String url = "YOUR_URL"; // Replace "YOUR_URL" with the desired URL

String dataA = String(currentTimestamp) + "/" + url;

void setup() {
  Serial.begin(9600);

  // Generate the HMAC signature with SHA256 algorithm
  CryptoSHA256 sha256;
  sha256.update((const uint8_t*)apiSecret, strlen(apiSecret));
  sha256.update((const uint8_t*)dataA.c_str(), dataA.length());
  uint8_t hmacSignature[CryptoSHA256::HASH_SIZE];
  sha256.finalize(hmacSignature);

  String hmacSignatureString = "";
  for (int i = 0; i < CryptoSHA256::HASH_SIZE; i++) {
    if (hmacSignature[i] < 0x10) {
      hmacSignatureString += "0";
    }
    hmacSignatureString += String(hmacSignature[i], HEX);
  }

  Serial.println(dataA);
  Serial.println(hmacSignatureString);

  // Set request headers
  Serial.println("X-API-Key: " + String(apiKey));
  Serial.println("X-HMAC-Signature: " + hmacSignatureString);
  Serial.println("X-Timestamp: " + String(currentTimestamp));
}

void loop() {
  // Your code here
}
