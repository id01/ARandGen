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
#define SHAINPUTSIZE 128

const char * FILENAME = "/dev/ttyACM0"; // File of Arduino random number generator
typedef unsigned char uchar;

// Get sha256 hash of input and put into output
void sha256(uchar* input, uchar output[32], int inputlen) {
	SHA256_CTX shactx;
	SHA256_Init(&shactx);
	SHA256_Update(&shactx, input, inputlen);
	SHA256_Final(output, &shactx);
}
// Get sha512 hash of input and put into output
void sha512(uchar* input, uchar output[64], int inputlen) {
	SHA512_CTX shactx;
	SHA512_Init(&shactx);
	SHA512_Update(&shactx, input, inputlen);
	SHA512_Final(output, &shactx);
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
	uchar *aeskey = malloc(64); // Aesiv is basically a pointer to the halfway point of aeskey
	uchar *aesiv = aeskey+32;
	uchar *aesrawbytes = malloc(INPUTINT);
	uchar *aesencbytes = malloc(INPUTINT+16);
	uchar *aesencbytes2 = malloc(INPUTINT);
	uchar *aestagandlastblock = aesencbytes+INPUTINT-16; // Tag and last block for aesencbytes
	int ciphertext_len;
	// Init openssl
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
	OPENSSL_config(NULL);
	// Get random data
	for (int x=0; x<4096/INPUTINT; x++) {
		// Get AES key and IV
		getnumbytes(sfd, SHAINPUTSIZE, shabytes);
		sha512(shabytes, aeskey, SHAINPUTSIZE);
		// Get AES bytes and encrypt
		getnumbytes(sfd, INPUTINT, aesrawbytes);
		ciphertext_len = encrypt(aesrawbytes, INPUTINT, aeskey, aesiv, aesencbytes);
		// Get AES key and IV for second encryption
		for (int i=0; i<32; i++) {
			aesiv[i] = aestagandlastblock[i]; // Get the aes iv from the aes tag and last block
		}
		sha256(aesiv, aeskey, 32); // SHA256 hash the AES IV for the AES key
		ciphertext_len = encrypt(aesencbytes, INPUTINT-16, aeskey, aesiv, aesencbytes2); // Encrypt using AES a second time
		// Put encrypted bytes to screen
		for (int i=0; i<INPUTINT; i++) {
			fputc(aesencbytes2[i], stdout);
		}
	}
	serialport_close(sfd);
}
