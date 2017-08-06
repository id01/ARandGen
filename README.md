## ARandgen  
A hardware random number generator using Arduino ADC noise.  
No additional hardware required.  
Generates data at about 1 KB/s on an Arduino Uno.  
Note that data generation rate can vary greatly depending on system and configuration.  

## Installation
Upload randgen.ino to arduino  
Configure files in c_source by changing FILENAME variable.  
Run make  
Run ./demo for demo  
Run sudo ./startrng to add device to entropy pool.  
Add startrng binary to startup if desired.  
