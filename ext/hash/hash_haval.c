/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  +----------------------------------------------------------------------+
*/

#include "php_hash.h"
#include "php_hash_haval.h"

static const unsigned char PADDING[128] ={
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static const uint32_t D0[8] = {
	0x243F6A88, 0x85A308D3, 0x13198A2E, 0x03707344, 0xA4093822, 0x299F31D0, 0x082EFA98, 0xEC4E6C89 };

static const uint32_t K2[32] = {
	0x452821E6, 0x38D01377, 0xBE5466CF, 0x34E90C6C, 0xC0AC29B7, 0xC97C50DD, 0x3F84D5B5, 0xB5470917,
	0x9216D5D9, 0x8979FB1B, 0xD1310BA6, 0x98DFB5AC, 0x2FFD72DB, 0xD01ADFB7, 0xB8E1AFED, 0x6A267E96,
	0xBA7C9045, 0xF12C7F99, 0x24A19947, 0xB3916CF7, 0x0801F2E2, 0x858EFC16, 0x636920D8, 0x71574E69,
	0xA458FEA3, 0xF4933D7E, 0x0D95748F, 0x728EB658, 0x718BCD58, 0x82154AEE, 0x7B54A41D, 0xC25A59B5 };

static const uint32_t K3[32] = {
	0x9C30D539, 0x2AF26013, 0xC5D1B023, 0x286085F0, 0xCA417918, 0xB8DB38EF, 0x8E79DCB0, 0x603A180E,
	0x6C9E0E8B, 0xB01E8A3E, 0xD71577C1, 0xBD314B27, 0x78AF2FDA, 0x55605C60, 0xE65525F3, 0xAA55AB94,
	0x57489862, 0x63E81440, 0x55CA396A, 0x2AAB10B6, 0xB4CC5C34, 0x1141E8CE, 0xA15486AF, 0x7C72E993,
	0xB3EE1411, 0x636FBC2A, 0x2BA9C55D, 0x741831F6, 0xCE5C3E16, 0x9B87931E, 0xAFD6BA33, 0x6C24CF5C };

static const uint32_t K4[32] = {
	0x7A325381, 0x28958677, 0x3B8F4898, 0x6B4BB9AF, 0xC4BFE81B, 0x66282193, 0x61D809CC, 0xFB21A991,
	0x487CAC60, 0x5DEC8032, 0xEF845D5D, 0xE98575B1, 0xDC262302, 0xEB651B88, 0x23893E81, 0xD396ACC5,
	0x0F6D6FF3, 0x83F44239, 0x2E0B4482, 0xA4842004, 0x69C8F04A, 0x9E1F9B5E, 0x21C66842, 0xF6E96C9A,
	0x670C9C61, 0xABD388F0, 0x6A51A0D2, 0xD8542F68, 0x960FA728, 0xAB5133A3, 0x6EEF0B6C, 0x137A3BE4 };

static const uint32_t K5[32] = {
	0xBA3BF050, 0x7EFB2A98, 0xA1F1651D, 0x39AF0176, 0x66CA593E, 0x82430E88, 0x8CEE8619, 0x456F9FB4,
	0x7D84A5C3, 0x3B8B5EBE, 0xE06F75D8, 0x85C12073, 0x401A449F, 0x56C16AA6, 0x4ED3AA62, 0x363F7706,
	0x1BFEDF72, 0x429B023D, 0x37D0D724, 0xD00A1248, 0xDB0FEAD3, 0x49F1C09B, 0x075372C9, 0x80991B7B,
	0x25D479D8, 0xF6E8DEF7, 0xE3FE501A, 0xB6794C3B, 0x976CE0BD, 0x04C006BA, 0xC1A94FB6, 0x409F60C4 };

static const short I2[32] = {	 5, 14, 26, 18, 11, 28,  7, 16,  0, 23, 20, 22,  1, 10,  4,  8,
						30,  3, 21,  9, 17, 24, 29,  6, 19, 12, 15, 13,  2, 25, 31, 27 };

static const short I3[32] = {	19,  9,  4, 20, 28, 17,  8, 22, 29, 14, 25, 12, 24, 30, 16, 26,
						31, 15,  7,  3,  1,  0, 18, 27, 13,  6, 21, 10, 23, 11,  5,  2 };

static const short I4[32] = {	24,  4,  0, 14,  2,  7, 28, 23, 26,  6, 30, 20, 18, 25, 19,  3,
						22, 11, 31, 21,  8, 27, 12,  9,  1, 29,  5, 15, 17, 10, 16, 13 };

static const short I5[32] = {	27,  3, 21, 26, 17, 11, 20, 29, 19,  0, 12,  7, 13,  8, 31, 10,
						 5,  9, 14, 30, 18,  6, 28, 24,  2, 23, 16, 22,  4,  1, 25, 15 };

static const short M0[32] = {	0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1,
						0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1 };

static const short M1[32] = {	1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2,
						1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2 };

static const short M2[32] = {	2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3,
						2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3 };

static const short M3[32] = {	3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4,
						3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4 };

static const short M4[32] = {	4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5,
						4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5 };

static const short M5[32] = {	5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6,
						5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7, 6 };

static const short M6[32] = {	6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7,
						6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0, 7 };

static const short M7[32] = {	7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0,
						7, 6, 5, 4, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0 };

/* {{{ Encode
   Encodes input (uint32_t) into output (unsigned char). Assumes len is
   a multiple of 4.
 */
static void Encode(unsigned char *output, uint32_t *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char) (input[i] & 0xff);
		output[j + 1] = (unsigned char) ((input[i] >> 8) & 0xff);
		output[j + 2] = (unsigned char) ((input[i] >> 16) & 0xff);
		output[j + 3] = (unsigned char) ((input[i] >> 24) & 0xff);
	}
}
/* }}} */

