/* $Id$ */

#ifndef _CRYPT_FREESEC_H
#define _CRYPT_FREESEC_H

#if PHP_WIN32
# include "win32/php_stdint.h"
# ifndef inline
# define inline __inline
# endif
#else
# include "php_config.h"
# if HAVE_INTTYPES_H
#  include <inttypes.h>
# elif HAVE_STDINT_H
#  include <stdint.h>
# endif
# ifndef HAVE_UINT32_T
#  if SIZEOF_INT == 4
typedef unsigned int uint32_t;
#  elif SIZEOF_LONG == 4
typedef unsigned long int uint32_t;
#  endif
# endif
#endif

#define MD5_HASH_MAX_LEN 120

struct php_crypt_extended_data {
	int initialized;
	uint32_t saltbits;
	uint32_t old_salt;
	uint32_t en_keysl[16], en_keysr[16];
	uint32_t de_keysl[16], de_keysr[16];
	uint32_t old_rawkey0, old_rawkey1;
	char output[21];
};

/*
 * _crypt_extended_init() must be called explicitly before first use of
 * _crypt_extended_r().
 */

void _crypt_extended_init(void);

char *_crypt_extended_r(const char *key, const char *setting,
	struct php_crypt_extended_data *data);

#endif
