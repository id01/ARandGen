typedef unsigned short randomtype; // Data type of random data. Larger is faster but less entropy.
int ppins = 6; // Number of physical analog pins
randomtype randBits; // Current random bits
randomtype rTotal; // Totals of random bits.
int ITERS = 11; // Number of iterations of XORing
int BYTESPERWRITE; // Bytes per write
int BITSPERWRITE; // Bits per write
byte BITSHIFT = 3; // Number of bits to shift per iteration
byte UNBITSHIFT;

void setup() {
  Serial.begin(38400); // Baud rate
  // Initialize all analog pins to INPUT
  for (int pin=A0; pin<ppins; pin++) {
    pinMode(pin, INPUT);
  }
  // Set variables that require operations
  BYTESPERWRITE = sizeof(randomtype);
  BITSPERWRITE = 8*BYTESPERWRITE;
  UNBITSHIFT = BITSPERWRITE-BITSHIFT;
}

void loop() {
  int i;
  // Calculate random total using loop. We do not need to initialize to 0.
  // After all, we're looking for random values :)
  for (i=0; i<ITERS; i++) {
    randBits = analogRead(i%ppins);
    rTotal ^= randBits;
    rTotal = (rTotal << BITSHIFT) | (rTotal >> UNBITSHIFT); // Shift rTotal with rotation
  }
  // Write serial
  randomtype rTotal2 = rTotal;
  for (i=0; i<BYTESPERWRITE; i++) {
    Serial.write((unsigned char)rTotal2);
    rTotal2 >>= 8;
  }
}
