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
	enum mbfl_no_encoding no_enc;
	const enum mbfl_no_language no_lang = mbfl_no_language_japanese;
	mbfl_memory_device dev;
	mbfl_string string, result;
	int final = 0;
	int state = 0;
	int mode = 0;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s encoding flags\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((no_enc = mbfl_name2no_encoding(argv[1])) ==
			mbfl_no_encoding_invalid) {
		printf("Unsupported encoding: %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	{
		const char *p;

		for (p= argv[2] + strlen(argv[2]); p > argv[2]; ) {
			switch (*(--p)) {
			case 'A':
				mode |= 0x1;
				break;
			case 'a':
				mode |= 0x10;
				break;
			case 'R':
				mode |= 0x2;
				break;
			case 'r':
				mode |= 0x20;
				break;
			case 'N':
				mode |= 0x4;
				break;
			case 'n':
				mode |= 0x40;
				break;
			case 'S':
				mode |= 0x8;
				break;
			case 's':
				mode |= 0x80;
				break;
			case 'K':
				mode |= 0x100;
				break;
			case 'k':
				mode |= 0x1000;
				break;
			case 'H':
				mode |= 0x200;
				break;
			case 'h':
				mode |= 0x2000;
				break;
			case 'V':
				mode |= 0x800;
				break;
			case 'C':
				mode |= 0x10000;
				break;
			case 'c':
				mode |= 0x20000;
				break;
			case 'M':
				mode |= 0x100000;
				break;
			case 'm':
				mode |= 0x200000;
				break;
			}
		}
	}

	do {
		mbfl_memory_device_init(&dev, 0, 4096);
		mbfl_string_init_set(&string, no_lang, no_enc);

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
		mbfl_ja_jp_hantozen(&string, &result, mode);
		hexdump(&result);
		mbfl_string_clear(&result);
		mbfl_string_clear(&string);
	} while (!final);

	return EXIT_SUCCESS;
}
