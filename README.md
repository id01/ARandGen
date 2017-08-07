## ARandgen  
A hardware random number generator using Arduino ADC noise.  
No additional hardware required.  
Generates data at about 1 KB/s on an Arduino Uno.  
Note that data generation rate can vary greatly depending on system and configuration.  
View stats.txt for entropy test results.  

## Installation
Upload randgen.ino to arduino  
Configure files in c_source by changing FILENAME variable.  
Run make (mode)  
```
make # Default  
make aes2 # Double-AES (Default). Encrypts the data using two layers of AES for additional entropy.  
make aes2-long # Double-AES-Long. Encrypts the data using two layers of AES in 2048 byte chunks.  
make aes # Single-AES. Encrypts the data using one layer of AES for additional entropy. Fast, but primitive.  
make sha # SHA-512. Hashes the data with SHA512, halving the length, but increasing entropy.  
```
Run ./demo for demo  
Run sudo ./startrng to add device to entropy pool.  
Add startrng binary to startup if desired.  
