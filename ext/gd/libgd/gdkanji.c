
/* gdkanji.c (Kanji code converter)                            */
/*                 written by Masahito Yamaga (ma@yama-ga.com) */

#include "gd.h"
#include "gd_errors.h"
#include "gdhelpers.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(HAVE_ICONV_H) || defined(HAVE_ICONV)
#include <errno.h>
#include <iconv.h>
#endif

#if defined(HAVE_ICONV_H) && !defined(HAVE_ICONV)
#define HAVE_ICONV 1
#endif
#ifndef HAVE_ICONV_T_DEF
typedef void *iconv_t;
#endif

#ifndef HAVE_ICONV
#define ICONV_CONST /**/
iconv_t iconv_open(const char *, const char *);
size_t iconv(iconv_t, ICONV_CONST char **, size_t *, char **, size_t *);
int iconv_close(iconv_t);

iconv_t iconv_open(const char *tocode, const char *fromcode)
{
    (void)tocode;
    (void)fromcode;
    return (iconv_t)(-1);
}

size_t iconv(iconv_t cd, ICONV_CONST char **inbuf, size_t *inbytesleft, char **outbuf,
             size_t *outbytesleft)
{
    (void)cd;
    (void)inbuf;
    (void)inbytesleft;
    (void)outbuf;
    (void)outbytesleft;
    return 0;
}

int iconv_close(iconv_t cd)
{
    (void)cd;
    return 0;
}

#endif /* !HAVE_ICONV */

#define LIBNAME "any2eucjp()"

#if defined(__MSC__) || defined(__BORLANDC__) || defined(__TURBOC__) || defined(_Windows) ||       \
    defined(MSDOS)
#ifndef SJISPRE
#define SJISPRE 1
#endif
#endif

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#define TRUE 1
#define FALSE 0

#define NEW 1
#define OLD 2
#define ESCI 3
#define NEC 4
#define EUC 5
#define SJIS 6
#define EUCORSJIS 7
#define ASCII 8

#define NEWJISSTR "JIS7"
#define OLDJISSTR "jis"
#define EUCSTR "eucJP"
#define SJISSTR "SJIS"

#define ESC 27
#define SS2 142

/* DetectKanjiCode() derived from DetectCodeType() by Ken Lunde. */

static int DetectKanjiCode(const unsigned char *str)
{
    static int whatcode = ASCII;
    int oldcode = ASCII;
    int c, i;
    char *lang = NULL;

    c = '\1';
    i = 0;

    if (whatcode != EUCORSJIS && whatcode != ASCII) {
        oldcode = whatcode;
        whatcode = ASCII;
    }

    while ((whatcode == EUCORSJIS || whatcode == ASCII) && c != '\0') {
        if ((c = str[i++]) != '\0') {
            if (c == ESC) {
                c = str[i++];
                if (c == '$') {
                    c = str[i++];
                    if (c == 'B')
                        whatcode = NEW;
                    else if (c == '@')
                        whatcode = OLD;
                } else if (c == '(') {
                    c = str[i++];
                    if (c == 'I')
                        whatcode = ESCI;
                } else if (c == 'K')
                    whatcode = NEC;
            } else if ((c >= 129 && c <= 141) || (c >= 143 && c <= 159))
                whatcode = SJIS;
            else if (c == SS2) {
                c = str[i++];
                if ((c >= 64 && c <= 126) || (c >= 128 && c <= 160) || (c >= 224 && c <= 252))
                    whatcode = SJIS;
                else if (c >= 161 && c <= 223)
                    whatcode = EUCORSJIS;
            } else if (c >= 161 && c <= 223) {
                c = str[i++];
                if (c >= 240 && c <= 254)
                    whatcode = EUC;
                else if (c >= 161 && c <= 223)
                    whatcode = EUCORSJIS;
                else if (c >= 224 && c <= 239) {
                    whatcode = EUCORSJIS;
                    while (c >= 64 && whatcode == EUCORSJIS) {
                        if (c >= 129) {
                            if (c <= 141 || (c >= 143 && c <= 159))
                                whatcode = SJIS;
                            else if (c >= 253 && c <= 254)
                                whatcode = EUC;
                        }
                        c = str[i++];
                    }
                } else if (c <= 159)
                    whatcode = SJIS;
            } else if (c >= 240 && c <= 254)
                whatcode = EUC;
            else if (c >= 224 && c <= 239) {
                c = str[i++];
                if ((c >= 64 && c <= 126) || (c >= 128 && c <= 160))
                    whatcode = SJIS;
                else if (c >= 253 && c <= 254)
                    whatcode = EUC;
                else if (c >= 161 && c <= 252)
                    whatcode = EUCORSJIS;
            }
        }
    }

#ifdef DEBUG
    if (whatcode == ASCII)
        gd_error_ex(GD_DEBUG, "Kanji code not included.");
    else if (whatcode == EUCORSJIS)
        gd_error_ex(GD_DEBUG, "Kanji code not detected.");
    else
        gd_error_ex(GD_DEBUG, "Kanji code detected at %d byte.", i);
#endif

    if (whatcode == EUCORSJIS && oldcode != ASCII)
        whatcode = oldcode;

    if (whatcode == EUCORSJIS) {
        if (getenv("LC_ALL"))
            lang = getenv("LC_ALL");
        else if (getenv("LC_CTYPE"))
            lang = getenv("LC_CTYPE");
        else if (getenv("LANG"))
            lang = getenv("LANG");

        if (lang) {
            if (strcmp(lang, "ja_JP.SJIS") == 0 ||
#ifdef hpux
                strcmp(lang, "japanese") == 0 ||
#endif
                strcmp(lang, "ja_JP.mscode") == 0 || strcmp(lang, "ja_JP.PCK") == 0)
                whatcode = SJIS;
            else if (strncmp(lang, "ja", 2) == 0)
#ifdef SJISPRE
                whatcode = SJIS;
#else
                whatcode = EUC;
#endif
        }
    }

    if (whatcode == EUCORSJIS)
#ifdef SJISPRE
        whatcode = SJIS;
#else
        whatcode = EUC;
#endif

    return whatcode;
}