/* {{{ Decode
   Decodes input (unsigned char) into output (uint32_t). Assumes len is
   a multiple of 4.
 */
static void Decode(uint32_t *output, const unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[i] = ((uint32_t) input[j]) | (((uint32_t) input[j + 1]) << 8) |
			(((uint32_t) input[j + 2]) << 16) | (((uint32_t) input[j + 3]) << 24);
	}
}
/* }}} */

#define F1(x6,x5,x4,x3,x2,x1,x0)	( ((x1) & (x4)) ^ ((x2) & (x5)) ^ ((x3) & (x6)) ^ ((x0) & (x1)) ^ (x0) )
#define F2(x6,x5,x4,x3,x2,x1,x0)	( ((x1) & (x2) & (x3)) ^ ((x2) & (x4) & (x5)) ^ ((x1) & (x2)) ^ ((x1) & (x4)) ^ \
									  ((x2) & (x6)) ^ ((x3) & (x5)) ^ ((x4) & (x5)) ^ ((x0) & (x2)) ^ (x0) )
#define F3(x6,x5,x4,x3,x2,x1,x0)	( ((x1) & (x2) & (x3)) ^ ((x1) & (x4)) ^ ((x2) & (x5)) ^ ((x3) & (x6)) ^ ((x0) & (x3)) ^ (x0) )
#define F4(x6,x5,x4,x3,x2,x1,x0)	( ((x1) & (x2) & (x3)) ^ ((x2) & (x4) & (x5)) ^ ((x3) & (x4) & (x6)) ^ \
									  ((x1) & (x4)) ^ ((x2) & (x6)) ^ ((x3) & (x4)) ^ ((x3) & (x5)) ^ \
									  ((x3) & (x6)) ^ ((x4) & (x5)) ^ ((x4) & (x6)) ^ ((x0) & (x4)) ^ (x0) )
#define F5(x6,x5,x4,x3,x2,x1,x0)	( ((x1) & (x4)) ^ ((x2) & (x5)) ^ ((x3) & (x6)) ^ \
									  ((x0) & (x1) & (x2) & (x3)) ^ ((x0) & (x5)) ^ (x0) )

#define ROTR(x,n)	(((x) >> (n)) | ((x) << (32 - (n))))


