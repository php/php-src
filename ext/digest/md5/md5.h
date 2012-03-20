/*
  Copyright (C) 1999 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321.
  It is derived directly from the text of the RFC and not from the
  reference implementation.

  The original and principal author of md5.h is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5);
	added conditionalization for C++ compilation from Martin
	Purschke <purschke@bnl.gov>.
  1999-05-03 lpd Original version.
 */

/* $OrigId: md5.h,v 1.2 2001/03/26 08:57:14 matz Exp $ */
/* $RoughId: md5.h,v 1.3 2002/02/24 08:14:31 knu Exp $ */
/* $Id$ */

#ifndef MD5_INCLUDED
#  define MD5_INCLUDED

#include "defs.h"

/*
 * This code has some adaptations for the Ghostscript environment, but it
 * will compile and run correctly in any environment with 8-bit chars and
 * 32-bit ints.  Specifically, it assumes that if the following are
 * defined, they have the same meaning as in Ghostscript: P1, P2, P3.
 */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    uint32_t count[2];	/* message length in bits, lsw first */
    uint32_t state[4];	/* digest buffer */
    uint8_t buffer[64];	/* accumulate block */
} MD5_CTX;

#ifdef RUBY
/* avoid name clash */
#define MD5_Init	rb_Digest_MD5_Init
#define MD5_Update	rb_Digest_MD5_Update
#define MD5_Finish	rb_Digest_MD5_Finish
#endif

void	MD5_Init _((MD5_CTX *pms));
void	MD5_Update _((MD5_CTX *pms, const uint8_t *data, size_t nbytes));
void	MD5_Finish _((MD5_CTX *pms, uint8_t *digest));

#define MD5_BLOCK_LENGTH		64
#define MD5_DIGEST_LENGTH		16
#define MD5_DIGEST_STRING_LENGTH	(MD5_DIGEST_LENGTH * 2 + 1)

#endif /* MD5_INCLUDED */
