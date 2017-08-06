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

// Create random pool struct
#define BUFSIZE 1024
typedef struct rand_pool_info2 {
	int entropy_count;
	int buf_size;
	uchar buf[BUFSIZE];
} randpool;

void sha512(uchar *input, uchar output[65]) {
	SHA512_CTX shactx;
	SHA512_Init(&shactx);
	SHA512_Update(&shactx, input, INPUTINT);
	SHA512_Final(output, &shactx);
	output[64] = 0;
}

// Inputs: rp = random pool to initialize, fd = file to read randomness from
void init_rand_pool_info(randpool* rp, int fd) {
	rp->entropy_count = BUFSIZE*7; // I'm underestimating this on purpose.
	rp->buf_size = BUFSIZE;
	uchar randbyte[1];
	uchar tohash[INPUTINT];
	uchar hashout[65];
	int n;
	// Populate the buffer
	for (int x=0; x<BUFSIZE/64; x++) {
		// Populate tohash
		for (int i=0; i<INPUTINT; i++) {
			n = read(fd, randbyte, 1);
			tohash[i] = randbyte[0];
			if (n==-1) {
				rp->entropy_count = 0;
				rp->buf_size = 0;
				return;
			}
			else if (n==0) {
				usleep(1000);
			}
		}
		// Create hash and copy it over to the buffer
		sha512(tohash, hashout);
		for (int i=0; i<64; i++) {
			rp->buf[x*64+i] = hashout[i];
		}
	}
}

int main() {
	int sfd = serialport_init_rdonly(FILENAME, 38400); // Serial file descriptor
	int rfd = open("/dev/random", O_WRONLY); // Random file descriptor
	if (sfd < 0 || rfd < 0) {
		printf("Could not open sfd or rfd. Is your device availiable at %s?\n", FILENAME);
		return 1;
	}
	// Allocate a random pool struct, repeatedly populate it, and add entropy.
	randpool* randomness = malloc(sizeof(randpool));
	while (1) {
		init_rand_pool_info(randomness, sfd);
		if (ioctl(rfd, RNDADDENTROPY, randomness) < 0) {
			puts("Could not add entropy. Are you root?");
			return 1;
		} else {
			printf("Adding %d bytes to entropy pool for a total of %d bits of entropy\n", randomness->buf_size, randomness->entropy_count);
		}
		usleep(100000);
	}
	serialport_close(sfd);
	close(rfd);
}
