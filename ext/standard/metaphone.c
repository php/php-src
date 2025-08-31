/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
*/

/*
	Based on CPANs "Text-Metaphone-1.96" by Michael G Schwern <schwern@pobox.com>
*/

#include "php.h"

static void metaphone(unsigned char *word, size_t word_len, zend_long max_phonemes, zend_string **phoned_word, int traditional);

/* {{{ Break english phrases down into their phonemes */
PHP_FUNCTION(metaphone)
{
	zend_string *str;
	zend_string *result = NULL;
	zend_long phones = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(str)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(phones)
	ZEND_PARSE_PARAMETERS_END();

	if (phones < 0) {
		zend_argument_value_error(2, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	metaphone((unsigned char *)ZSTR_VAL(str), ZSTR_LEN(str), phones, &result, 1);
	RETVAL_STR(result);
}
/* }}} */

/*
   this is now the original code by Michael G Schwern:
   i've changed it just a slightly bit (use emalloc,
   get rid of includes etc)
	- thies - 13.09.1999
*/

/*-----------------------------  */
/* this used to be "metaphone.h" */
/*-----------------------------  */

/* Special encodings */
#define  SH 	'X'
#define  TH		'0'

/*-----------------------------  */
/* end of "metaphone.h"          */
/*-----------------------------  */

/*----------------------------- */
/* this used to be "metachar.h" */
/*----------------------------- */

/* Metachar.h ... little bits about characters for metaphone */
/*-- Character encoding array & accessing macros --*/
/* Stolen directly out of the book... */
static const char _codes[26] =
{
	1, 16, 4, 16, 9, 2, 4, 16, 9, 2, 0, 2, 2, 2, 1, 4, 0, 2, 4, 4, 1, 0, 0, 0, 8, 0
/*  a  b c  d e f g  h i j k l m n o p q r s t u v w x y z */
};


/* Note: these functions require an uppercase letter input! */
static zend_always_inline char encode(char c) {
	if (isalpha(c)) {
		ZEND_ASSERT(c >= 'A' && c <= 'Z');
		return _codes[(c - 'A')];
	} else {
		return 0;
	}
}

#define isvowel(c)  (encode(c) & 1)		/* AEIOU */

/* These letters are passed through unchanged */
#define NOCHANGE(c) (encode(c) & 2)		/* FJMNR */

/* These form diphthongs when preceding H */
#define AFFECTH(c)  (encode(c) & 4)		/* CGPST */

/* These make C and G soft */
#define MAKESOFT(c) (encode(c) & 8)		/* EIY */

/* These prevent GH from becoming F */
#define NOGHTOF(c)  (encode(c) & 16)	/* BDH */

/*----------------------------- */
/* end of "metachar.h"          */
/*----------------------------- */

/* I suppose I could have been using a character pointer instead of
 * accesssing the array directly... */

#define Convert_Raw(c) toupper(c)
/* Look at the next letter in the word */
#define Read_Raw_Next_Letter (word[w_idx+1])
#define Read_Next_Letter (Convert_Raw(Read_Raw_Next_Letter))
/* Look at the current letter in the word */
#define Read_Raw_Curr_Letter (word[w_idx])
#define Read_Curr_Letter (Convert_Raw(Read_Raw_Curr_Letter))
/* Go N letters back. */
#define Look_Back_Letter(n)	(w_idx >= n ? Convert_Raw(word[w_idx-n]) : '\0')
/* Previous letter.  I dunno, should this return null on failure? */
#define Read_Prev_Letter (Look_Back_Letter(1))
/* Look two letters down.  It makes sure you don't walk off the string. */
#define Read_After_Next_Letter	(Read_Raw_Next_Letter != '\0' ? Convert_Raw(word[w_idx+2]) \
											     : '\0')
#define Look_Ahead_Letter(n) (toupper(Lookahead((char *) word+w_idx, n)))


/* Allows us to safely look ahead an arbitrary # of letters */
/* I probably could have just used strlen... */
static char Lookahead(char *word, int how_far)
{
	int idx;
	for (idx = 0; word[idx] != '\0' && idx < how_far; idx++);
	/* Edge forward in the string... */

	return  word[idx];			/* idx will be either == to how_far or
								 * at the end of the string where it will be null
								 */
}


/* phonize one letter
 * We don't know the buffers size in advance. On way to solve this is to just
 * re-allocate the buffer size. We're using an extra of 2 characters (this
 * could be one though; or more too). */
#define Phonize(c)	{ \
						if (p_idx >= max_buffer_len) { \
							*phoned_word = zend_string_extend(*phoned_word, 2 * sizeof(char) + max_buffer_len, 0); \
							max_buffer_len += 2; \
						} \
						ZSTR_VAL(*phoned_word)[p_idx++] = c; \
						ZSTR_LEN(*phoned_word) = p_idx; \
					}
