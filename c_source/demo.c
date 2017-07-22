#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/random.h>

#include "arduino-serial/arduino-serial-lib.h"

const char * FILENAME = "/dev/ttyACM0"; // File of Arduino random number generator

int main() {
	int sfd = serialport_init_rdonly(FILENAME, 9600); // Serial file descriptor
	if (sfd < 0) {
		printf("Could not open sfd. Is your device availiable at %s?\n", FILENAME);
		return 1;
	}
	unsigned char randbyte[1];
	int n;
	while (1) {
		n = read(sfd, randbyte, 1);
		if (n == -1 || n == 0) {
			usleep(1000);
		} else {
			fputc(randbyte[0], stdout);
		}
	}
	serialport_close(sfd);
}
