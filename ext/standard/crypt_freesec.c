/*
 * This version is derived from the original implementation of FreeSec
 * (release 1.1) by David Burren.  I've reviewed the changes made in
 * OpenBSD (as of 2.7) and modified the original code in a similar way
 * where applicable.  I've also made it reentrant and made a number of
 * other changes.
 * - Solar Designer <solar at openwall.com>
 */

/*
 * FreeSec: libcrypt for NetBSD
 *
 * Copyright (c) 1994 David Burren
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of other contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$Owl: Owl/packages/glibc/crypt_freesec.c,v 1.4 2005/11/16 13:08:32 solar Exp $
 *
 * This is an original implementation of the DES and the crypt(3) interfaces
 * by David Burren <davidb at werj.com.au>.
 *
 * An excellent reference on the underlying algorithm (and related
 * algorithms) is:
 *
 *	B. Schneier, Applied Cryptography: protocols, algorithms,
 *	and source code in C, John Wiley & Sons, 1994.
 *
 * Note that in that book's description of DES the lookups for the initial,
 * pbox, and final permutations are inverted (this has been brought to the
 * attention of the author).  A list of errata for this book has been
 * posted to the sci.crypt newsgroup by the author and is available for FTP.
 *
 * ARCHITECTURE ASSUMPTIONS:
 *	This code used to have some nasty ones, but these have been removed
 *	by now.  The code requires a 32-bit integer type, though.
 */

#include <sys/types.h>
#include <string.h>

#ifdef TEST
#include <stdio.h>
#endif

#include "crypt_freesec.h"

#define _PASSWORD_EFMT1 '_'

static const uint8_t IP[64] = {
	58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
};

static const uint8_t key_perm[56] = {
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};

static const uint8_t key_shifts[16] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

static const uint8_t comp_perm[48] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

/*
 *	No E box is used, as it's replaced by some ANDs, shifts, and ORs.
 */

static const uint8_t sbox[8][64] = {
	{
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	},
	{
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	},
	{
		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	},
	{
		 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	},
	{
		 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	},
	{
		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
	},
	{
		 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
	},
	{
		13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	}
};

static const uint8_t pbox[32] = {
	16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
	 2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
};

static const uint32_t bits32[32] =
{
	0x80000000, 0x40000000, 0x20000000, 0x10000000,
	0x08000000, 0x04000000, 0x02000000, 0x01000000,
	0x00800000, 0x00400000, 0x00200000, 0x00100000,
	0x00080000, 0x00040000, 0x00020000, 0x00010000,
	0x00008000, 0x00004000, 0x00002000, 0x00001000,
	0x00000800, 0x00000400, 0x00000200, 0x00000100,
	0x00000080, 0x00000040, 0x00000020, 0x00000010,
	0x00000008, 0x00000004, 0x00000002, 0x00000001
};

