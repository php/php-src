/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/*
	Based on CPANs "Text-Metaphone-1.96" by Michael G Schwern <schwern@pobox.com> 
*/

#include "php.h"
#include "php_metaphone.h"

static int metaphone(char *word, int word_len, int max_phonemes, char **phoned_word, int traditional);

PHP_FUNCTION(metaphone);

/* {{{ proto string metaphone(string text, int phones)
   Break english phrases down into their phonemes */
PHP_FUNCTION(metaphone)
{
	char *str;
	char *result = 0;
	int phones = 0, str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &str_len,
							  &phones) == FAILURE) {
		return;
	}

	if (metaphone(str, str_len, phones, &result, 1) == 0) {
		RETVAL_STRING(result, 0);
	} else {
		if (result) {
			efree(result);
		}
		RETURN_FALSE;
	}
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
char _codes[26] =
{
	1, 16, 4, 16, 9, 2, 4, 16, 9, 2, 0, 2, 2, 2, 1, 4, 0, 2, 4, 4, 1, 0, 0, 0, 8, 0
/*  a  b c  d e f g  h i j k l m n o p q r s t u v w x y z */
};


#define ENCODE(c) (isalpha(c) ? _codes[((toupper(c)) - 'A')] : 0)

#define isvowel(c)  (ENCODE(c) & 1)		/* AEIOU */

/* These letters are passed through unchanged */
#define NOCHANGE(c) (ENCODE(c) & 2)		/* FJMNR */

/* These form dipthongs when preceding H */
#define AFFECTH(c)  (ENCODE(c) & 4)		/* CGPST */

/* These make C and G soft */
#define MAKESOFT(c) (ENCODE(c) & 8)		/* EIY */

/* These prevent GH from becoming F */
#define NOGHTOF(c)  (ENCODE(c) & 16)	/* BDH */

/*----------------------------- */
/* end of "metachar.h"          */
/*----------------------------- */

/* I suppose I could have been using a character pointer instead of
 * accesssing the array directly... */

/* Look at the next letter in the word */
#define Next_Letter (toupper(word[w_idx+1]))
/* Look at the current letter in the word */
#define Curr_Letter (toupper(word[w_idx]))
/* Go N letters back. */
#define Look_Back_Letter(n)	(w_idx >= n ? toupper(word[w_idx-n]) : '\0')
/* Previous letter.  I dunno, should this return null on failure? */
#define Prev_Letter (Look_Back_Letter(1))
/* Look two letters down.  It makes sure you don't walk off the string. */
#define After_Next_Letter	(Next_Letter != '\0' ? toupper(word[w_idx+2]) \
											     : '\0')
#define Look_Ahead_Letter(n) (toupper(Lookahead(word+w_idx, n)))


/* Allows us to safely look ahead an arbitrary # of letters */
/* I probably could have just used strlen... */
static char Lookahead(char *word, int how_far)
{
	char letter_ahead = '\0';	/* null by default */
	int idx;
	for (idx = 0; word[idx] != '\0' && idx < how_far; idx++);
	/* Edge forward in the string... */

	letter_ahead = word[idx];	/* idx will be either == to how_far or
								 * at the end of the string
								 */
	return letter_ahead;
}


/* phonize one letter
 * We don't know the buffers size in advance. On way to solve this is to just
 * re-allocate the buffer size. We're using an extra of 2 characters (this
 * could be one though; or more too). */
#define Phonize(c)	{ \
						if (p_idx >= max_buffer_len) { \
							*phoned_word = erealloc(*phoned_word, max_buffer_len + 2); \
							max_buffer_len += 2; \
						} \
						(*phoned_word)[p_idx++] = c; \
					}
/* Slap a null character on the end of the phoned word */
#define End_Phoned_Word	{(*phoned_word)[p_idx] = '\0';}
/* How long is the phoned word? */
#define Phone_Len	(p_idx)

/* Note is a letter is a 'break' in the word */
#define Isbreak(c)  (!isalpha(c))

/* {{{ metaphone
 */
