#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/random.h>
#include <openssl/sha.h>

#include "arduino-serial/arduino-serial-lib.h"
#define INPUTINT 128

const char * FILENAME = "/dev/ttyACM0"; // File of Arduino random number generator
typedef unsigned char uchar;

void sha512(uchar *input, uchar output[64]) {
	SHA512_CTX shactx;
	SHA512_Init(&shactx);
	SHA512_Update(&shactx, input, INPUTINT);
	SHA512_Final(output, &shactx);
}

int main() {
	int sfd = serialport_init_rdonly(FILENAME, 38400); // Serial file descriptor
	if (sfd < 0) {
		printf("Could not open sfd. Is your device availiable at %s?\n", FILENAME);
		return 1;
	}
	uchar randbyte[1];
	uchar rawBytes[INPUTINT];
	uchar bytes2[64];
	int n;
	for (int x=0; x<4096/64; x++) {
		for (int i=0; i<INPUTINT; i++) {
			n = read(sfd, randbyte, 1);
			if (n == -1 || n == 0) {
				usleep(1000);
				i--;
			} else {
				rawBytes[i] = randbyte[0];
			}
		}
		sha512(rawBytes, bytes2);
		for (int i=0; i<64; i++) {
			fputc(bytes2[i], stdout);
		}
	}
	serialport_close(sfd);
}