/* SJIStoJIS() is sjis2jis() by Ken Lunde. */

static void SJIStoJIS(int *p1, int *p2)
{
    register unsigned char c1 = *p1;
    register unsigned char c2 = *p2;
    register int adjust = c2 < 159;
    register int rowOffset = c1 < 160 ? 112 : 176;
    register int cellOffset = adjust ? (31 + (c2 > 127)) : 126;

    *p1 = ((c1 - rowOffset) << 1) - adjust;
    *p2 -= cellOffset;
}

/* han2zen() was derived from han2zen() written by Ken Lunde. */

#define IS_DAKU(c) ((c >= 182 && c <= 196) || (c >= 202 && c <= 206) || (c == 179))
#define IS_HANDAKU(c) (c >= 202 && c <= 206)

static void han2zen(int *p1, int *p2)
{
    int c = *p1;
    int daku = FALSE;
    int handaku = FALSE;
    int mtable[][2] = {
        {129, 66},  {129, 117}, {129, 118}, {129, 65},  {129, 69},  {131, 146}, {131, 64},
        {131, 66},  {131, 68},  {131, 70},  {131, 72},  {131, 131}, {131, 133}, {131, 135},
        {131, 98},  {129, 91},  {131, 65},  {131, 67},  {131, 69},  {131, 71},  {131, 73},
        {131, 74},  {131, 76},  {131, 78},  {131, 80},  {131, 82},  {131, 84},  {131, 86},
        {131, 88},  {131, 90},  {131, 92},  {131, 94},  {131, 96},  {131, 99},  {131, 101},
        {131, 103}, {131, 105}, {131, 106}, {131, 107}, {131, 108}, {131, 109}, {131, 110},
        {131, 113}, {131, 116}, {131, 119}, {131, 122}, {131, 125}, {131, 126}, {131, 128},
        {131, 129}, {131, 130}, {131, 132}, {131, 134}, {131, 136}, {131, 137}, {131, 138},
        {131, 139}, {131, 140}, {131, 141}, {131, 143}, {131, 147}, {129, 74},  {129, 75}};

    if (*p2 == 222 && IS_DAKU(*p1))
        daku = TRUE; /* Daku-ten */
    else if (*p2 == 223 && IS_HANDAKU(*p1))
        handaku = TRUE; /* Han-daku-ten */

    *p1 = mtable[c - 161][0];
    *p2 = mtable[c - 161][1];

    if (daku) {
        if ((*p2 >= 74 && *p2 <= 103) || (*p2 >= 110 && *p2 <= 122))
            (*p2)++;
        else if (*p2 == 131 || *p2 == 69)
            *p2 = 148;
    } else if (handaku && *p2 >= 110 && *p2 <= 122)
        (*p2) += 2;
}