/* Slap a null character on the end of the phoned word */
#define End_Phoned_Word()	{ \
							if (p_idx == max_buffer_len) { \
								*phoned_word = zend_string_extend(*phoned_word, 1 * sizeof(char) + max_buffer_len, 0); \
								max_buffer_len += 1; \
							} \
							ZSTR_VAL(*phoned_word)[p_idx] = '\0'; \
							ZSTR_LEN(*phoned_word) = p_idx; \
						}
/* How long is the phoned word? */
#define Phone_Len	(p_idx)

/* Note is a letter is a 'break' in the word */
#define Isbreak(c)  (!isalpha(c))

/* {{{ metaphone */
static void metaphone(unsigned char *word, size_t word_len, zend_long max_phonemes, zend_string **phoned_word, int traditional)
{
	int w_idx = 0;				/* point in the phonization we're at. */
	size_t p_idx = 0;				/* end of the phoned phrase */
	size_t max_buffer_len = 0;		/* maximum length of the destination buffer */
	char curr_letter;
	ZEND_ASSERT(word != NULL);
	ZEND_ASSERT(max_phonemes >= 0);

/*-- Allocate memory for our phoned_phrase --*/
	if (max_phonemes == 0) {	/* Assume largest possible */
		max_buffer_len = word_len;
		*phoned_word = zend_string_alloc(sizeof(char) * word_len + 1, 0);
	} else {
		max_buffer_len = max_phonemes;
		*phoned_word = zend_string_alloc(sizeof(char) * max_phonemes + 1, 0);
	}


/*-- The first phoneme has to be processed specially. --*/
	/* Find our first letter */
	for (; !isalpha(curr_letter = Read_Raw_Curr_Letter); w_idx++) {
		/* On the off chance we were given nothing but crap... */
		if (curr_letter == '\0') {
			End_Phoned_Word();
			return;
		}
	}

	curr_letter = Convert_Raw(curr_letter);

	switch (curr_letter) {
		/* AE becomes E */
	case 'A':
		if (Read_Next_Letter == 'E') {
			Phonize('E');
			w_idx += 2;
		}
		/* Remember, preserve vowels at the beginning */
		else {
			Phonize('A');
			w_idx++;
		}
		break;
		/* [GKP]N becomes N */
	case 'G':
	case 'K':
	case 'P':
		if (Read_Next_Letter == 'N') {
			Phonize('N');
			w_idx += 2;
		}
		break;
		/* WH becomes W,
		   WR becomes R
		   W if followed by a vowel */
	case 'W': {
		char next_letter = Read_Next_Letter;
		if (next_letter == 'R') {
			Phonize('R');
			w_idx += 2;
		} else if (next_letter == 'H' || isvowel(next_letter)) {
			Phonize('W');
			w_idx += 2;
		}
		/* else ignore */
		break;
	}
		/* X becomes S */
	case 'X':
		Phonize('S');
		w_idx++;
		break;
		/* Vowels are kept */
		/* We did A already
		   case 'A':
		   case 'a':
		 */
	case 'E':
	case 'I':
	case 'O':
	case 'U':
		Phonize(curr_letter);
		w_idx++;
		break;
	default:
		/* do nothing */
		break;
	}



	/* On to the metaphoning */
	for (; (curr_letter = Read_Raw_Curr_Letter) != '\0' &&
		 (max_phonemes == 0 || Phone_Len < (size_t)max_phonemes);
		 w_idx++) {
		/* How many letters to skip because an eariler encoding handled
		 * multiple letters */
		unsigned short int skip_letter = 0;


		/* THOUGHT:  It would be nice if, rather than having things like...
		 * well, SCI.  For SCI you encode the S, then have to remember
		 * to skip the C.  So the phonome SCI invades both S and C.  It would
		 * be better, IMHO, to skip the C from the S part of the encoding.
		 * Hell, I'm trying it.
		 */

		/* Ignore non-alphas */
		if (!isalpha(curr_letter))
			continue;

		curr_letter = Convert_Raw(curr_letter);
		/* Note: we can't cache curr_letter from the previous loop
		 * because of the skip_letter variable. */
		char prev_letter = Read_Prev_Letter;

		/* Drop duplicates, except CC */
		if (curr_letter == prev_letter &&
			curr_letter != 'C')
			continue;

		switch (curr_letter) {
			/* B -> B unless in MB */
		case 'B':
			if (prev_letter != 'M')
				Phonize('B');
			break;
			/* 'sh' if -CIA- or -CH, but not SCH, except SCHW.
			 * (SCHW is handled in S)
			 *  S if -CI-, -CE- or -CY-
			 *  dropped if -SCI-, SCE-, -SCY- (handed in S)
			 *  else K
			 */
		case 'C': {
			char next_letter = Read_Next_Letter;
			if (MAKESOFT(next_letter)) {	/* C[IEY] */
				if (next_letter == 'I' && Read_After_Next_Letter == 'A') {	/* CIA */
					Phonize(SH);
				}
				/* SC[IEY] */
				else if (prev_letter == 'S') {
					/* Dropped */
				} else {
					Phonize('S');
				}
			} else if (next_letter == 'H') {
				if ((!traditional) && (prev_letter == 'S' || Read_After_Next_Letter == 'R')) {	/* Christ, School */
					Phonize('K');
				} else {
					Phonize(SH);
				}
				skip_letter++;
			} else {
				Phonize('K');
			}
			break;
		}
			/* J if in -DGE-, -DGI- or -DGY-
			 * else T
			 */
		case 'D':
			if (Read_Next_Letter == 'G' &&
				MAKESOFT(Read_After_Next_Letter)) {
				Phonize('J');
				skip_letter++;
			} else
				Phonize('T');
			break;
			/* F if in -GH and not B--GH, D--GH, -H--GH, -H---GH
			 * else dropped if -GNED, -GN,
			 * else dropped if -DGE-, -DGI- or -DGY- (handled in D)
			 * else J if in -GE-, -GI, -GY and not GG
			 * else K
			 */
		case 'G': {
			char next_letter = Read_Next_Letter;
			if (next_letter == 'H') {
				if (!(NOGHTOF(Look_Back_Letter(3)) ||
					  Look_Back_Letter(4) == 'H')) {
					Phonize('F');
					skip_letter++;
				} else {
					/* silent */
				}
			} else if (next_letter == 'N') {
				char after_next_letter = Read_After_Next_Letter;
				if (Isbreak(after_next_letter) ||
					(after_next_letter == 'E' &&
					 Look_Ahead_Letter(3) == 'D')) {
					/* dropped */
				} else
					Phonize('K');
			} else if (MAKESOFT(next_letter) &&
					   prev_letter != 'G') {
				Phonize('J');
			} else {
				Phonize('K');
			}
			break;
		}
			/* H if before a vowel and not after C,G,P,S,T */
		case 'H':
			if (isvowel(Read_Next_Letter) &&
				!AFFECTH(prev_letter))
				Phonize('H');
			break;
			/* dropped if after C
			 * else K
			 */
		case 'K':
			if (prev_letter != 'C')
				Phonize('K');
			break;
			/* F if before H
			 * else P
			 */
		case 'P':
			if (Read_Next_Letter == 'H') {
				Phonize('F');
			} else {
				Phonize('P');
			}
			break;
			/* K
			 */
		case 'Q':
			Phonize('K');
			break;
			/* 'sh' in -SH-, -SIO- or -SIA- or -SCHW-
			 * else S
			 */
		case 'S': {
			char next_letter = Read_Next_Letter;
			char after_next_letter;
			if (next_letter == 'I' &&
				((after_next_letter = Read_After_Next_Letter) == 'O' ||
				 after_next_letter == 'A')) {
				Phonize(SH);
			} else if (next_letter == 'H') {
				Phonize(SH);
				skip_letter++;
			} else if ((!traditional) && (next_letter == 'C' && Look_Ahead_Letter(2) == 'H' && Look_Ahead_Letter(3) == 'W')) {
				Phonize(SH);
				skip_letter += 2;
			} else {
				Phonize('S');
			}
			break;
		}
			/* 'sh' in -TIA- or -TIO-
			 * else 'th' before H
			 * else T
			 */
		case 'T': {
			char next_letter = Read_Next_Letter;
			char after_next_letter;
			if (next_letter == 'I' &&
				((after_next_letter = Read_After_Next_Letter) == 'O' ||
				 after_next_letter == 'A')) {
				Phonize(SH);
			} else if (next_letter == 'H') {
				Phonize(TH);
				skip_letter++;
			} else if (!(next_letter == 'C' && Read_After_Next_Letter == 'H')) {
				Phonize('T');
			}
			break;
		}
			/* F */
		case 'V':
			Phonize('F');
			break;
			/* W before a vowel, else dropped */
		case 'W':
			if (isvowel(Read_Next_Letter))
				Phonize('W');
			break;
			/* KS */
		case 'X':
			Phonize('K');
			Phonize('S');
			break;
			/* Y if followed by a vowel */
		case 'Y':
			if (isvowel(Read_Next_Letter))
				Phonize('Y');
			break;
			/* S */
		case 'Z':
			Phonize('S');
			break;
			/* No transformation */
		case 'F':
		case 'J':
		case 'L':
		case 'M':
		case 'N':
		case 'R':
			Phonize(curr_letter);
			break;
		default:
			/* nothing */
			break;
		}						/* END SWITCH */

		w_idx += skip_letter;
	}							/* END FOR */

	End_Phoned_Word();
}								/* END metaphone */
/* }}} */
