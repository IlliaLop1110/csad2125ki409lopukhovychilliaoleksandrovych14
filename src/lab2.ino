void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    String receivedMessage = Serial.readStringUntil('\n');
    Serial.println("modified: " + receivedMessage);
  }
}
