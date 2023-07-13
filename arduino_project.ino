//  "a2ee973a-8e1e-4be0-b689-11704d514b72" DEVICE_ID change if new device
//  "14444878-632a-07be-67c5-703e3f566392"; KEY 
//  "f2c24d11-a83e-dd9b-579c-9be7228a1a26"; SECRET

// Pin connected to the LED


void setup() {
  // Initialize the digital pin as an output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Turn the LED on
  digitalWrite(LED_PIN, HIGH);
  
  // Wait for 1 second
  delay(1000);

  // Turn the LED off
  digitalWrite(LED_PIN, LOW);
  

