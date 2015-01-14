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
	enum mbfl_no_encoding from_encoding, to_encoding;
	enum mbfl_no_language no_language;
	mbfl_buffer_converter *convd = NULL;
	mbfl_memory_device dev;
	mbfl_string string, result, *ret;
	int final = 0;
	int state = 0;

	if (argc < 4) {
		fprintf(stderr, "Usage: %s lang to_encoding from_encoding\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((no_language = mbfl_name2no_language(argv[1])) ==
			mbfl_no_language_invalid) {
		printf("Unsupported NLS: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	if ((to_encoding = mbfl_name2no_encoding(argv[2])) ==
			mbfl_no_encoding_invalid) {
		printf("Unsupported encoding: %s\n", argv[2]);
		return EXIT_FAILURE;
	}

	if ((from_encoding = mbfl_name2no_encoding(argv[3])) ==
			mbfl_no_encoding_invalid) {
		printf("Unsupported encoding: %s\n", argv[3]);
		return EXIT_FAILURE;
	}

	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0);

	do {
		mbfl_memory_device_init(&dev, 0, 4096);
		mbfl_string_init_set(&string, no_language, from_encoding);

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
		mbfl_string_init_set(&result, no_language, to_encoding);
		ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
		hexdump(&result);
		mbfl_string_clear(&result);
		mbfl_string_clear(&string);
	} while (!final);

	mbfl_buffer_converter_delete(convd);

	return EXIT_SUCCESS;
}