/* {{{ PHP_3HAVALTransform
 */
static void PHP_3HAVALTransform(uint32_t state[8], const unsigned char block[128])
{
	uint32_t E[8];
	uint32_t x[32];
	int i;

	Decode(x, block, 128);

	for(i = 0; i < 8; i++) {
		E[i] = state[i];
	}

	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F1(E[M1[i]],E[M0[i]],E[M3[i]],E[M5[i]],E[M6[i]],E[M2[i]],E[M4[i]]),7) + ROTR(E[M7[i]],11) + x[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F2(E[M4[i]],E[M2[i]],E[M1[i]],E[M0[i]],E[M5[i]],E[M3[i]],E[M6[i]]),7) + ROTR(E[M7[i]],11) + x[I2[i]] + K2[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F3(E[M6[i]],E[M1[i]],E[M2[i]],E[M3[i]],E[M4[i]],E[M5[i]],E[M0[i]]),7) + ROTR(E[M7[i]],11) + x[I3[i]] + K3[i];
	}

	/* Update digest */
	for(i = 0; i < 8; i++) {
		state[i] += E[i];
	}

	/* Zeroize sensitive information. */
	ZEND_SECURE_ZERO((unsigned char*) x, sizeof(x));
}
/* }}} */

/* {{{ PHP_4HAVALTransform
 */
static void PHP_4HAVALTransform(uint32_t state[8], const unsigned char block[128])
{
	uint32_t E[8];
	uint32_t x[32];
	int i;

	Decode(x, block, 128);

	for(i = 0; i < 8; i++) {
		E[i] = state[i];
	}

	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F1(E[M2[i]],E[M6[i]],E[M1[i]],E[M4[i]],E[M5[i]],E[M3[i]],E[M0[i]]),7) + ROTR(E[M7[i]],11) + x[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F2(E[M3[i]],E[M5[i]],E[M2[i]],E[M0[i]],E[M1[i]],E[M6[i]],E[M4[i]]),7) + ROTR(E[M7[i]],11) + x[I2[i]] + K2[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F3(E[M1[i]],E[M4[i]],E[M3[i]],E[M6[i]],E[M0[i]],E[M2[i]],E[M5[i]]),7) + ROTR(E[M7[i]],11) + x[I3[i]] + K3[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F4(E[M6[i]],E[M4[i]],E[M0[i]],E[M5[i]],E[M2[i]],E[M1[i]],E[M3[i]]),7) + ROTR(E[M7[i]],11) + x[I4[i]] + K4[i];
	}

	/* Update digest */
	for(i = 0; i < 8; i++) {
		state[i] += E[i];
	}

	/* Zeroize sensitive information. */
	ZEND_SECURE_ZERO((unsigned char*) x, sizeof(x));
}
/* }}} */

/* {{{ PHP_5HAVALTransform
 */
static void PHP_5HAVALTransform(uint32_t state[8], const unsigned char block[128])
{
	uint32_t E[8];
	uint32_t x[32];
	int i;

	Decode(x, block, 128);

	for(i = 0; i < 8; i++) {
		E[i] = state[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F1(E[M3[i]],E[M4[i]],E[M1[i]],E[M0[i]],E[M5[i]],E[M2[i]],E[M6[i]]),7) + ROTR(E[M7[i]],11) + x[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F2(E[M6[i]],E[M2[i]],E[M1[i]],E[M0[i]],E[M3[i]],E[M4[i]],E[M5[i]]),7) + ROTR(E[M7[i]],11) + x[I2[i]] + K2[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F3(E[M2[i]],E[M6[i]],E[M0[i]],E[M4[i]],E[M3[i]],E[M1[i]],E[M5[i]]),7) + ROTR(E[M7[i]],11) + x[I3[i]] + K3[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F4(E[M1[i]],E[M5[i]],E[M3[i]],E[M2[i]],E[M0[i]],E[M4[i]],E[M6[i]]),7) + ROTR(E[M7[i]],11) + x[I4[i]] + K4[i];
	}
	for(i = 0; i < 32; i++) {
		E[7 - (i % 8)] = ROTR(F5(E[M2[i]],E[M5[i]],E[M0[i]],E[M6[i]],E[M4[i]],E[M3[i]],E[M1[i]]),7) + ROTR(E[M7[i]],11) + x[I5[i]] + K5[i];
	}

	/* Update digest */
	for(i = 0; i < 8; i++) {
		state[i] += E[i];
	}

	/* Zeroize sensitive information. */
	ZEND_SECURE_ZERO((unsigned char*) x, sizeof(x));
}
/* }}} */

