#define BYTESPERWRITE 2
#define BITSPERREAD 2
int ppins = 6; // Number of physical analog pins
unsigned char randBits; // Current random bits
unsigned char rTotal[BYTESPERWRITE]; // Totals of random bits.
int moduloer;

void setup() {
  Serial.begin(9600); // Use a slower baud rate as to not overload the processor
  for (int pin=A0; pin<ppins; pin++) {
    pinMode(pin, INPUT);
  }
  moduloer = pow(2, BITSPERREAD);
}

void loop() {
  int i;
  // Calculate random total using loop. We do not need to initialize to 0.
  // After all, we're looking for random values :)
  for (i=0; i<(8*BYTESPERWRITE)/BITSPERREAD; i++) {
    randBits = analogRead(i%ppins)%moduloer;
    rTotal[i/moduloer] ^= randBits*(unsigned char)(pow(moduloer,i%moduloer));
  }
  // Write serial
  for (i=0; i<BYTESPERWRITE; i++) {
    Serial.write(rTotal[i]);
  }
}
