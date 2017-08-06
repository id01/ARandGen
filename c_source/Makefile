.PHONY: aes2
aes2: demo_aes2.c startrng_aes2.c arduino-serial/arduino-serial-lib.h
	gcc demo_aes2.c -O2 -o demo -lcrypto
	gcc startrng_aes2.c -O2 -o startrng -lcrypto

.PHONY: aes
aes: demo_aes.c startrng_aes.c arduino-serial/arduino-serial-lib.h
	gcc demo_aes.c -O2 -o demo -lcrypto
	gcc startrng_aes.c -O2 -o startrng -lcrypto

.PHONY: sha
sha: demo_sha.c startrng_sha.c arduino-serial/arduino-serial-lib.h
	gcc demo_sha.c -O2 -o demo -lcrypto
	gcc startrng_sha.c -O2 -o startrng -lcrypto