#define PHP_HASH_HAVAL_INIT(p,b) \
const php_hash_ops php_hash_##p##haval##b##_ops = { \
	(php_hash_init_func_t) PHP_##p##HAVAL##b##Init, \
	(php_hash_update_func_t) PHP_HAVALUpdate, \
	(php_hash_final_func_t) PHP_HAVAL##b##Final, \
	(php_hash_copy_func_t) php_hash_copy, \
	((b) / 8), 128, sizeof(PHP_HAVAL_CTX), 1 }; \
PHP_HASH_API void PHP_##p##HAVAL##b##Init(PHP_HAVAL_CTX *context) \
{	int i; context->count[0] = 	context->count[1] = 	0; \
	for(i = 0; i < 8; i++) context->state[i] = D0[i]; \
	context->passes = p;	context->output = b; \
	context->Transform = PHP_##p##HAVALTransform; }

PHP_HASH_HAVAL_INIT(3,128)
PHP_HASH_HAVAL_INIT(3,160)
PHP_HASH_HAVAL_INIT(3,192)
PHP_HASH_HAVAL_INIT(3,224)
PHP_HASH_HAVAL_INIT(3,256)

PHP_HASH_HAVAL_INIT(4,128)
PHP_HASH_HAVAL_INIT(4,160)
PHP_HASH_HAVAL_INIT(4,192)
PHP_HASH_HAVAL_INIT(4,224)
PHP_HASH_HAVAL_INIT(4,256)

PHP_HASH_HAVAL_INIT(5,128)
PHP_HASH_HAVAL_INIT(5,160)
PHP_HASH_HAVAL_INIT(5,192)
PHP_HASH_HAVAL_INIT(5,224)
PHP_HASH_HAVAL_INIT(5,256)

/* {{{ PHP_HAVALUpdate
 */
PHP_HASH_API void PHP_HAVALUpdate(PHP_HAVAL_CTX *context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 128 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7F);
	/* Update number of bits */
	if ((context->count[0] += ((uint32_t) inputLen << 3)) < ((uint32_t) inputLen << 3)) {
		context->count[1]++;
	}
	context->count[1] += ((uint32_t) inputLen >> 29);

	partLen = 128 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy((unsigned char*) & context->buffer[index], (unsigned char*) input, partLen);
		context->Transform(context->state, context->buffer);

		for (i = partLen; i + 127 < inputLen; i += 128) {
			context->Transform(context->state, &input[i]);
		}

		index = 0;
	} else {
		i = 0;
	}

	/* Buffer remaining input */
	memcpy((unsigned char*) &context->buffer[index], (unsigned char*) &input[i], inputLen - i);
}
/* }}} */

#define PHP_HASH_HAVAL_VERSION	0x01

/* {{{ PHP_HAVAL128Final
 */
