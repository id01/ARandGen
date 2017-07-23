## ARandgen  
A hardware random number generator using Arduino ADC noise.  
No additional hardware required.  
Generates data at about 2 KB/s on an Arduino Uno.  
Entropy ~ 7.94 (4 KB)  

## Installation
Upload randgen.ino to arduino  
Configure files in c_source by changing FILENAME variable.  
Run make  
Run ./demo for demo  
Run sudo ./startrng to add device to entropy pool.  
Add startrng binary to startup if desired.  
