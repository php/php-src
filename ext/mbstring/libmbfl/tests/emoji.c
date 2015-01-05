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

//#define TEST_DOCOMO
//#define TEST_KDDI
#define TEST_SOFTBANK

int main(int argc, char **argv)
{
	enum mbfl_no_encoding from_encoding, to_encoding;
	enum mbfl_no_language no_language;
	mbfl_buffer_converter *convd = NULL, *convd2 = NULL;
	mbfl_memory_device dev, dev2;
	mbfl_string string, result, *ret;
#ifdef TEST_DOCOMO
	//char str[] = {0xF9,0xD7,0x00}; // U+2122
	//char str[] = {0xF9,0x82,0x00};   // U+1F195
	char str[] = {0xF9,0xD6,0x00};   // U+00A9
#endif
#ifdef TEST_KDDI
	//char str[] = {0xF7,0x6A,0x00};// U+2122
	//char str[] = {0xF7,0xE5,0x00};  // U+1F195
	//char str[] = {0xF3,0xD2,0x00}; // U+1F1E8 U+1F1F3
	char str[] = {0xF7,0x74,0x00};  // U+00A9
#endif
#ifdef TEST_SOFTBANK
	//char str[] = {0xFB,0xD7,0x00};// U+2122
	//char str[] = {0xF7,0xB2,0x00};  // U+1F195
	//char str[] = {0xFB,0xB3,0x00}; // U+1F1E8 U+1F1F3
	char str[] = {0xF7,0xEE,0x00}; // U+00A9
#endif
	int final = 0;
	int state = 0;
	int i;

	no_language = mbfl_name2no_language("Japanese");
#ifdef TEST_DOCOMO
	from_encoding = mbfl_name2no_encoding("SJIS-win#DOCOMO");
#endif
#ifdef TEST_KDDI
	from_encoding = mbfl_name2no_encoding("SJIS-win#KDDI");
#endif
#ifdef TEST_SOFTBANK
	from_encoding = mbfl_name2no_encoding("SJIS-win#SOFTBANK");
#endif
	to_encoding = mbfl_name2no_encoding("UTF-8");

	convd = mbfl_buffer_converter_new(from_encoding, to_encoding, 0);

	mbfl_memory_device_init(&dev, 0, 4096);
	mbfl_string_init_set(&string, no_language, from_encoding);
	mbfl_memory_device_realloc(&dev, dev.length + dev.allocsz, dev.allocsz);

	strcpy(dev.buffer, str);
	dev.pos += strlen(str);

	mbfl_memory_device_result(&dev, &string);
	mbfl_string_init_set(&result, no_language, to_encoding);
	ret = mbfl_buffer_converter_feed_result(convd, &string, &result);

#if 0
	for (i = 0; i < result.len; i+= 2) {
		if (result.val[i] >= 0xD8 && result.val[i] < 0xE0) { // Surrogate pair
			int h = (result.val[i] & 0x07)<<8 | result.val[i+1];
			int l = (result.val[i+2] & 0x03)<<8 | result.val[i+3];
			int c = (h<<(2+8)) | l;
			printf("U+%x\n",c+0x10000);
			i+=2;
		} else {
			printf("U+%x\n",(result.val[i] << 8) | result.val[i+1]);
		}
	}
	hexdump(&result);
#endif


#if 1
	convd2 = mbfl_buffer_converter_new(to_encoding, from_encoding, 0);
	mbfl_memory_device_init(&dev2, 0, 4096);
	mbfl_string_init_set(&string, no_language, to_encoding);
	mbfl_memory_device_realloc(&dev2, dev2.length + dev2.allocsz, dev2.allocsz);

	memcpy(dev2.buffer, result.val, result.len+1);
	dev2.pos += strlen(dev2.buffer);

	mbfl_memory_device_result(&dev2, &string);
	mbfl_string_init_set(&result, no_language, from_encoding);
	ret = mbfl_buffer_converter_feed_result(convd2, &string, &result);
	hexdump(&result);
	mbfl_buffer_converter_delete(convd2);	
#endif
	mbfl_string_clear(&result);
	mbfl_string_clear(&string);

	mbfl_buffer_converter_delete(convd);	


	return EXIT_SUCCESS;
}
