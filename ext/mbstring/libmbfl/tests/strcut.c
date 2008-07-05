/**
 * this is a small sample script to use libmbfl.
 * Rui Hirokawa <hirokawa@php.net>
 *
 * this file is encoded in EUC-JP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mbfl/mbfilter.h"

static void hexdump(const mbfl_string *ptr)
{
	unsigned int i;

	for (i = 0; i < ptr->len; i++) {
		printf("%%%02x", ptr->val[i]);
	}

	printf(" (%u)\n", ptr->len);
}

int main(int argc, char **argv)
{
	enum mbfl_no_encoding no_encoding;
	enum mbfl_no_language no_language;
	mbfl_memory_device dev;
	mbfl_string string;
	int offset, length;
	int final = 0;
	int state = 0;

	if (argc < 5) {
		fprintf(stderr, "Usage: %s lang encoding offset length\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((no_language = mbfl_name2no_language(argv[1])) ==
			mbfl_no_language_invalid) {
		printf("Unsupported NLS: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	if ((no_encoding = mbfl_name2no_encoding(argv[2])) ==
			mbfl_no_encoding_invalid) {
		printf("Unsupported encoding: %s\n", argv[2]);
		return EXIT_FAILURE;
	}

	errno = 0;
	offset = strtol(argv[3], NULL, 10);
	if (errno) {
		printf("Invalid offset: %s\n", argv[3]);
		return EXIT_FAILURE;
	}

	length = strtol(argv[4], NULL, 10);
	if (errno) {
		printf("Invalid length: %s\n", argv[4]);
		return EXIT_FAILURE;
	}


	do {
		mbfl_string result;

		mbfl_memory_device_init(&dev, 0, 4096);
		mbfl_string_init_set(&string, no_language, no_encoding);

		for (;;) {
			const int c = fgetc(stdin);

			if (c == EOF) {
				final = 1;
				break;
			} else if (c == 10) {
				if (state == 1) {
					state = 0;
					continue;
				}
				break;
			} else if (c == 13) {
				state = 1;
				break;
			}

			if (dev.pos >= dev.length) {
				if (dev.length + dev.allocsz < dev.length) {
					printf("Unable to allocate memory\n");
					return EXIT_FAILURE;
				}

				mbfl_memory_device_realloc(&dev, dev.length + dev.allocsz,
						dev.allocsz);
			}

			dev.buffer[dev.pos++] = (unsigned char)c;
		}

		mbfl_memory_device_result(&dev, &string);
		if (mbfl_strcut(&string, &result, offset, length)) {
			hexdump(&result);
			mbfl_string_clear(&result);
		} else {
			printf("***ERROR***\n");
		}
		mbfl_string_clear(&string);
	} while (!final);

	return EXIT_SUCCESS;
}