PHP_HASH_API void PHP_HAVAL128Final(unsigned char *digest, PHP_HAVAL_CTX * context)
{
	unsigned char bits[10];
	unsigned int index, padLen;

	/* Version, Passes, and Digest Length */
	bits[0] =	(PHP_HASH_HAVAL_VERSION & 0x07) |
				((context->passes & 0x07) << 3) |
				((context->output & 0x03) << 6);
	bits[1] = (context->output >> 2);

	/* Save number of bits */
	Encode(bits + 2, context->count, 8);

	/* Pad out to 118 mod 128.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7f);
	padLen = (index < 118) ? (118 - index) : (246 - index);
	PHP_HAVALUpdate(context, PADDING, padLen);

	/* Append version, passes, digest length, and message length */
	PHP_HAVALUpdate(context, bits, 10);

	/* Store state in digest */
	context->state[3] += (context->state[7] & 0xFF000000) |
						 (context->state[6] & 0x00FF0000) |
						 (context->state[5] & 0x0000FF00) |
						 (context->state[4] & 0x000000FF);

	context->state[2] += (((context->state[7] & 0x00FF0000) |
						   (context->state[6] & 0x0000FF00) |
						   (context->state[5] & 0x000000FF)) << 8) |
						  ((context->state[4] & 0xFF000000) >> 24);

	context->state[1] += (((context->state[7] & 0x0000FF00) |
						   (context->state[6] & 0x000000FF)) << 16) |
						 (((context->state[5] & 0xFF000000) |
						   (context->state[4] & 0x00FF0000)) >> 16);

	context->state[0] +=  ((context->state[7] & 0x000000FF) << 24) |
						 (((context->state[6] & 0xFF000000) |
						   (context->state[5] & 0x00FF0000) |
						   (context->state[4] & 0x0000FF00)) >> 8);

	Encode(digest, context->state, 16);

	/* Zeroize sensitive information.
	 */
	ZEND_SECURE_ZERO((unsigned char*) context, sizeof(*context));
}
/* }}} */

/* {{{ PHP_HAVAL160Final
 */
PHP_HASH_API void PHP_HAVAL160Final(unsigned char *digest, PHP_HAVAL_CTX * context)
{
	unsigned char bits[10];
	unsigned int index, padLen;

	/* Version, Passes, and Digest Length */
	bits[0] =	(PHP_HASH_HAVAL_VERSION & 0x07) |
				((context->passes & 0x07) << 3) |
				((context->output & 0x03) << 6);
	bits[1] = (context->output >> 2);

	/* Save number of bits */
	Encode(bits + 2, context->count, 8);

	/* Pad out to 118 mod 128.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7f);
	padLen = (index < 118) ? (118 - index) : (246 - index);
	PHP_HAVALUpdate(context, PADDING, padLen);

	/* Append version, passes, digest length, and message length */
	PHP_HAVALUpdate(context, bits, 10);

	/* Store state in digest */
	context->state[4] += ((context->state[7] & 0xFE000000) |
						  (context->state[6] & 0x01F80000) |
						  (context->state[5] & 0x0007F000)) >> 12;

	context->state[3] += ((context->state[7] & 0x01F80000) |
						  (context->state[6] & 0x0007F000) |
						  (context->state[5] & 0x00000FC0)) >> 6;

	context->state[2] +=  (context->state[7] & 0x0007F000) |
						  (context->state[6] & 0x00000FC0) |
						  (context->state[5] & 0x0000003F);

	context->state[1] += ROTR((context->state[7] & 0x00000FC0) |
							  (context->state[6] & 0x0000003F) |
							  (context->state[5] & 0xFE000000), 25);

	context->state[0] += ROTR((context->state[7] & 0x0000003F) |
							  (context->state[6] & 0xFE000000) |
							  (context->state[5] & 0x01F80000), 19);

	Encode(digest, context->state, 20);

	/* Zeroize sensitive information.
	 */
	ZEND_SECURE_ZERO((unsigned char*) context, sizeof(*context));
}
/* }}} */

/* {{{ PHP_HAVAL192Final
 */
