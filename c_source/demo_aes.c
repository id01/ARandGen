#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/random.h>

#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

#include "arduino-serial/arduino-serial-lib.h"
#define INPUTINT 256
#define SHAINPUTSIZE 64

const char * FILENAME = "/dev/ttyACM0"; // File of Arduino random number generator
typedef unsigned char uchar;

// Get sha256 hash of input and put into output
void sha256(uchar input[SHAINPUTSIZE], uchar output[32]) {
	SHA256_CTX shactx;
	SHA256_Init(&shactx);
	SHA256_Update(&shactx, input, SHAINPUTSIZE);
	SHA256_Final(output, &shactx);
}

// Get numbytes from fd and put into output
void getnumbytes(int fd, int numbytes, char* output) {
	int n;
	uchar randbyte[1];
	for (int i=0; i<numbytes; i++) {
		n = read(fd, randbyte, 1);
		if (n == 0) {
			usleep(1000);
			i--;
		} else if (n == -1) {
			printf("ERROR!\n");
			return;
		} else {
			output[i] = randbyte[0];
		}
	}
}

// Encrypt using AES
int encrypt(unsigned char *plaintext, int plaintext_len, uchar *key, uchar *iv, uchar *ciphertext) {
	int len=0;
	int finallen=0;
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	if (1!=EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
		ERR_print_errors_fp(stderr);
	if (1!=EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		ERR_print_errors_fp(stderr);
	if (1!=EVP_EncryptFinal_ex(ctx, ciphertext+len, &finallen))
		ERR_print_errors_fp(stderr);
	len += finallen;
	EVP_CIPHER_CTX_free(ctx);
	return len;
}

int main() {
	// Open serialport
	int sfd = serialport_init_rdonly(FILENAME, 38400); // Serial file descriptor
	if (sfd < 0) {
		printf("Could not open sfd. Is your device availiable at %s?\n", FILENAME);
		return 1;
	}
	// Create sha256 hash of first 128 chars for AES key
	uchar shabytes[SHAINPUTSIZE];
	uchar aeskey[32];
	uchar aesiv[32];
	uchar aesrawbytes[INPUTINT];
	uchar aesencbytes[INPUTINT+64];
	int ciphertext_len;
	// Init openssl
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	// Get random data
	for (int x=0; x<4096/INPUTINT; x++) {
		// Get AES key and IV
		getnumbytes(sfd, SHAINPUTSIZE, shabytes);
		sha256(shabytes, aeskey);
		getnumbytes(sfd, SHAINPUTSIZE, shabytes);
		sha256(shabytes, aesiv);
		// Get AES bytes and encrypt
		getnumbytes(sfd, INPUTINT, aesrawbytes);
		ciphertext_len = encrypt(aesrawbytes, INPUTINT, aeskey, aesiv, aesencbytes);
		// Put encrypted bytes to screen
		for (int i=0; i<INPUTINT; i++) {
			fputc(aesencbytes[i], stdout);
		}
	}
	serialport_close(sfd);
}
