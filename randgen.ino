int ppins = 6; // Number of physical analog pins
bool randBit; // Current random bit
unsigned char rTotal[2]; // Totals of random bits

void setup() {
  Serial.begin(9600); // Use a slower baud rate as to not overload the processor
  for (int pin=A0; pin<ppins; pin++) {
    pinMode(pin, INPUT);
  }
}

void loop() {
  // Calculate random total using loop. We do not need to initialize to 0.
  // After all, we're looking for random values :)
  // Do it twice to do XOR for more randomness
  for (int i=0; i<8; i++) {
    randBit = analogRead(i%ppins)%2;
    rTotal[0] += randBit*pow(2,i);
  }
  for (int i=0; i<8; i++) {
    randBit = analogRead(i%ppins)%2;
    rTotal[1] += randBit*pow(2,i);
  }
  // Write serial
  Serial.write(rTotal[0] ^ rTotal[1]);
}