/* Recast strcpy to handle unsigned chars used below. */
#define ustrcpy(A, B) (strcpy((char *)(A), (const char *)(B)))
#define ustrncpy(A, B, maxsize) (strncpy((char *)(A), (const char *)(B), maxsize))
static void do_convert(unsigned char **to, const unsigned char **from, const char *code)
{
#ifdef HAVE_ICONV
    iconv_t cd;
    size_t from_len, to_len;

    if ((cd = iconv_open(EUCSTR, code)) == (iconv_t)-1) {
        gd_error("iconv_open() error");
        if (errno == EINVAL)
            gd_error("invalid code specification: \"%s\" or \"%s\"", EUCSTR, code);
        ustrcpy(*to, *from);
        return;
    }

    from_len = strlen((const char *)*from) + 1;
    to_len = BUFSIZ;

    if ((int)(iconv(cd, (char **)from, &from_len, (char **)to, &to_len)) == -1) {
        if (errno == EINVAL)
            gd_error("invalid end of input string");
        else if (errno == EILSEQ)
            gd_error("invalid code in input string");
        else if (errno == E2BIG)
            gd_error("output buffer overflow at do_convert()");
        else
            gd_error("something happen");
        ustrcpy(*to, *from);
        if (iconv_close(cd) != 0)
            gd_error("iconv_close() error");
        return;
    }

    if (iconv_close(cd) != 0)
        gd_error("iconv_close() error");
#else
    int p1, p2, i, j;
    int jisx0208 = FALSE;
    int hankaku = FALSE;
    unsigned char *to_buf = *to;
    const unsigned char *from_buf = *from;

    j = 0;
    if (strcmp(code, NEWJISSTR) == 0 || strcmp(code, OLDJISSTR) == 0) {
        for (i = 0; from_buf[i] != '\0' && j < BUFSIZ; i++) {
            if (from_buf[i] == ESC) {
                i++;
                if (from_buf[i] == '$') {
                    jisx0208 = TRUE;
                    hankaku = FALSE;
                    i++;
                } else if (from_buf[i] == '(') {
                    jisx0208 = FALSE;
                    i++;
                    if (from_buf[i] == 'I')
                        hankaku = TRUE;
                    else
                        hankaku = FALSE;
                }
            } else {
                if (jisx0208)
                    to_buf[j++] = from_buf[i] + 128;
                else if (hankaku) {
                    to_buf[j++] = SS2;
                    to_buf[j++] = from_buf[i] + 128;
                } else
                    to_buf[j++] = from_buf[i];
            }
        }
    } else if (strcmp(code, SJISSTR) == 0) {
        for (i = 0; from_buf[i] != '\0' && j < BUFSIZ; i++) {
            p1 = from_buf[i];
            if (p1 < 127)
                to_buf[j++] = p1;
            else if ((p1 >= 161) && (p1 <= 223)) {
                to_buf[j++] = SS2;
                to_buf[j++] = p1;
            } else {
                p2 = from_buf[++i];
                SJIStoJIS(&p1, &p2);
                to_buf[j++] = p1 + 128;
                to_buf[j++] = p2 + 128;
            }
        }
    } else {
        gd_error("invalid code specification: \"%s\"", code);
        return;
    }

    if (j >= BUFSIZ) {
        gd_error("output buffer overflow at do_convert()");
        ustrcpy(*to, *from);
    } else
        to_buf[j] = '\0';
#endif
}