static int metaphone(char *word, int word_len, int max_phonemes, char **phoned_word, int traditional)
{
	int w_idx = 0;				/* point in the phonization we're at. */
	int p_idx = 0;				/* end of the phoned phrase */
	int max_buffer_len = 0;		/* maximum length of the destination buffer */

/*-- Parameter checks --*/
	/* Negative phoneme length is meaningless */

	if (max_phonemes < 0)
		return -1;

	/* Empty/null string is meaningless */
	/* Overly paranoid */
	/* assert(word != NULL && word[0] != '\0'); */

	if (word == NULL)
		return -1;

/*-- Allocate memory for our phoned_phrase --*/
	if (max_phonemes == 0) {	/* Assume largest possible */
		max_buffer_len = word_len;
		*phoned_word = emalloc(sizeof(char) * word_len + 1);
	} else {
		max_buffer_len = max_phonemes;
		*phoned_word = emalloc(sizeof(char) * max_phonemes + 1);
	}


/*-- The first phoneme has to be processed specially. --*/
	/* Find our first letter */
	for (; !isalpha(Curr_Letter); w_idx++) {
		/* On the off chance we were given nothing but crap... */
		if (Curr_Letter == '\0') {
			End_Phoned_Word
				return SUCCESS;	/* For testing */
		}
	}

	switch (Curr_Letter) {
		/* AE becomes E */
	case 'A':
		if (Next_Letter == 'E') {
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
		if (Next_Letter == 'N') {
			Phonize('N');
			w_idx += 2;
		}
		break;
		/* WH becomes H, 
		   WR becomes R 
		   W if followed by a vowel */
	case 'W':
		if (Next_Letter == 'H' ||
			Next_Letter == 'R') {
			Phonize(Next_Letter);
			w_idx += 2;
		} else if (isvowel(Next_Letter)) {
			Phonize('W');
			w_idx += 2;
		}
		/* else ignore */
		break;
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
		Phonize(Curr_Letter);
		w_idx++;
		break;
	default:
		/* do nothing */
		break;
	}



	/* On to the metaphoning */
	for (; Curr_Letter != '\0' &&
		 (max_phonemes == 0 || Phone_Len < max_phonemes);
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
		if (!isalpha(Curr_Letter))
			continue;

		/* Drop duplicates, except CC */
		if (Curr_Letter == Prev_Letter &&
			Curr_Letter != 'C')
			continue;

		switch (Curr_Letter) {
			/* B -> B unless in MB */
		case 'B':
			if (Prev_Letter != 'M')
				Phonize('B');
			break;
			/* 'sh' if -CIA- or -CH, but not SCH, except SCHW.
			 * (SCHW is handled in S)
			 *  S if -CI-, -CE- or -CY-
			 *  dropped if -SCI-, SCE-, -SCY- (handed in S)
			 *  else K
			 */
		case 'C':
			if (MAKESOFT(Next_Letter)) {	/* C[IEY] */
				if (After_Next_Letter == 'A' &&
					Next_Letter == 'I') {	/* CIA */
					Phonize(SH);
				}
				/* SC[IEY] */
				else if (Prev_Letter == 'S') {
					/* Dropped */
				} else {
					Phonize('S');
				}
			} else if (Next_Letter == 'H') {
				if ((!traditional) && (After_Next_Letter == 'R' || Prev_Letter == 'S')) {	/* Christ, School */
					Phonize('K');
				} else {
					Phonize(SH);
				}
				skip_letter++;
			} else {
				Phonize('K');
			}
			break;
			/* J if in -DGE-, -DGI- or -DGY-
			 * else T
			 */
		case 'D':
			if (Next_Letter == 'G' &&
				MAKESOFT(After_Next_Letter)) {
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
		case 'G':
			if (Next_Letter == 'H') {
				if (!(NOGHTOF(Look_Back_Letter(3)) ||
					  Look_Back_Letter(4) == 'H')) {
					Phonize('F');
					skip_letter++;
				} else {
					/* silent */
				}
			} else if (Next_Letter == 'N') {
				if (Isbreak(After_Next_Letter) ||
					(After_Next_Letter == 'E' &&
					 Look_Ahead_Letter(3) == 'D')) {
					/* dropped */
				} else
					Phonize('K');
			} else if (MAKESOFT(Next_Letter) &&
					   Prev_Letter != 'G') {
				Phonize('J');
			} else {
				Phonize('K');
			}
			break;
			/* H if before a vowel and not after C,G,P,S,T */
		case 'H':
			if (isvowel(Next_Letter) &&
				!AFFECTH(Prev_Letter))
				Phonize('H');
			break;
			/* dropped if after C
			 * else K
			 */
		case 'K':
			if (Prev_Letter != 'C')
				Phonize('K');
			break;
			/* F if before H
			 * else P
			 */
		case 'P':
			if (Next_Letter == 'H') {
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
		case 'S':
			if (Next_Letter == 'I' &&
				(After_Next_Letter == 'O' ||
				 After_Next_Letter == 'A')) {
				Phonize(SH);
			} else if (Next_Letter == 'H') {
				Phonize(SH);
				skip_letter++;
			} else if ((!traditional) && (Next_Letter == 'C' && Look_Ahead_Letter(2) == 'H' && Look_Ahead_Letter(3) == 'W')) {
				Phonize(SH);
				skip_letter += 2;
			} else {
				Phonize('S');
			}
			break;
			/* 'sh' in -TIA- or -TIO-
			 * else 'th' before H
			 * else T
			 */
		case 'T':
			if (Next_Letter == 'I' &&
				(After_Next_Letter == 'O' ||
				 After_Next_Letter == 'A')) {
				Phonize(SH);
			} else if (Next_Letter == 'H') {
				Phonize(TH);
				skip_letter++;
			} else {
				Phonize('T');
			}
			break;
			/* F */
		case 'V':
			Phonize('F');
			break;
			/* W before a vowel, else dropped */
		case 'W':
			if (isvowel(Next_Letter))
				Phonize('W');
			break;
			/* KS */
		case 'X':
			Phonize('K');
			Phonize('S');
			break;
			/* Y if followed by a vowel */
		case 'Y':
			if (isvowel(Next_Letter))
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
			Phonize(Curr_Letter);
			break;
		default:
			/* nothing */
			break;
		}						/* END SWITCH */

		w_idx += skip_letter;
	}							/* END FOR */

	End_Phoned_Word;

	return 0;
}								/* END metaphone */
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