static const uint8_t bits8[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

static const unsigned char	ascii64[] =
	 "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static uint8_t m_sbox[4][4096];
static uint32_t psbox[4][256];
static uint32_t ip_maskl[8][256], ip_maskr[8][256];
static uint32_t fp_maskl[8][256], fp_maskr[8][256];
static uint32_t key_perm_maskl[8][128], key_perm_maskr[8][128];
static uint32_t comp_maskl[8][128], comp_maskr[8][128];

static inline int
ascii_to_bin(char ch)
{
	signed char sch = ch;
	int retval;

	retval = sch - '.';
	if (sch >= 'A') {
		retval = sch - ('A' - 12);
		if (sch >= 'a')
			retval = sch - ('a' - 38);
	}
	retval &= 0x3f;

	return(retval);
}

/*
 * When we choose to "support" invalid salts, nevertheless disallow those
 * containing characters that would violate the passwd file format.
 */
static inline int
ascii_is_unsafe(char ch)
{
	return !ch || ch == '\n' || ch == ':';
}

void
_crypt_extended_init(void)
{
	int i, j, b, k, inbit, obit;
	uint32_t *p, *il, *ir, *fl, *fr;
	const uint32_t *bits28, *bits24;
	uint8_t inv_key_perm[64];
	uint8_t inv_comp_perm[56];
	uint8_t init_perm[64], final_perm[64];
	uint8_t u_sbox[8][64];
	uint8_t un_pbox[32];

	bits24 = (bits28 = bits32 + 4) + 4;

	/*
	 * Invert the S-boxes, reordering the input bits.
	 */
	for (i = 0; i < 8; i++)
		for (j = 0; j < 64; j++) {
			b = (j & 0x20) | ((j & 1) << 4) | ((j >> 1) & 0xf);
			u_sbox[i][j] = sbox[i][b];
		}

	/*
	 * Convert the inverted S-boxes into 4 arrays of 8 bits.
	 * Each will handle 12 bits of the S-box input.
	 */
	for (b = 0; b < 4; b++)
		for (i = 0; i < 64; i++)
			for (j = 0; j < 64; j++)
				m_sbox[b][(i << 6) | j] =
					(u_sbox[(b << 1)][i] << 4) |
					u_sbox[(b << 1) + 1][j];

	/*
	 * Set up the initial & final permutations into a useful form, and
	 * initialise the inverted key permutation.
	 */
	for (i = 0; i < 64; i++) {
		init_perm[final_perm[i] = IP[i] - 1] = i;
		inv_key_perm[i] = 255;
	}

	/*
	 * Invert the key permutation and initialise the inverted key
	 * compression permutation.
	 */
	for (i = 0; i < 56; i++) {
		inv_key_perm[key_perm[i] - 1] = i;
		inv_comp_perm[i] = 255;
	}

	/*
	 * Invert the key compression permutation.
	 */
	for (i = 0; i < 48; i++) {
		inv_comp_perm[comp_perm[i] - 1] = i;
	}

	/*
	 * Set up the OR-mask arrays for the initial and final permutations,
	 * and for the key initial and compression permutations.
	 */
	for (k = 0; k < 8; k++) {
		for (i = 0; i < 256; i++) {
			*(il = &ip_maskl[k][i]) = 0;
			*(ir = &ip_maskr[k][i]) = 0;
			*(fl = &fp_maskl[k][i]) = 0;
			*(fr = &fp_maskr[k][i]) = 0;
			for (j = 0; j < 8; j++) {
				inbit = 8 * k + j;
				if (i & bits8[j]) {
					if ((obit = init_perm[inbit]) < 32)
						*il |= bits32[obit];
					else
						*ir |= bits32[obit-32];
					if ((obit = final_perm[inbit]) < 32)
						*fl |= bits32[obit];
					else
						*fr |= bits32[obit - 32];
				}
			}
		}
		for (i = 0; i < 128; i++) {
			*(il = &key_perm_maskl[k][i]) = 0;
			*(ir = &key_perm_maskr[k][i]) = 0;
			for (j = 0; j < 7; j++) {
				inbit = 8 * k + j;
				if (i & bits8[j + 1]) {
					if ((obit = inv_key_perm[inbit]) == 255)
						continue;
					if (obit < 28)
						*il |= bits28[obit];
					else
						*ir |= bits28[obit - 28];
				}
			}
			*(il = &comp_maskl[k][i]) = 0;
			*(ir = &comp_maskr[k][i]) = 0;
			for (j = 0; j < 7; j++) {
				inbit = 7 * k + j;
				if (i & bits8[j + 1]) {
					if ((obit=inv_comp_perm[inbit]) == 255)
						continue;
					if (obit < 24)
						*il |= bits24[obit];
					else
						*ir |= bits24[obit - 24];
				}
			}
		}
	}

	/*
	 * Invert the P-box permutation, and convert into OR-masks for
	 * handling the output of the S-box arrays setup above.
	 */
	for (i = 0; i < 32; i++)
		un_pbox[pbox[i] - 1] = i;

	for (b = 0; b < 4; b++)
		for (i = 0; i < 256; i++) {
			*(p = &psbox[b][i]) = 0;
			for (j = 0; j < 8; j++) {
				if (i & bits8[j])
					*p |= bits32[un_pbox[8 * b + j]];
			}
		}
}

static void
des_init_local(struct php_crypt_extended_data *data)
{
	data->old_rawkey0 = data->old_rawkey1 = 0;
	data->saltbits = 0;
	data->old_salt = 0;

	data->initialized = 1;
}

static void
setup_salt(uint32_t salt, struct php_crypt_extended_data *data)
{
	uint32_t	obit, saltbit, saltbits;
	int	i;

	if (salt == data->old_salt)
		return;
	data->old_salt = salt;

	saltbits = 0;
	saltbit = 1;
	obit = 0x800000;
	for (i = 0; i < 24; i++) {
		if (salt & saltbit)
			saltbits |= obit;
		saltbit <<= 1;
		obit >>= 1;
	}
	data->saltbits = saltbits;
}

static int
des_setkey(const char *key, struct php_crypt_extended_data *data)
{
	uint32_t	k0, k1, rawkey0, rawkey1;
	int	shifts, round;

	rawkey0 =
		(uint32_t)(uint8_t)key[3] |
		((uint32_t)(uint8_t)key[2] << 8) |
		((uint32_t)(uint8_t)key[1] << 16) |
		((uint32_t)(uint8_t)key[0] << 24);
	rawkey1 =
		(uint32_t)(uint8_t)key[7] |
		((uint32_t)(uint8_t)key[6] << 8) |
		((uint32_t)(uint8_t)key[5] << 16) |
		((uint32_t)(uint8_t)key[4] << 24);

	if ((rawkey0 | rawkey1)
	    && rawkey0 == data->old_rawkey0
	    && rawkey1 == data->old_rawkey1) {
		/*
		 * Already setup for this key.
		 * This optimisation fails on a zero key (which is weak and
		 * has bad parity anyway) in order to simplify the starting
		 * conditions.
		 */
		return(0);
	}
	data->old_rawkey0 = rawkey0;
	data->old_rawkey1 = rawkey1;

	/*
	 *	Do key permutation and split into two 28-bit subkeys.
	 */
	k0 = key_perm_maskl[0][rawkey0 >> 25]
	   | key_perm_maskl[1][(rawkey0 >> 17) & 0x7f]
	   | key_perm_maskl[2][(rawkey0 >> 9) & 0x7f]
	   | key_perm_maskl[3][(rawkey0 >> 1) & 0x7f]
	   | key_perm_maskl[4][rawkey1 >> 25]
	   | key_perm_maskl[5][(rawkey1 >> 17) & 0x7f]
	   | key_perm_maskl[6][(rawkey1 >> 9) & 0x7f]
	   | key_perm_maskl[7][(rawkey1 >> 1) & 0x7f];
	k1 = key_perm_maskr[0][rawkey0 >> 25]
	   | key_perm_maskr[1][(rawkey0 >> 17) & 0x7f]
	   | key_perm_maskr[2][(rawkey0 >> 9) & 0x7f]
	   | key_perm_maskr[3][(rawkey0 >> 1) & 0x7f]
	   | key_perm_maskr[4][rawkey1 >> 25]
	   | key_perm_maskr[5][(rawkey1 >> 17) & 0x7f]
	   | key_perm_maskr[6][(rawkey1 >> 9) & 0x7f]
	   | key_perm_maskr[7][(rawkey1 >> 1) & 0x7f];
	/*
	 *	Rotate subkeys and do compression permutation.
	 */
	shifts = 0;
	for (round = 0; round < 16; round++) {
		uint32_t	t0, t1;

		shifts += key_shifts[round];

		t0 = (k0 << shifts) | (k0 >> (28 - shifts));
		t1 = (k1 << shifts) | (k1 >> (28 - shifts));

		data->de_keysl[15 - round] =
		data->en_keysl[round] = comp_maskl[0][(t0 >> 21) & 0x7f]
				| comp_maskl[1][(t0 >> 14) & 0x7f]
				| comp_maskl[2][(t0 >> 7) & 0x7f]
				| comp_maskl[3][t0 & 0x7f]
				| comp_maskl[4][(t1 >> 21) & 0x7f]
				| comp_maskl[5][(t1 >> 14) & 0x7f]
				| comp_maskl[6][(t1 >> 7) & 0x7f]
				| comp_maskl[7][t1 & 0x7f];

		data->de_keysr[15 - round] =
		data->en_keysr[round] = comp_maskr[0][(t0 >> 21) & 0x7f]
				| comp_maskr[1][(t0 >> 14) & 0x7f]
				| comp_maskr[2][(t0 >> 7) & 0x7f]
				| comp_maskr[3][t0 & 0x7f]
				| comp_maskr[4][(t1 >> 21) & 0x7f]
				| comp_maskr[5][(t1 >> 14) & 0x7f]
				| comp_maskr[6][(t1 >> 7) & 0x7f]
				| comp_maskr[7][t1 & 0x7f];
	}
	return(0);
}

static int
do_des(uint32_t l_in, uint32_t r_in, uint32_t *l_out, uint32_t *r_out,
	int count, struct php_crypt_extended_data *data)
{
	/*
	 *	l_in, r_in, l_out, and r_out are in pseudo-"big-endian" format.
	 */
	uint32_t	l, r, *kl, *kr, *kl1, *kr1;
	uint32_t	f, r48l, r48r, saltbits;
	int	round;

	if (count == 0) {
		return(1);
	} else if (count > 0) {
		/*
		 * Encrypting
		 */
		kl1 = data->en_keysl;
		kr1 = data->en_keysr;
	} else {
		/*
		 * Decrypting
		 */
		count = -count;
		kl1 = data->de_keysl;
		kr1 = data->de_keysr;
	}

	/*
	 *	Do initial permutation (IP).
	 */
	l = ip_maskl[0][l_in >> 24]
	  | ip_maskl[1][(l_in >> 16) & 0xff]
	  | ip_maskl[2][(l_in >> 8) & 0xff]
	  | ip_maskl[3][l_in & 0xff]
	  | ip_maskl[4][r_in >> 24]
	  | ip_maskl[5][(r_in >> 16) & 0xff]
	  | ip_maskl[6][(r_in >> 8) & 0xff]
	  | ip_maskl[7][r_in & 0xff];
	r = ip_maskr[0][l_in >> 24]
	  | ip_maskr[1][(l_in >> 16) & 0xff]
	  | ip_maskr[2][(l_in >> 8) & 0xff]
	  | ip_maskr[3][l_in & 0xff]
	  | ip_maskr[4][r_in >> 24]
	  | ip_maskr[5][(r_in >> 16) & 0xff]
	  | ip_maskr[6][(r_in >> 8) & 0xff]
	  | ip_maskr[7][r_in & 0xff];

	saltbits = data->saltbits;
	while (count--) {
		/*
		 * Do each round.
		 */
		kl = kl1;
		kr = kr1;
		round = 16;
		while (round--) {
			/*
			 * Expand R to 48 bits (simulate the E-box).
			 */
			r48l	= ((r & 0x00000001) << 23)
				| ((r & 0xf8000000) >> 9)
				| ((r & 0x1f800000) >> 11)
				| ((r & 0x01f80000) >> 13)
				| ((r & 0x001f8000) >> 15);

			r48r	= ((r & 0x0001f800) << 7)
				| ((r & 0x00001f80) << 5)
				| ((r & 0x000001f8) << 3)
				| ((r & 0x0000001f) << 1)
				| ((r & 0x80000000) >> 31);
			/*
			 * Do salting for crypt() and friends, and
			 * XOR with the permuted key.
			 */
			f = (r48l ^ r48r) & saltbits;
			r48l ^= f ^ *kl++;
			r48r ^= f ^ *kr++;
			/*
			 * Do sbox lookups (which shrink it back to 32 bits)
			 * and do the pbox permutation at the same time.
			 */
			f = psbox[0][m_sbox[0][r48l >> 12]]
			  | psbox[1][m_sbox[1][r48l & 0xfff]]
			  | psbox[2][m_sbox[2][r48r >> 12]]
			  | psbox[3][m_sbox[3][r48r & 0xfff]];
			/*
			 * Now that we've permuted things, complete f().
			 */
			f ^= l;
			l = r;
			r = f;
		}
		r = l;
		l = f;
	}
	/*
	 * Do final permutation (inverse of IP).
	 */
	*l_out	= fp_maskl[0][l >> 24]
		| fp_maskl[1][(l >> 16) & 0xff]
		| fp_maskl[2][(l >> 8) & 0xff]
		| fp_maskl[3][l & 0xff]
		| fp_maskl[4][r >> 24]
		| fp_maskl[5][(r >> 16) & 0xff]
		| fp_maskl[6][(r >> 8) & 0xff]
		| fp_maskl[7][r & 0xff];
	*r_out	= fp_maskr[0][l >> 24]
		| fp_maskr[1][(l >> 16) & 0xff]
		| fp_maskr[2][(l >> 8) & 0xff]
		| fp_maskr[3][l & 0xff]
		| fp_maskr[4][r >> 24]
		| fp_maskr[5][(r >> 16) & 0xff]
		| fp_maskr[6][(r >> 8) & 0xff]
		| fp_maskr[7][r & 0xff];
	return(0);
}

static int
des_cipher(const char *in, char *out, uint32_t salt, int count,
	struct php_crypt_extended_data *data)
{
	uint32_t	l_out = 0, r_out = 0, rawl, rawr;
	int	retval;

	setup_salt(salt, data);

	rawl =
		(uint32_t)(uint8_t)in[3] |
		((uint32_t)(uint8_t)in[2] << 8) |
		((uint32_t)(uint8_t)in[1] << 16) |
		((uint32_t)(uint8_t)in[0] << 24);
	rawr =
		(uint32_t)(uint8_t)in[7] |
		((uint32_t)(uint8_t)in[6] << 8) |
		((uint32_t)(uint8_t)in[5] << 16) |
		((uint32_t)(uint8_t)in[4] << 24);

	retval = do_des(rawl, rawr, &l_out, &r_out, count, data);

	out[0] = l_out >> 24;
	out[1] = l_out >> 16;
	out[2] = l_out >> 8;
	out[3] = l_out;
	out[4] = r_out >> 24;
	out[5] = r_out >> 16;
	out[6] = r_out >> 8;
	out[7] = r_out;

	return(retval);
}

char *
_crypt_extended_r(const unsigned char *key, const char *setting,
	struct php_crypt_extended_data *data)
{
	int		i;
	uint32_t	count, salt, l, r0, r1, keybuf[2];
	uint8_t		*p, *q;

	if (!data->initialized)
		des_init_local(data);

	/*
	 * Copy the key, shifting each character up by one bit
	 * and padding with zeros.
	 */
	q = (uint8_t *) keybuf;
	while ((size_t)(q - (uint8_t *) keybuf) < sizeof(keybuf)) {
		*q++ = *key << 1;
		if (*key)
			key++;
	}
	if (des_setkey((char *) keybuf, data))
		return(NULL);

	if (*setting == _PASSWORD_EFMT1) {
		/*
		 * "new"-style:
		 *	setting - underscore, 4 chars of count, 4 chars of salt
		 *	key - unlimited characters
		 */
		for (i = 1, count = 0; i < 5; i++) {
			int value = ascii_to_bin(setting[i]);
			if (ascii64[value] != setting[i])
				return(NULL);
			count |= value << (i - 1) * 6;
		}
		if (!count)
			return(NULL);

		for (i = 5, salt = 0; i < 9; i++) {
			int value = ascii_to_bin(setting[i]);
			if (ascii64[value] != setting[i])
				return(NULL);
			salt |= value << (i - 5) * 6;
		}

		while (*key) {
			/*
			 * Encrypt the key with itself.
			 */
			if (des_cipher((char *) keybuf, (char *) keybuf,
			    0, 1, data))
				return(NULL);
			/*
			 * And XOR with the next 8 characters of the key.
			 */
			q = (uint8_t *) keybuf;
			while ((size_t)(q - (uint8_t *) keybuf) < sizeof(keybuf) && *key)
				*q++ ^= *key++ << 1;

			if (des_setkey((char *) keybuf, data))
				return(NULL);
		}
		memcpy(data->output, setting, 9);
		data->output[9] = '\0';
		p = (uint8_t *) data->output + 9;
	} else {
		/*
		 * "old"-style:
		 *	setting - 2 chars of salt
		 *	key - up to 8 characters
		 */
		count = 25;

		if (ascii_is_unsafe(setting[0]) || ascii_is_unsafe(setting[1]))
			return(NULL);

		salt = (ascii_to_bin(setting[1]) << 6)
		     |  ascii_to_bin(setting[0]);

		data->output[0] = setting[0];
		data->output[1] = setting[1];
		p = (uint8_t *) data->output + 2;
	}
	setup_salt(salt, data);
	/*
	 * Do it.
	 */
	if (do_des(0, 0, &r0, &r1, count, data))
		return(NULL);
	/*
	 * Now encode the result...
	 */
	l = (r0 >> 8);
	*p++ = ascii64[(l >> 18) & 0x3f];
	*p++ = ascii64[(l >> 12) & 0x3f];
	*p++ = ascii64[(l >> 6) & 0x3f];
	*p++ = ascii64[l & 0x3f];

	l = (r0 << 16) | ((r1 >> 16) & 0xffff);
	*p++ = ascii64[(l >> 18) & 0x3f];
	*p++ = ascii64[(l >> 12) & 0x3f];
	*p++ = ascii64[(l >> 6) & 0x3f];
	*p++ = ascii64[l & 0x3f];

	l = r1 << 2;
	*p++ = ascii64[(l >> 12) & 0x3f];
	*p++ = ascii64[(l >> 6) & 0x3f];
	*p++ = ascii64[l & 0x3f];
	*p = 0;

	return(data->output);
}

#ifdef TEST
static char *
_crypt_extended(const char *key, const char *setting)
{
	static int initialized = 0;
	static struct php_crypt_extended_data data;

	if (!initialized) {
		_crypt_extended_init();
		initialized = 1;
		data.initialized = 0;
	}
	return _crypt_extended_r(key, setting, &data);
}

#define crypt _crypt_extended

static const struct {
	const char *hash;
	const char *pw;
} tests[] = {
/* "new"-style */
	{"_J9..CCCCXBrJUJV154M", "U*U*U*U*"},
	{"_J9..CCCCXUhOBTXzaiE", "U*U***U"},
	{"_J9..CCCC4gQ.mB/PffM", "U*U***U*"},
	{"_J9..XXXXvlzQGqpPPdk", "*U*U*U*U"},
	{"_J9..XXXXsqM/YSSP..Y", "*U*U*U*U*"},
	{"_J9..XXXXVL7qJCnku0I", "*U*U*U*U*U*U*U*U"},
	{"_J9..XXXXAj8cFbP5scI", "*U*U*U*U*U*U*U*U*"},
	{"_J9..SDizh.vll5VED9g", "ab1234567"},
	{"_J9..SDizRjWQ/zePPHc", "cr1234567"},
	{"_J9..SDizxmRI1GjnQuE", "zxyDPWgydbQjgq"},
	{"_K9..SaltNrQgIYUAeoY", "726 even"},
	{"_J9..SDSD5YGyRCr4W4c", ""},
/* "old"-style, valid salts */
	{"CCNf8Sbh3HDfQ", "U*U*U*U*"},
	{"CCX.K.MFy4Ois", "U*U***U"},
	{"CC4rMpbg9AMZ.", "U*U***U*"},
	{"XXxzOu6maQKqQ", "*U*U*U*U"},
	{"SDbsugeBiC58A", ""},
	{"./xZjzHv5vzVE", "password"},
	{"0A2hXM1rXbYgo", "password"},
	{"A9RXdR23Y.cY6", "password"},
	{"ZziFATVXHo2.6", "password"},
	{"zZDDIZ0NOlPzw", "password"},
/* "old"-style, "reasonable" invalid salts, UFC-crypt behavior expected */
	{"\001\002wyd0KZo65Jo", "password"},
	{"a_C10Dk/ExaG.", "password"},
	{"~\377.5OTsRVjwLo", "password"},
/* The below are erroneous inputs, so NULL return is expected/required */
	{"", ""}, /* no salt */
	{" ", ""}, /* setting string is too short */
	{"a:", ""}, /* unsafe character */
	{"\na", ""}, /* unsafe character */
	{"_/......", ""}, /* setting string is too short for its type */
	{"_........", ""}, /* zero iteration count */
	{"_/!......", ""}, /* invalid character in count */
	{"_/......!", ""}, /* invalid character in salt */
	{NULL}
};

int main(void)
{
	int i;

	for (i = 0; tests[i].hash; i++) {
		char *hash = crypt(tests[i].pw, tests[i].hash);
		if (!hash && strlen(tests[i].hash) < 13)
			continue; /* expected failure */
		if (!strcmp(hash, tests[i].hash))
			continue; /* expected success */
		puts("FAILED");
		return 1;
	}

	puts("PASSED");

	return 0;
}
#endif
