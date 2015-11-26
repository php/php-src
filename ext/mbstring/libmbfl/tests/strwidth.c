/**
 * this is a small sample script to use libmbfl.
 * Rui Hirokawa <hirokawa@php.net>
 *
 * this file is encoded in EUC-JP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mbfl/mbfilter.h"

int main(int argc, char **argv)
{
	enum mbfl_no_encoding no_encoding;
	enum mbfl_no_language no_language;
	mbfl_memory_device dev;
	mbfl_string string;
	int final = 0;
	int state = 0;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s lang encoding\n", argv[0]);
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

	do {
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
		printf("%d\n", mbfl_strwidth(&string));
		mbfl_string_clear(&string);
	} while (!final);

	return EXIT_SUCCESS;
}