PHP_HASH_API void PHP_HAVAL192Final(unsigned char *digest, PHP_HAVAL_CTX * context)
{
	unsigned char bits[10];
	unsigned int index, padLen;

	/* Version, Passes, and Digest Length */
	bits[0] =	(PHP_HASH_HAVAL_VERSION & 0x07) |
				((context->passes & 0x07) << 3) |
				((context->output & 0x03) << 6);
	bits[1] = (context->output >> 2);

	/* Save number of bits */
	Encode(bits + 2, context->count, 8);

	/* Pad out to 118 mod 128.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7f);
	padLen = (index < 118) ? (118 - index) : (246 - index);
	PHP_HAVALUpdate(context, PADDING, padLen);

	/* Append version, passes, digest length, and message length */
	PHP_HAVALUpdate(context, bits, 10);

	/* Store state in digest */
	context->state[5] += ((context->state[7] & 0xFC000000) | (context->state[6] & 0x03E00000)) >> 21;
	context->state[4] += ((context->state[7] & 0x03E00000) | (context->state[6] & 0x001F0000)) >> 16;
	context->state[3] += ((context->state[7] & 0x001F0000) | (context->state[6] & 0x0000FC00)) >> 10;
	context->state[2] += ((context->state[7] & 0x0000FC00) | (context->state[6] & 0x000003E0)) >>  5;
	context->state[1] +=  (context->state[7] & 0x000003E0) | (context->state[6] & 0x0000001F);
	context->state[0] += ROTR((context->state[7] & 0x0000001F) | (context->state[6] & 0xFC000000), 26);
	Encode(digest, context->state, 24);

	/* Zeroize sensitive information.
	 */
	ZEND_SECURE_ZERO((unsigned char*) context, sizeof(*context));
}
/* }}} */

/* {{{ PHP_HAVAL224Final
 */
PHP_HASH_API void PHP_HAVAL224Final(unsigned char *digest, PHP_HAVAL_CTX * context)
{
	unsigned char bits[10];
	unsigned int index, padLen;

	/* Version, Passes, and Digest Length */
	bits[0] =	(PHP_HASH_HAVAL_VERSION & 0x07) |
				((context->passes & 0x07) << 3) |
				((context->output & 0x03) << 6);
	bits[1] = (context->output >> 2);

	/* Save number of bits */
	Encode(bits + 2, context->count, 8);

	/* Pad out to 118 mod 128.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7f);
	padLen = (index < 118) ? (118 - index) : (246 - index);
	PHP_HAVALUpdate(context, PADDING, padLen);

	/* Append version, passes, digest length, and message length */
	PHP_HAVALUpdate(context, bits, 10);

	/* Store state in digest */
	context->state[6] +=  context->state[7]        & 0x0000000F;
	context->state[5] += (context->state[7] >>  4) & 0x0000001F;
	context->state[4] += (context->state[7] >>  9) & 0x0000000F;
	context->state[3] += (context->state[7] >> 13) & 0x0000001F;
	context->state[2] += (context->state[7] >> 18) & 0x0000000F;
	context->state[1] += (context->state[7] >> 22) & 0x0000001F;
	context->state[0] += (context->state[7] >> 27) & 0x0000001F;
	Encode(digest, context->state, 28);

	/* Zeroize sensitive information.
	 */
	ZEND_SECURE_ZERO((unsigned char*) context, sizeof(*context));
}
/* }}} */

/* {{{ PHP_HAVAL256Final
 */
PHP_HASH_API void PHP_HAVAL256Final(unsigned char *digest, PHP_HAVAL_CTX * context)
{
	unsigned char bits[10];
	unsigned int index, padLen;

	/* Version, Passes, and Digest Length */
	bits[0] =	(PHP_HASH_HAVAL_VERSION & 0x07) |
				((context->passes & 0x07) << 3) |
				((context->output & 0x03) << 6);
	bits[1] = (context->output >> 2);

	/* Save number of bits */
	Encode(bits + 2, context->count, 8);

	/* Pad out to 118 mod 128.
	 */
	index = (unsigned int) ((context->count[0] >> 3) & 0x7f);
	padLen = (index < 118) ? (118 - index) : (246 - index);
	PHP_HAVALUpdate(context, PADDING, padLen);

	/* Append version, passes, digest length, and message length */
	PHP_HAVALUpdate(context, bits, 10);

	/* Store state in digest */
	Encode(digest, context->state, 32);

	/* Zeroize sensitive information.
	 */
	ZEND_SECURE_ZERO((unsigned char*) context, sizeof(*context));
}
/* }}} */
