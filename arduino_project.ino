#include <Arduino.h>
#include <Crypto.h>
#include <CryptoLib.h>

const char* apiKey = "14444878-632a-07be-67c5-703e3f566392";
const char* apiSecret = "f2c24d11-a83e-dd9b-579c-9be7228a1a26";
char url[] = "{{RELAY_BASE_URL}}"; // Replace with the actual URL
char dataA[128];

void setup() {
  Serial.begin(9600);
  
  // Generate current timestamp
  unsigned long currentTimestamp = millis();

  // Construct dataA
  sprintf(dataA, "%lu/%s", currentTimestamp, url);

  // Generate the HMAC signature with SHA256 algorithm
  byte hmacSignature[32];
  Sha256.initHmac((const uint8_t*)apiSecret, strlen(apiSecret));
  Sha256.print(dataA);
  Sha256.getHmacResult(hmacSignature, true);

  // Print dataA and hmacSignature
  Serial.println(dataA);
  Serial.println();

  Serial.print("HMAC Signature: ");
  for (int i = 0; i < sizeof(hmacSignature); i++) {
    if (hmacSignature[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(hmacSignature[i], HEX);
  }
  Serial.println();

  // Add headers to the request
  // Replace "client" with the appropriate client library you are using for sending HTTP requests
  client.addHeader("X-API-Key", apiKey);
  client.addHeader("X-HMAC-Signature", hmacSignature);
  client.addHeader("X-Timestamp", String(currentTimestamp));
}

void loop() {
  // Your code here
}