static int do_check_and_conv(unsigned char *to, const unsigned char *from)
{
    static unsigned char tmp[BUFSIZ];
    unsigned char *tmp_p = &tmp[0];
    int p1, p2, i, j;
    int kanji = TRUE;
    int copy_string = FALSE;

    switch (DetectKanjiCode(from)) {
    case NEW:
        gd_error_ex(GD_DEBUG, "Kanji code is New JIS.");
        do_convert(&tmp_p, &from, NEWJISSTR);
        break;
    case OLD:
        gd_error_ex(GD_DEBUG, "Kanji code is Old JIS.");
        do_convert(&tmp_p, &from, OLDJISSTR);
        break;
    case ESCI:
        gd_error_ex(GD_DEBUG, "This string includes Hankaku-Kana (jisx0201) "
                              "escape sequence [ESC] + ( + I.");
        do_convert(&tmp_p, &from, NEWJISSTR);
        break;
    case NEC:
        gd_error_ex(GD_DEBUG, "Kanji code is NEC Kanji.");
        gd_error("cannot convert NEC Kanji.");
        copy_string = TRUE;
        kanji = FALSE;
        break;
    case EUC:
        gd_error_ex(GD_DEBUG, "Kanji code is EUC.");
        copy_string = TRUE;
        break;
    case SJIS:
        gd_error_ex(GD_DEBUG, "Kanji code is SJIS.");
        do_convert(&tmp_p, &from, SJISSTR);
        break;
    case EUCORSJIS:
        gd_error_ex(GD_DEBUG, "Kanji code is EUC or SJIS.");
        copy_string = TRUE;
        kanji = FALSE;
        break;
    case ASCII:
        gd_error_ex(GD_DEBUG, "This is ASCII string.");
        copy_string = TRUE;
        kanji = FALSE;
        break;
    default:
        gd_error_ex(GD_DEBUG, "This string includes unknown code.");
        copy_string = TRUE;
        kanji = FALSE;
        break;
    }

    if (copy_string) {
        ustrncpy(tmp, from, BUFSIZ);
        tmp[BUFSIZ - 1] = '\0';
    }

    /* Hankaku Kana ---> Zenkaku Kana */
    if (kanji) {
        j = 0;
        for (i = 0; tmp[i] != '\0' && j < BUFSIZ; i++) {
            if (tmp[i] == SS2) {
                p1 = tmp[++i];
                if (tmp[i + 1] == SS2) {
                    p2 = tmp[i + 2];
                    if (p2 == 222 || p2 == 223)
                        i += 2;
                    else
                        p2 = 0;
                } else
                    p2 = 0;
                han2zen(&p1, &p2);
                SJIStoJIS(&p1, &p2);
                to[j++] = p1 + 128;
                to[j++] = p2 + 128;
            } else
                to[j++] = tmp[i];
        }

        if (j >= BUFSIZ) {
            gd_error("output buffer overflow at Hankaku --> Zenkaku");
            ustrcpy(to, tmp);
        } else
            to[j] = '\0';
    } else
        ustrcpy(to, tmp);

    return kanji;
}

int any2eucjp(unsigned char *dest, const unsigned char *src, unsigned int dest_max)
{
    static unsigned char tmp_dest[BUFSIZ];
    int ret;

    if (strlen((const char *)src) >= BUFSIZ) {
        gd_error("input string too large");
        return -1;
    }
    if (dest_max > BUFSIZ) {
        gd_error("invalid maximum size of destination\nit should be less than %d.", BUFSIZ);
        return -1;
    }
    ret = do_check_and_conv(tmp_dest, src);
    if (strlen((const char *)tmp_dest) >= dest_max) {
        gd_error("output buffer overflow");
        ustrcpy(dest, src);
        return -1;
    }
    ustrcpy(dest, tmp_dest);
    return ret;
}

#if 0
unsigned int
strwidth (unsigned char *s)
{
  unsigned char *t;
  unsigned int i;

  t = (unsigned char *) gdMalloc (BUFSIZ);
  any2eucjp (t, s, BUFSIZ);
  i = strlen (t);
  gdFree (t);
  return i;
}

#ifdef DEBUG
int
main ()
{
  unsigned char input[BUFSIZ];
  unsigned char *output;
  unsigned char *str;
  int c, i = 0;

  while ((c = fgetc (stdin)) != '\n' && i < BUFSIZ)
    input[i++] = c;
  input[i] = '\0';

  printf ("input : %d bytes\n", strlen ((const char *) input));
  printf ("output: %d bytes\n", strwidth (input));

  output = (unsigned char *) gdMalloc (BUFSIZ);
  any2eucjp (output, input, BUFSIZ);
  str = output;
  while (*str != '\0')
    putchar (*(str++));
  putchar ('\n');
  gdFree (output);

  return 0;
}
#endif
#endif
