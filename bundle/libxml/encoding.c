/*
 * encoding.c : implements the encoding conversion functions needed for XML
 *
 * Related specs: 
 * rfc2044        (UTF-8 and UTF-16) F. Yergeau Alis Technologies
 * rfc2781        UTF-16, an encoding of ISO 10646, P. Hoffman, F. Yergeau
 * [ISO-10646]    UTF-8 and UTF-16 in Annexes
 * [ISO-8859-1]   ISO Latin-1 characters codes.
 * [UNICODE]      The Unicode Consortium, "The Unicode Standard --
 *                Worldwide Character Encoding -- Version 1.0", Addison-
 *                Wesley, Volume 1, 1991, Volume 2, 1992.  UTF-8 is
 *                described in Unicode Technical Report #4.
 * [US-ASCII]     Coded Character Set--7-bit American Standard Code for
 *                Information Interchange, ANSI X3.4-1986.
 *
 * See Copyright for the status of this software.
 *
 * daniel@veillard.com
 *
 * UTF8 string routines from:
 * "William M. Brack" <wbrack@mmm.com.hk>
 *
 * Original code for IsoLatin1 and UTF-16 by "Martin J. Duerst" <duerst@w3.org>
 */

#define IN_LIBXML
#include "libxml.h"

#include <string.h>

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef LIBXML_ICONV_ENABLED
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#endif
#include <libxml/encoding.h>
#include <libxml/xmlmemory.h>
#ifdef LIBXML_HTML_ENABLED
#include <libxml/HTMLparser.h>
#endif
#include <libxml/globals.h>
#include <libxml/xmlerror.h>

static xmlCharEncodingHandlerPtr xmlUTF16LEHandler = NULL;
static xmlCharEncodingHandlerPtr xmlUTF16BEHandler = NULL;

typedef struct _xmlCharEncodingAlias xmlCharEncodingAlias;
typedef xmlCharEncodingAlias *xmlCharEncodingAliasPtr;
struct _xmlCharEncodingAlias {
    const char *name;
    const char *alias;
};

static xmlCharEncodingAliasPtr xmlCharEncodingAliases = NULL;
static int xmlCharEncodingAliasesNb = 0;
static int xmlCharEncodingAliasesMax = 0;

#ifdef LIBXML_ICONV_ENABLED
#if 0
#define DEBUG_ENCODING  /* Define this to get encoding traces */
#endif
#endif

static int xmlLittleEndian = 1;

/************************************************************************
 *									*
 *			Generic UTF8 handling routines			*
 *									*
 * From rfc2044: encoding of the Unicode values on UTF-8:		*
 *									*
 * UCS-4 range (hex.)           UTF-8 octet sequence (binary)		*
 * 0000 0000-0000 007F   0xxxxxxx					*
 * 0000 0080-0000 07FF   110xxxxx 10xxxxxx				*
 * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 			*
 *									*
 * I hope we won't use values > 0xFFFF anytime soon !			*
 *									*
 ************************************************************************/

/**
 * xmlUTF8Strlen:
 * @utf:  a sequence of UTF-8 encoded bytes
 *
 * compute the length of an UTF8 string, it doesn't do a full UTF8
 * checking of the content of the string.
 *
 * Returns the number of characters in the string or -1 in case of error
 */
int
xmlUTF8Strlen(const xmlChar *utf) {
    int ret = 0;

    if (utf == NULL)
	return(-1);

    while (*utf != 0) {
	if (utf[0] & 0x80) {
	    if ((utf[1] & 0xc0) != 0x80)
		return(-1);
	    if ((utf[0] & 0xe0) == 0xe0) {
		if ((utf[2] & 0xc0) != 0x80)
		    return(-1);
		if ((utf[0] & 0xf0) == 0xf0) {
		    if ((utf[0] & 0xf8) != 0xf0 || (utf[3] & 0xc0) != 0x80)
			return(-1);
		    utf += 4;
		} else {
		    utf += 3;
		}
	    } else {
		utf += 2;
	    }
	} else {
	    utf++;
	}
	ret++;
    }
    return(ret);
}

/**
 * xmlGetUTF8Char:
 * @utf:  a sequence of UTF-8 encoded bytes
 * @len:  a pointer to @bytes len
 *
 * Read one UTF8 Char from @utf
 *
 * Returns the char value or -1 in case of error and update @len with the
 *        number of bytes used
 */
int
xmlGetUTF8Char(const unsigned char *utf, int *len) {
    unsigned int c;

    if (utf == NULL)
	goto error;
    if (len == NULL)
	goto error;
    if (*len < 1)
	goto error;

    c = utf[0];
    if (c & 0x80) {
	if (*len < 2)
	    goto error;
	if ((utf[1] & 0xc0) != 0x80)
	    goto error;
	if ((c & 0xe0) == 0xe0) {
	    if (*len < 3)
		goto error;
	    if ((utf[2] & 0xc0) != 0x80)
		goto error;
	    if ((c & 0xf0) == 0xf0) {
		if (*len < 4)
		    goto error;
		if ((c & 0xf8) != 0xf0 || (utf[3] & 0xc0) != 0x80)
		    goto error;
		*len = 4;
		/* 4-byte code */
		c = (utf[0] & 0x7) << 18;
		c |= (utf[1] & 0x3f) << 12;
		c |= (utf[2] & 0x3f) << 6;
		c |= utf[3] & 0x3f;
	    } else {
	      /* 3-byte code */
		*len = 3;
		c = (utf[0] & 0xf) << 12;
		c |= (utf[1] & 0x3f) << 6;
		c |= utf[2] & 0x3f;
	    }
	} else {
	  /* 2-byte code */
	    *len = 2;
	    c = (utf[0] & 0x1f) << 6;
	    c |= utf[1] & 0x3f;
	}
    } else {
	/* 1-byte code */
	*len = 1;
    }
    return(c);

error:
    *len = 0;
    return(-1);
}

/**
 * xmlCheckUTF8:
 * @utf: Pointer to putative utf-8 encoded string.
 *
 * Checks @utf for being valid utf-8. @utf is assumed to be
 * null-terminated. This function is not super-strict, as it will
 * allow longer utf-8 sequences than necessary. Note that Java is
 * capable of producing these sequences if provoked. Also note, this
 * routine checks for the 4-byte maximum size, but does not check for
 * 0x10ffff maximum value.
 *
 * Return value: true if @utf is valid.
 **/
int
xmlCheckUTF8(const unsigned char *utf)
{
    int ix;
    unsigned char c;

    for (ix = 0; (c = utf[ix]);) {
        if (c & 0x80) {
	    if ((utf[ix + 1] & 0xc0) != 0x80)
	        return(0);
	    if ((c & 0xe0) == 0xe0) {
	        if ((utf[ix + 2] & 0xc0) != 0x80)
		    return(0);
	        if ((c & 0xf0) == 0xf0) {
		    if ((c & 0xf8) != 0xf0 || (utf[ix + 3] & 0xc0) != 0x80)
		        return(0);
		    ix += 4;
		    /* 4-byte code */
	        } else
		  /* 3-byte code */
		    ix += 3;
	    } else
	      /* 2-byte code */
	        ix += 2;
	} else
	    /* 1-byte code */
	    ix++;
      }
      return(1);
}

/**
 * xmlUTF8Strsize:
 * @utf:  a sequence of UTF-8 encoded bytes
 * @len:  the number of characters in the array
 *
 * storage size of an UTF8 string
 *
 * Returns the storage size of
 * the first 'len' characters of ARRAY
 *
 */

int
xmlUTF8Strsize(const xmlChar *utf, int len) {
    const xmlChar	*ptr=utf;
    xmlChar	ch;

    if (len <= 0)
	return(0);

    while ( len-- > 0) {
	if ( !*ptr )
	    break;
	if ( (ch = *ptr++) & 0x80)
	    while ( (ch<<=1) & 0x80 )
		ptr++;
    }
    return (ptr - utf);
}


/**
 * xmlUTF8Strndup:
 * @utf:  the input UTF8 *
 * @len:  the len of @utf (in chars)
 *
 * a strndup for array of UTF8's
 *
 * Returns a new UTF8 * or NULL
 */
xmlChar *
xmlUTF8Strndup(const xmlChar *utf, int len) {
    xmlChar *ret;
    int i;
    
    if ((utf == NULL) || (len < 0)) return(NULL);
    i = xmlUTF8Strsize(utf, len);
    ret = (xmlChar *) xmlMalloc((i + 1) * sizeof(xmlChar));
    if (ret == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"malloc of %ld byte failed\n",
	        (len + 1) * (long)sizeof(xmlChar));
        return(NULL);
    }
    memcpy(ret, utf, i * sizeof(xmlChar));
    ret[i] = 0;
    return(ret);
}

/**
 * xmlUTF8Strpos:
 * @utf:  the input UTF8 *
 * @pos:  the position of the desired UTF8 char (in chars)
 *
 * a function to provide the equivalent of fetching a
 * character from a string array
 *
 * Returns a pointer to the UTF8 character or NULL
 */
xmlChar *
xmlUTF8Strpos(const xmlChar *utf, int pos) {
    xmlChar ch;

    if (utf == NULL) return(NULL);
    if ( (pos < 0) || (pos >= xmlUTF8Strlen(utf)) )
	return(NULL);
    while (pos--) {
	if ((ch=*utf++) == 0) return(NULL);
	if ( ch & 0x80 ) {
	    /* if not simple ascii, verify proper format */
	    if ( (ch & 0xc0) != 0xc0 )
		return(NULL);
	    /* then skip over remaining bytes for this char */
	    while ( (ch <<= 1) & 0x80 )
		if ( (*utf++ & 0xc0) != 0x80 )
		    return(NULL);
	}
    }
    return((xmlChar *)utf);
}

/**
 * xmlUTF8Strloc:
 * @utf:  the input UTF8 *
 * @utfchar:  the UTF8 character to be found
 *
 * a function to provide relative location of a UTF8 char
 *
 * Returns the relative character position of the desired char
 * or -1 if not found
 */
int
xmlUTF8Strloc(const xmlChar *utf, const xmlChar *utfchar) {
    int i, size;
    xmlChar ch;

    if (utf==NULL || utfchar==NULL) return -1;
    size = xmlUTF8Strsize(utfchar, 1);
	for(i=0; (ch=*utf) != 0; i++) {
	    if (xmlStrncmp(utf, utfchar, size)==0)
		return(i);
	    utf++;
	    if ( ch & 0x80 ) {
		/* if not simple ascii, verify proper format */
		if ( (ch & 0xc0) != 0xc0 )
		    return(-1);
		/* then skip over remaining bytes for this char */
		while ( (ch <<= 1) & 0x80 )
		    if ( (*utf++ & 0xc0) != 0x80 )
			return(-1);
	    }
	}

    return(-1);
}
/**
 * xmlUTF8Strsub:
 * @utf:  a sequence of UTF-8 encoded bytes
 * @start: relative pos of first char
 * @len:   total number to copy
 *
 * Note:  positions are given in units of UTF-8 chars
 *
 * Returns a pointer to a newly created string
 * or NULL if any problem
 */

xmlChar *
xmlUTF8Strsub(const xmlChar *utf, int start, int len) {
    int	    i;
    xmlChar ch;

    if (utf == NULL) return(NULL);
    if (start < 0) return(NULL);
    if (len < 0) return(NULL);

    /*
     * Skip over any leading chars
     */
    for (i = 0;i < start;i++) {
	if ((ch=*utf++) == 0) return(NULL);
	if ( ch & 0x80 ) {
	    /* if not simple ascii, verify proper format */
	    if ( (ch & 0xc0) != 0xc0 )
		return(NULL);
	    /* then skip over remaining bytes for this char */
	    while ( (ch <<= 1) & 0x80 )
		if ( (*utf++ & 0xc0) != 0x80 )
		    return(NULL);
	}
    }

    return(xmlUTF8Strndup(utf, len));
}

/************************************************************************
 *									*
 *		Conversions To/From UTF8 encoding			*
 *									*
 ************************************************************************/

/**
 * asciiToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ASCII chars
 * @inlen:  the length of @in
 *
 * Take a block of ASCII chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of ocetes consumed.
 */
static int
asciiToUTF8(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    unsigned char* outstart = out;
    const unsigned char* base = in;
    const unsigned char* processed = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    unsigned int c;
    int bits;

    inend = in + (*inlen);
    while ((in < inend) && (out - outstart + 5 < *outlen)) {
	c= *in++;

	/* assertion: c is a single UTF-4 value */
        if (out >= outend)
	    break;
        if      (c <    0x80) {  *out++=  c;                bits= -6; }
        else { 
	    *outlen = out - outstart;
	    *inlen = processed - base;
	    return(-1);
	}
 
        for ( ; bits >= 0; bits-= 6) {
            if (out >= outend)
	        break;
            *out++= ((c >> bits) & 0x3F) | 0x80;
        }
	processed = (const unsigned char*) in;
    }
    *outlen = out - outstart;
    *inlen = processed - base;
    return(0);
}

/**
 * UTF8Toascii:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ASCII
 * block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of ocetes consumed.
 */
static int
UTF8Toascii(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if (in == NULL) {
        /*
	 * initialization nothing to do
	 */
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    inend = in + (*inlen);
    outend = out + (*outlen);
    while (in < inend) {
	d = *in++;
	if      (d < 0x80)  { c= d; trailing= 0; }
	else if (d < 0xC0) {
	    /* trailing byte in leading position */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
        } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
	else {
	    /* no chance for this in Ascii */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}

	if (inend - in < trailing) {
	    break;
	} 

	for ( ; trailing; trailing--) {
	    if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80))
		break;
	    c <<= 6;
	    c |= d & 0x3F;
	}

	/* assertion: c is a single UTF-4 value */
	if (c < 0x80) {
	    if (out >= outend)
		break;
	    *out++ = c;
	} else {
	    /* no chance for this in Ascii */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}
	processed = in;
    }
    *outlen = out - outstart;
    *inlen = processed - instart;
    return(0);
}

/**
 * isolat1ToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ISO Latin 1 chars
 * @inlen:  the length of @in
 *
 * Take a block of ISO Latin 1 chars in and try to convert it to an UTF-8
 * block of chars out.
 * Returns 0 if success, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of ocetes consumed.
 */
int
isolat1ToUTF8(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    unsigned char* outstart = out;
    const unsigned char* base = in;
    unsigned char* outend = out + *outlen;
    const unsigned char* inend;
    const unsigned char* instop;
    xmlChar c = *in;

    inend = in + (*inlen);
    instop = inend;
    
    while (in < inend && out < outend - 1) {
    	if (c >= 0x80) {
	    *out++= ((c >>  6) & 0x1F) | 0xC0;
            *out++= (c & 0x3F) | 0x80;
	    ++in;
	    c = *in;
	}
	if (instop - in > outend - out) instop = in + (outend - out); 
	while (c < 0x80 && in < instop) {
	    *out++ =  c;
	    ++in;
	    c = *in;
	}
    }	
    if (in < inend && out < outend && c < 0x80) {
        *out++ =  c;
	++in;
    }
    *outlen = out - outstart;
    *inlen = in - base;
    return(0);
}


/**
 * UTF8Toisolat1:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an ISO Latin 1
 * block of chars out.
 *
 * Returns 0 if success, -2 if the transcoding fails, or -1 otherwise
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of ocetes consumed.
 */
int
UTF8Toisolat1(unsigned char* out, int *outlen,
              const unsigned char* in, int *inlen) {
    const unsigned char* processed = in;
    const unsigned char* outend;
    const unsigned char* outstart = out;
    const unsigned char* instart = in;
    const unsigned char* inend;
    unsigned int c, d;
    int trailing;

    if (in == NULL) {
        /*
	 * initialization nothing to do
	 */
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    inend = in + (*inlen);
    outend = out + (*outlen);
    while (in < inend) {
	d = *in++;
	if      (d < 0x80)  { c= d; trailing= 0; }
	else if (d < 0xC0) {
	    /* trailing byte in leading position */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
        } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
        else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
        else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
	else {
	    /* no chance for this in IsoLat1 */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}

	if (inend - in < trailing) {
	    break;
	} 

	for ( ; trailing; trailing--) {
	    if (in >= inend)
		break;
	    if (((d= *in++) & 0xC0) != 0x80) {
		*outlen = out - outstart;
		*inlen = processed - instart;
		return(-2);
	    }
	    c <<= 6;
	    c |= d & 0x3F;
	}

	/* assertion: c is a single UTF-4 value */
	if (c <= 0xFF) {
	    if (out >= outend)
		break;
	    *out++ = c;
	} else {
	    /* no chance for this in IsoLat1 */
	    *outlen = out - outstart;
	    *inlen = processed - instart;
	    return(-2);
	}
	processed = in;
    }
    *outlen = out - outstart;
    *inlen = processed - instart;
    return(0);
}

/**
 * UTF16LEToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @inb:  a pointer to an array of UTF-16LE passwd as a byte array
 * @inlenb:  the length of @in in UTF-16LE chars
 *
 * Take a block of UTF-16LE ushorts in and try to convert it to an UTF-8
 * block of chars out. This function assume the endian property
 * is the same between the native type of this machine and the
 * inputed one.
 *
 * Returns the number of byte written, or -1 by lack of space, or -2
 *     if the transcoding fails (for *in is not valid utf16 string)
 *     The value of *inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 */
static int
UTF16LEToUTF8(unsigned char* out, int *outlen,
            const unsigned char* inb, int *inlenb)
{
    unsigned char* outstart = out;
    const unsigned char* processed = inb;
    unsigned char* outend = out + *outlen;
    unsigned short* in = (unsigned short*) inb;
    unsigned short* inend;
    unsigned int c, d, inlen;
    unsigned char *tmp;
    int bits;

    if ((*inlenb % 2) == 1)
        (*inlenb)--;
    inlen = *inlenb / 2;
    inend = in + inlen;
    while ((in < inend) && (out - outstart + 5 < *outlen)) {
        if (xmlLittleEndian) {
	    c= *in++;
	} else {
	    tmp = (unsigned char *) in;
	    c = *tmp++;
	    c = c | (((unsigned int)*tmp) << 8);
	    in++;
	}
        if ((c & 0xFC00) == 0xD800) {    /* surrogates */
	    if (in >= inend) {           /* (in > inend) shouldn't happens */
		break;
	    }
	    if (xmlLittleEndian) {
		d = *in++;
	    } else {
		tmp = (unsigned char *) in;
		d = *tmp++;
		d = d | (((unsigned int)*tmp) << 8);
		in++;
	    }
            if ((d & 0xFC00) == 0xDC00) {
                c &= 0x03FF;
                c <<= 10;
                c |= d & 0x03FF;
                c += 0x10000;
            }
            else {
		*outlen = out - outstart;
		*inlenb = processed - inb;
	        return(-2);
	    }
        }

	/* assertion: c is a single UTF-4 value */
        if (out >= outend)
	    break;
        if      (c <    0x80) {  *out++=  c;                bits= -6; }
        else if (c <   0x800) {  *out++= ((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
        else if (c < 0x10000) {  *out++= ((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
        else                  {  *out++= ((c >> 18) & 0x07) | 0xF0;  bits= 12; }
 
        for ( ; bits >= 0; bits-= 6) {
            if (out >= outend)
	        break;
            *out++= ((c >> bits) & 0x3F) | 0x80;
        }
	processed = (const unsigned char*) in;
    }
    *outlen = out - outstart;
    *inlenb = processed - inb;
    return(0);
}

/**
 * UTF8ToUTF16LE:
 * @outb:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @outb
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an UTF-16LE
 * block of chars out.
 *
 * Returns the number of byte written, or -1 by lack of space, or -2
 *     if the transcoding failed. 
 */
static int
UTF8ToUTF16LE(unsigned char* outb, int *outlen,
            const unsigned char* in, int *inlen)
{
    unsigned short* out = (unsigned short*) outb;
    const unsigned char* processed = in;
    unsigned short* outstart= out;
    unsigned short* outend;
    const unsigned char* inend= in+*inlen;
    unsigned int c, d;
    int trailing;
    unsigned char *tmp;
    unsigned short tmp1, tmp2;

    if (in == NULL) {
        /*
	 * initialization, add the Byte Order Mark
	 */
        if (*outlen >= 2) {
	    outb[0] = 0xFF;
	    outb[1] = 0xFE;
	    *outlen = 2;
	    *inlen = 0;
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
		    "Added FFFE Byte Order Mark\n");
#endif
	    return(2);
	}
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    outend = out + (*outlen / 2);
    while (in < inend) {
      d= *in++;
      if      (d < 0x80)  { c= d; trailing= 0; }
      else if (d < 0xC0) {
          /* trailing byte in leading position */
	  *outlen = (out - outstart) * 2;
	  *inlen = processed - in;
	  return(-2);
      } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
      else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
      else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
      else {
	/* no chance for this in UTF-16 */
	*outlen = (out - outstart) * 2;
	*inlen = processed - in;
	return(-2);
      }

      if (inend - in < trailing) {
          break;
      } 

      for ( ; trailing; trailing--) {
          if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80))
	      break;
          c <<= 6;
          c |= d & 0x3F;
      }

      /* assertion: c is a single UTF-4 value */
        if (c < 0x10000) {
            if (out >= outend)
	        break;
	    if (xmlLittleEndian) {
		*out++ = c;
	    } else {
		tmp = (unsigned char *) out;
		*tmp = c ;
		*(tmp + 1) = c >> 8 ;
		out++;
	    }
        }
        else if (c < 0x110000) {
            if (out+1 >= outend)
	        break;
            c -= 0x10000;
	    if (xmlLittleEndian) {
		*out++ = 0xD800 | (c >> 10);
		*out++ = 0xDC00 | (c & 0x03FF);
	    } else {
		tmp1 = 0xD800 | (c >> 10);
		tmp = (unsigned char *) out;
		*tmp = (unsigned char) tmp1;
		*(tmp + 1) = tmp1 >> 8;
		out++;

		tmp2 = 0xDC00 | (c & 0x03FF);
		tmp = (unsigned char *) out;
		*tmp  = (unsigned char) tmp2;
		*(tmp + 1) = tmp2 >> 8;
		out++;
	    }
        }
        else
	    break;
	processed = in;
    }
    *outlen = (out - outstart) * 2;
    *inlen = processed - in;
    return(0);
}

/**
 * UTF16BEToUTF8:
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @inb:  a pointer to an array of UTF-16 passwd as a byte array
 * @inlenb:  the length of @in in UTF-16 chars
 *
 * Take a block of UTF-16 ushorts in and try to convert it to an UTF-8
 * block of chars out. This function assume the endian property
 * is the same between the native type of this machine and the
 * inputed one.
 *
 * Returns the number of byte written, or -1 by lack of space, or -2
 *     if the transcoding fails (for *in is not valid utf16 string)
 * The value of *inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 */
static int
UTF16BEToUTF8(unsigned char* out, int *outlen,
            const unsigned char* inb, int *inlenb)
{
    unsigned char* outstart = out;
    const unsigned char* processed = inb;
    unsigned char* outend = out + *outlen;
    unsigned short* in = (unsigned short*) inb;
    unsigned short* inend;
    unsigned int c, d, inlen;
    unsigned char *tmp;
    int bits;

    if ((*inlenb % 2) == 1)
        (*inlenb)--;
    inlen = *inlenb / 2;
    inend= in + inlen;
    while (in < inend) {
	if (xmlLittleEndian) {
	    tmp = (unsigned char *) in;
	    c = *tmp++;
	    c = c << 8;
	    c = c | (unsigned int) *tmp;
	    in++;
	} else {
	    c= *in++;
	} 
        if ((c & 0xFC00) == 0xD800) {    /* surrogates */
	    if (in >= inend) {           /* (in > inend) shouldn't happens */
		*outlen = out - outstart;
		*inlenb = processed - inb;
	        return(-2);
	    }
	    if (xmlLittleEndian) {
		tmp = (unsigned char *) in;
		d = *tmp++;
		d = d << 8;
		d = d | (unsigned int) *tmp;
		in++;
	    } else {
		d= *in++;
	    }
            if ((d & 0xFC00) == 0xDC00) {
                c &= 0x03FF;
                c <<= 10;
                c |= d & 0x03FF;
                c += 0x10000;
            }
            else {
		*outlen = out - outstart;
		*inlenb = processed - inb;
	        return(-2);
	    }
        }

	/* assertion: c is a single UTF-4 value */
        if (out >= outend) 
	    break;
        if      (c <    0x80) {  *out++=  c;                bits= -6; }
        else if (c <   0x800) {  *out++= ((c >>  6) & 0x1F) | 0xC0;  bits=  0; }
        else if (c < 0x10000) {  *out++= ((c >> 12) & 0x0F) | 0xE0;  bits=  6; }
        else                  {  *out++= ((c >> 18) & 0x07) | 0xF0;  bits= 12; }
 
        for ( ; bits >= 0; bits-= 6) {
            if (out >= outend) 
	        break;
            *out++= ((c >> bits) & 0x3F) | 0x80;
        }
	processed = (const unsigned char*) in;
    }
    *outlen = out - outstart;
    *inlenb = processed - inb;
    return(0);
}

/**
 * UTF8ToUTF16BE:
 * @outb:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @outb
 * @in:  a pointer to an array of UTF-8 chars
 * @inlen:  the length of @in
 *
 * Take a block of UTF-8 chars in and try to convert it to an UTF-16BE
 * block of chars out.
 *
 * Returns the number of byte written, or -1 by lack of space, or -2
 *     if the transcoding failed. 
 */
static int
UTF8ToUTF16BE(unsigned char* outb, int *outlen,
            const unsigned char* in, int *inlen)
{
    unsigned short* out = (unsigned short*) outb;
    const unsigned char* processed = in;
    unsigned short* outstart= out;
    unsigned short* outend;
    const unsigned char* inend= in+*inlen;
    unsigned int c, d;
    int trailing;
    unsigned char *tmp;
    unsigned short tmp1, tmp2;

    if (in == NULL) {
        /*
	 * initialization, add the Byte Order Mark
	 */
        if (*outlen >= 2) {
	    outb[0] = 0xFE;
	    outb[1] = 0xFF;
	    *outlen = 2;
	    *inlen = 0;
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
		    "Added FEFF Byte Order Mark\n");
#endif
	    return(2);
	}
	*outlen = 0;
	*inlen = 0;
	return(0);
    }
    outend = out + (*outlen / 2);
    while (in < inend) {
      d= *in++;
      if      (d < 0x80)  { c= d; trailing= 0; }
      else if (d < 0xC0)  {
          /* trailing byte in leading position */
	  *outlen = out - outstart;
	  *inlen = processed - in;
	  return(-2);
      } else if (d < 0xE0)  { c= d & 0x1F; trailing= 1; }
      else if (d < 0xF0)  { c= d & 0x0F; trailing= 2; }
      else if (d < 0xF8)  { c= d & 0x07; trailing= 3; }
      else {
          /* no chance for this in UTF-16 */
	  *outlen = out - outstart;
	  *inlen = processed - in;
	  return(-2);
      }

      if (inend - in < trailing) {
          break;
      } 

      for ( ; trailing; trailing--) {
          if ((in >= inend) || (((d= *in++) & 0xC0) != 0x80))  break;
          c <<= 6;
          c |= d & 0x3F;
      }

      /* assertion: c is a single UTF-4 value */
        if (c < 0x10000) {
            if (out >= outend)  break;
	    if (xmlLittleEndian) {
		tmp = (unsigned char *) out;
		*tmp = c >> 8;
		*(tmp + 1) = c;
		out++;
	    } else {
		*out++ = c;
	    }
        }
        else if (c < 0x110000) {
            if (out+1 >= outend)  break;
            c -= 0x10000;
	    if (xmlLittleEndian) {
		tmp1 = 0xD800 | (c >> 10);
		tmp = (unsigned char *) out;
		*tmp = tmp1 >> 8;
		*(tmp + 1) = (unsigned char) tmp1;
		out++;

		tmp2 = 0xDC00 | (c & 0x03FF);
		tmp = (unsigned char *) out;
		*tmp = tmp2 >> 8;
		*(tmp + 1) = (unsigned char) tmp2;
		out++;
	    } else {
		*out++ = 0xD800 | (c >> 10);
		*out++ = 0xDC00 | (c & 0x03FF);
	    }
        }
        else
	    break;
	processed = in;
    }
    *outlen = (out - outstart) * 2;
    *inlen = processed - in;
    return(0);
}

/************************************************************************
 *									*
 *		Generic encoding handling routines			*
 *									*
 ************************************************************************/

/**
 * xmlDetectCharEncoding:
 * @in:  a pointer to the first bytes of the XML entity, must be at least
 *       4 bytes long.
 * @len:  pointer to the length of the buffer
 *
 * Guess the encoding of the entity using the first bytes of the entity content
 * accordingly of the non-normative appendix F of the XML-1.0 recommendation.
 * 
 * Returns one of the XML_CHAR_ENCODING_... values.
 */
xmlCharEncoding
xmlDetectCharEncoding(const unsigned char* in, int len)
{
    if (len >= 4) {
	if ((in[0] == 0x00) && (in[1] == 0x00) &&
	    (in[2] == 0x00) && (in[3] == 0x3C))
	    return(XML_CHAR_ENCODING_UCS4BE);
	if ((in[0] == 0x3C) && (in[1] == 0x00) &&
	    (in[2] == 0x00) && (in[3] == 0x00))
	    return(XML_CHAR_ENCODING_UCS4LE);
	if ((in[0] == 0x00) && (in[1] == 0x00) &&
	    (in[2] == 0x3C) && (in[3] == 0x00))
	    return(XML_CHAR_ENCODING_UCS4_2143);
	if ((in[0] == 0x00) && (in[1] == 0x3C) &&
	    (in[2] == 0x00) && (in[3] == 0x00))
	    return(XML_CHAR_ENCODING_UCS4_3412);
	if ((in[0] == 0x4C) && (in[1] == 0x6F) &&
	    (in[2] == 0xA7) && (in[3] == 0x94))
	    return(XML_CHAR_ENCODING_EBCDIC);
	if ((in[0] == 0x3C) && (in[1] == 0x3F) &&
	    (in[2] == 0x78) && (in[3] == 0x6D))
	    return(XML_CHAR_ENCODING_UTF8);
    }
    if (len >= 3) {
	/*
	 * Errata on XML-1.0 June 20 2001
	 * We now allow an UTF8 encoded BOM
	 */
	if ((in[0] == 0xEF) && (in[1] == 0xBB) &&
	    (in[2] == 0xBF))
	    return(XML_CHAR_ENCODING_UTF8);
    }
    if (len >= 2) {
	if ((in[0] == 0xFE) && (in[1] == 0xFF))
	    return(XML_CHAR_ENCODING_UTF16BE);
	if ((in[0] == 0xFF) && (in[1] == 0xFE))
	    return(XML_CHAR_ENCODING_UTF16LE);
    }
    return(XML_CHAR_ENCODING_NONE);
}

/**
 * xmlCleanupEncodingAliases:
 *
 * Unregisters all aliases
 */
void
xmlCleanupEncodingAliases(void) {
    int i;

    if (xmlCharEncodingAliases == NULL)
	return;

    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (xmlCharEncodingAliases[i].name != NULL)
	    xmlFree((char *) xmlCharEncodingAliases[i].name);
	if (xmlCharEncodingAliases[i].alias != NULL)
	    xmlFree((char *) xmlCharEncodingAliases[i].alias);
    }
    xmlCharEncodingAliasesNb = 0;
    xmlCharEncodingAliasesMax = 0;
    xmlFree(xmlCharEncodingAliases);
    xmlCharEncodingAliases = NULL;
}

/**
 * xmlGetEncodingAlias:
 * @alias:  the alias name as parsed, in UTF-8 format (ASCII actually)
 *
 * Lookup an encoding name for the given alias.
 * 
 * Returns NULL if not found the original name otherwise
 */
const char *
xmlGetEncodingAlias(const char *alias) {
    int i;
    char upper[100];

    if (alias == NULL)
	return(NULL);

    if (xmlCharEncodingAliases == NULL)
	return(NULL);

    for (i = 0;i < 99;i++) {
        upper[i] = toupper(alias[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;

    /*
     * Walk down the list looking for a definition of the alias
     */
    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (!strcmp(xmlCharEncodingAliases[i].alias, upper)) {
	    return(xmlCharEncodingAliases[i].name);
	}
    }
    return(NULL);
}

/**
 * xmlAddEncodingAlias:
 * @name:  the encoding name as parsed, in UTF-8 format (ASCII actually)
 * @alias:  the alias name as parsed, in UTF-8 format (ASCII actually)
 *
 * Registers and alias @alias for an encoding named @name. Existing alias
 * will be overwritten.
 * 
 * Returns 0 in case of success, -1 in case of error
 */
int
xmlAddEncodingAlias(const char *name, const char *alias) {
    int i;
    char upper[100];

    if ((name == NULL) || (alias == NULL))
	return(-1);

    for (i = 0;i < 99;i++) {
        upper[i] = toupper(alias[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;

    if (xmlCharEncodingAliases == NULL) {
	xmlCharEncodingAliasesNb = 0;
	xmlCharEncodingAliasesMax = 20;
	xmlCharEncodingAliases = (xmlCharEncodingAliasPtr) 
	      xmlMalloc(xmlCharEncodingAliasesMax * sizeof(xmlCharEncodingAlias));
	if (xmlCharEncodingAliases == NULL)
	    return(-1);
    } else if (xmlCharEncodingAliasesNb >= xmlCharEncodingAliasesMax) {
	xmlCharEncodingAliasesMax *= 2;
	xmlCharEncodingAliases = (xmlCharEncodingAliasPtr) 
	      xmlRealloc(xmlCharEncodingAliases,
		         xmlCharEncodingAliasesMax * sizeof(xmlCharEncodingAlias));
    }
    /*
     * Walk down the list looking for a definition of the alias
     */
    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (!strcmp(xmlCharEncodingAliases[i].alias, upper)) {
	    /*
	     * Replace the definition.
	     */
	    xmlFree((char *) xmlCharEncodingAliases[i].name);
	    xmlCharEncodingAliases[i].name = xmlMemStrdup(name);
	    return(0);
	}
    }
    /*
     * Add the definition
     */
    xmlCharEncodingAliases[xmlCharEncodingAliasesNb].name = xmlMemStrdup(name);
    xmlCharEncodingAliases[xmlCharEncodingAliasesNb].alias = xmlMemStrdup(upper);
    xmlCharEncodingAliasesNb++;
    return(0);
}

/**
 * xmlDelEncodingAlias:
 * @alias:  the alias name as parsed, in UTF-8 format (ASCII actually)
 *
 * Unregisters an encoding alias @alias
 * 
 * Returns 0 in case of success, -1 in case of error
 */
int
xmlDelEncodingAlias(const char *alias) {
    int i;

    if (alias == NULL)
	return(-1);

    if (xmlCharEncodingAliases == NULL)
	return(-1);
    /*
     * Walk down the list looking for a definition of the alias
     */
    for (i = 0;i < xmlCharEncodingAliasesNb;i++) {
	if (!strcmp(xmlCharEncodingAliases[i].alias, alias)) {
	    xmlFree((char *) xmlCharEncodingAliases[i].name);
	    xmlFree((char *) xmlCharEncodingAliases[i].alias);
	    xmlCharEncodingAliasesNb--;
	    memmove(&xmlCharEncodingAliases[i], &xmlCharEncodingAliases[i + 1],
		    sizeof(xmlCharEncodingAlias) * (xmlCharEncodingAliasesNb - i));
	    return(0);
	}
    }
    return(-1);
}

/**
 * xmlParseCharEncoding:
 * @name:  the encoding name as parsed, in UTF-8 format (ASCII actually)
 *
 * Compare the string to the known encoding schemes already known. Note
 * that the comparison is case insensitive accordingly to the section
 * [XML] 4.3.3 Character Encoding in Entities.
 * 
 * Returns one of the XML_CHAR_ENCODING_... values or XML_CHAR_ENCODING_NONE
 * if not recognized.
 */
xmlCharEncoding
xmlParseCharEncoding(const char* name)
{
    const char *alias;
    char upper[500];
    int i;

    if (name == NULL)
	return(XML_CHAR_ENCODING_NONE);

    /*
     * Do the alias resolution
     */
    alias = xmlGetEncodingAlias(name);
    if (alias != NULL)
	name = alias;

    for (i = 0;i < 499;i++) {
        upper[i] = toupper(name[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;

    if (!strcmp(upper, "")) return(XML_CHAR_ENCODING_NONE);
    if (!strcmp(upper, "UTF-8")) return(XML_CHAR_ENCODING_UTF8);
    if (!strcmp(upper, "UTF8")) return(XML_CHAR_ENCODING_UTF8);

    /*
     * NOTE: if we were able to parse this, the endianness of UTF16 is
     *       already found and in use
     */
    if (!strcmp(upper, "UTF-16")) return(XML_CHAR_ENCODING_UTF16LE);
    if (!strcmp(upper, "UTF16")) return(XML_CHAR_ENCODING_UTF16LE);
    
    if (!strcmp(upper, "ISO-10646-UCS-2")) return(XML_CHAR_ENCODING_UCS2);
    if (!strcmp(upper, "UCS-2")) return(XML_CHAR_ENCODING_UCS2);
    if (!strcmp(upper, "UCS2")) return(XML_CHAR_ENCODING_UCS2);

    /*
     * NOTE: if we were able to parse this, the endianness of UCS4 is
     *       already found and in use
     */
    if (!strcmp(upper, "ISO-10646-UCS-4")) return(XML_CHAR_ENCODING_UCS4LE);
    if (!strcmp(upper, "UCS-4")) return(XML_CHAR_ENCODING_UCS4LE);
    if (!strcmp(upper, "UCS4")) return(XML_CHAR_ENCODING_UCS4LE);

    
    if (!strcmp(upper,  "ISO-8859-1")) return(XML_CHAR_ENCODING_8859_1);
    if (!strcmp(upper,  "ISO-LATIN-1")) return(XML_CHAR_ENCODING_8859_1);
    if (!strcmp(upper,  "ISO LATIN 1")) return(XML_CHAR_ENCODING_8859_1);

    if (!strcmp(upper,  "ISO-8859-2")) return(XML_CHAR_ENCODING_8859_2);
    if (!strcmp(upper,  "ISO-LATIN-2")) return(XML_CHAR_ENCODING_8859_2);
    if (!strcmp(upper,  "ISO LATIN 2")) return(XML_CHAR_ENCODING_8859_2);

    if (!strcmp(upper,  "ISO-8859-3")) return(XML_CHAR_ENCODING_8859_3);
    if (!strcmp(upper,  "ISO-8859-4")) return(XML_CHAR_ENCODING_8859_4);
    if (!strcmp(upper,  "ISO-8859-5")) return(XML_CHAR_ENCODING_8859_5);
    if (!strcmp(upper,  "ISO-8859-6")) return(XML_CHAR_ENCODING_8859_6);
    if (!strcmp(upper,  "ISO-8859-7")) return(XML_CHAR_ENCODING_8859_7);
    if (!strcmp(upper,  "ISO-8859-8")) return(XML_CHAR_ENCODING_8859_8);
    if (!strcmp(upper,  "ISO-8859-9")) return(XML_CHAR_ENCODING_8859_9);

    if (!strcmp(upper, "ISO-2022-JP")) return(XML_CHAR_ENCODING_2022_JP);
    if (!strcmp(upper, "SHIFT_JIS")) return(XML_CHAR_ENCODING_SHIFT_JIS);
    if (!strcmp(upper, "EUC-JP")) return(XML_CHAR_ENCODING_EUC_JP);

#ifdef DEBUG_ENCODING
    xmlGenericError(xmlGenericErrorContext, "Unknown encoding %s\n", name);
#endif
    return(XML_CHAR_ENCODING_ERROR);
}

/**
 * xmlGetCharEncodingName:
 * @enc:  the encoding
 *
 * The "canonical" name for XML encoding.
 * C.f. http://www.w3.org/TR/REC-xml#charencoding
 * Section 4.3.3  Character Encoding in Entities
 *
 * Returns the canonical name for the given encoding
 */

const char*
xmlGetCharEncodingName(xmlCharEncoding enc) {
    switch (enc) {
        case XML_CHAR_ENCODING_ERROR:
	    return(NULL);
        case XML_CHAR_ENCODING_NONE:
	    return(NULL);
        case XML_CHAR_ENCODING_UTF8:
	    return("UTF-8");
        case XML_CHAR_ENCODING_UTF16LE:
	    return("UTF-16");
        case XML_CHAR_ENCODING_UTF16BE:
	    return("UTF-16");
        case XML_CHAR_ENCODING_EBCDIC:
            return("EBCDIC");
        case XML_CHAR_ENCODING_UCS4LE:
            return("ISO-10646-UCS-4");
        case XML_CHAR_ENCODING_UCS4BE:
            return("ISO-10646-UCS-4");
        case XML_CHAR_ENCODING_UCS4_2143:
            return("ISO-10646-UCS-4");
        case XML_CHAR_ENCODING_UCS4_3412:
            return("ISO-10646-UCS-4");
        case XML_CHAR_ENCODING_UCS2:
            return("ISO-10646-UCS-2");
        case XML_CHAR_ENCODING_8859_1:
	    return("ISO-8859-1");
        case XML_CHAR_ENCODING_8859_2:
	    return("ISO-8859-2");
        case XML_CHAR_ENCODING_8859_3:
	    return("ISO-8859-3");
        case XML_CHAR_ENCODING_8859_4:
	    return("ISO-8859-4");
        case XML_CHAR_ENCODING_8859_5:
	    return("ISO-8859-5");
        case XML_CHAR_ENCODING_8859_6:
	    return("ISO-8859-6");
        case XML_CHAR_ENCODING_8859_7:
	    return("ISO-8859-7");
        case XML_CHAR_ENCODING_8859_8:
	    return("ISO-8859-8");
        case XML_CHAR_ENCODING_8859_9:
	    return("ISO-8859-9");
        case XML_CHAR_ENCODING_2022_JP:
            return("ISO-2022-JP");
        case XML_CHAR_ENCODING_SHIFT_JIS:
            return("Shift-JIS");
        case XML_CHAR_ENCODING_EUC_JP:
            return("EUC-JP");
	case XML_CHAR_ENCODING_ASCII:
	    return(NULL);
    }
    return(NULL);
}

/************************************************************************
 *									*
 *			Char encoding handlers				*
 *									*
 ************************************************************************/


/* the size should be growable, but it's not a big deal ... */
#define MAX_ENCODING_HANDLERS 50
static xmlCharEncodingHandlerPtr *handlers = NULL;
static int nbCharEncodingHandler = 0;

/*
 * The default is UTF-8 for XML, that's also the default used for the
 * parser internals, so the default encoding handler is NULL
 */

static xmlCharEncodingHandlerPtr xmlDefaultCharEncodingHandler = NULL;

/**
 * xmlNewCharEncodingHandler:
 * @name:  the encoding name, in UTF-8 format (ASCII actually)
 * @input:  the xmlCharEncodingInputFunc to read that encoding
 * @output:  the xmlCharEncodingOutputFunc to write that encoding
 *
 * Create and registers an xmlCharEncodingHandler.
 *
 * Returns the xmlCharEncodingHandlerPtr created (or NULL in case of error).
 */
xmlCharEncodingHandlerPtr
xmlNewCharEncodingHandler(const char *name, 
                          xmlCharEncodingInputFunc input,
                          xmlCharEncodingOutputFunc output) {
    xmlCharEncodingHandlerPtr handler;
    const char *alias;
    char upper[500];
    int i;
    char *up = 0;

    /*
     * Do the alias resolution
     */
    alias = xmlGetEncodingAlias(name);
    if (alias != NULL)
	name = alias;

    /*
     * Keep only the uppercase version of the encoding.
     */
    if (name == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewCharEncodingHandler : no name !\n");
	return(NULL);
    }
    for (i = 0;i < 499;i++) {
        upper[i] = toupper(name[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;
    up = xmlMemStrdup(upper);
    if (up == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewCharEncodingHandler : out of memory !\n");
	return(NULL);
    }

    /*
     * allocate and fill-up an handler block.
     */
    handler = (xmlCharEncodingHandlerPtr)
              xmlMalloc(sizeof(xmlCharEncodingHandler));
    if (handler == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlNewCharEncodingHandler : out of memory !\n");
	return(NULL);
    }
    handler->input = input;
    handler->output = output;
    handler->name = up;

#ifdef LIBXML_ICONV_ENABLED
    handler->iconv_in = NULL;
    handler->iconv_out = NULL;
#endif /* LIBXML_ICONV_ENABLED */

    /*
     * registers and returns the handler.
     */
    xmlRegisterCharEncodingHandler(handler);
#ifdef DEBUG_ENCODING
    xmlGenericError(xmlGenericErrorContext,
	    "Registered encoding handler for %s\n", name);
#endif
    return(handler);
}

/**
 * xmlInitCharEncodingHandlers:
 *
 * Initialize the char encoding support, it registers the default
 * encoding supported.
 * NOTE: while public, this function usually doesn't need to be called
 *       in normal processing.
 */
void
xmlInitCharEncodingHandlers(void) {
    unsigned short int tst = 0x1234;
    unsigned char *ptr = (unsigned char *) &tst; 

    if (handlers != NULL) return;

    handlers = (xmlCharEncodingHandlerPtr *)
        xmlMalloc(MAX_ENCODING_HANDLERS * sizeof(xmlCharEncodingHandlerPtr));

    if (*ptr == 0x12) xmlLittleEndian = 0;
    else if (*ptr == 0x34) xmlLittleEndian = 1;
    else xmlGenericError(xmlGenericErrorContext,
	    "Odd problem at endianness detection\n");

    if (handlers == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlInitCharEncodingHandlers : out of memory !\n");
	return;
    }
    xmlNewCharEncodingHandler("UTF-8", NULL, NULL);
    xmlUTF16LEHandler = 
          xmlNewCharEncodingHandler("UTF-16LE", UTF16LEToUTF8, UTF8ToUTF16LE);
    xmlUTF16BEHandler = 
          xmlNewCharEncodingHandler("UTF-16BE", UTF16BEToUTF8, UTF8ToUTF16BE);
    xmlNewCharEncodingHandler("ISO-8859-1", isolat1ToUTF8, UTF8Toisolat1);
    xmlNewCharEncodingHandler("ASCII", asciiToUTF8, UTF8Toascii);
    xmlNewCharEncodingHandler("US-ASCII", asciiToUTF8, UTF8Toascii);
#ifdef LIBXML_HTML_ENABLED
    xmlNewCharEncodingHandler("HTML", NULL, UTF8ToHtml);
#endif
}

/**
 * xmlCleanupCharEncodingHandlers:
 *
 * Cleanup the memory allocated for the char encoding support, it
 * unregisters all the encoding handlers and the aliases.
 */
void
xmlCleanupCharEncodingHandlers(void) {
    xmlCleanupEncodingAliases();

    if (handlers == NULL) return;

    for (;nbCharEncodingHandler > 0;) {
        nbCharEncodingHandler--;
	if (handlers[nbCharEncodingHandler] != NULL) {
	    if (handlers[nbCharEncodingHandler]->name != NULL)
		xmlFree(handlers[nbCharEncodingHandler]->name);
	    xmlFree(handlers[nbCharEncodingHandler]);
	}
    }
    xmlFree(handlers);
    handlers = NULL;
    nbCharEncodingHandler = 0;
    xmlDefaultCharEncodingHandler = NULL;
}

/**
 * xmlRegisterCharEncodingHandler:
 * @handler:  the xmlCharEncodingHandlerPtr handler block
 *
 * Register the char encoding handler, surprising, isn't it ?
 */
void
xmlRegisterCharEncodingHandler(xmlCharEncodingHandlerPtr handler) {
    if (handlers == NULL) xmlInitCharEncodingHandlers();
    if (handler == NULL) {
        xmlGenericError(xmlGenericErrorContext,
		"xmlRegisterCharEncodingHandler: NULL handler !\n");
	return;
    }

    if (nbCharEncodingHandler >= MAX_ENCODING_HANDLERS) {
        xmlGenericError(xmlGenericErrorContext, 
	"xmlRegisterCharEncodingHandler: Too many handler registered\n");
        xmlGenericError(xmlGenericErrorContext,
		"\tincrease MAX_ENCODING_HANDLERS : %s\n", __FILE__);
	return;
    }
    handlers[nbCharEncodingHandler++] = handler;
}

/**
 * xmlGetCharEncodingHandler:
 * @enc:  an xmlCharEncoding value.
 *
 * Search in the registered set the handler able to read/write that encoding.
 *
 * Returns the handler or NULL if not found
 */
xmlCharEncodingHandlerPtr
xmlGetCharEncodingHandler(xmlCharEncoding enc) {
    xmlCharEncodingHandlerPtr handler;

    if (handlers == NULL) xmlInitCharEncodingHandlers();
    switch (enc) {
        case XML_CHAR_ENCODING_ERROR:
	    return(NULL);
        case XML_CHAR_ENCODING_NONE:
	    return(NULL);
        case XML_CHAR_ENCODING_UTF8:
	    return(NULL);
        case XML_CHAR_ENCODING_UTF16LE:
	    return(xmlUTF16LEHandler);
        case XML_CHAR_ENCODING_UTF16BE:
	    return(xmlUTF16BEHandler);
        case XML_CHAR_ENCODING_EBCDIC:
            handler = xmlFindCharEncodingHandler("EBCDIC");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("ebcdic");
            if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_UCS4BE:
            handler = xmlFindCharEncodingHandler("ISO-10646-UCS-4");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("UCS-4");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("UCS4");
            if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_UCS4LE:
            handler = xmlFindCharEncodingHandler("ISO-10646-UCS-4");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("UCS-4");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("UCS4");
            if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_UCS4_2143:
	    break;
        case XML_CHAR_ENCODING_UCS4_3412:
	    break;
        case XML_CHAR_ENCODING_UCS2:
            handler = xmlFindCharEncodingHandler("ISO-10646-UCS-2");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("UCS-2");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("UCS2");
            if (handler != NULL) return(handler);
	    break;

	    /*
	     * We used to keep ISO Latin encodings native in the
	     * generated data. This led to so many problems that
	     * this has been removed. One can still change this
	     * back by registering no-ops encoders for those
	     */
        case XML_CHAR_ENCODING_8859_1:
	    handler = xmlFindCharEncodingHandler("ISO-8859-1");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_2:
	    handler = xmlFindCharEncodingHandler("ISO-8859-2");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_3:
	    handler = xmlFindCharEncodingHandler("ISO-8859-3");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_4:
	    handler = xmlFindCharEncodingHandler("ISO-8859-4");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_5:
	    handler = xmlFindCharEncodingHandler("ISO-8859-5");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_6:
	    handler = xmlFindCharEncodingHandler("ISO-8859-6");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_7:
	    handler = xmlFindCharEncodingHandler("ISO-8859-7");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_8:
	    handler = xmlFindCharEncodingHandler("ISO-8859-8");
	    if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_8859_9:
	    handler = xmlFindCharEncodingHandler("ISO-8859-9");
	    if (handler != NULL) return(handler);
	    break;


        case XML_CHAR_ENCODING_2022_JP:
            handler = xmlFindCharEncodingHandler("ISO-2022-JP");
            if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_SHIFT_JIS:
            handler = xmlFindCharEncodingHandler("SHIFT-JIS");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("SHIFT_JIS");
            if (handler != NULL) return(handler);
            handler = xmlFindCharEncodingHandler("Shift_JIS");
            if (handler != NULL) return(handler);
	    break;
        case XML_CHAR_ENCODING_EUC_JP:
            handler = xmlFindCharEncodingHandler("EUC-JP");
            if (handler != NULL) return(handler);
	    break;
	default: 
	    break;
    }
    
#ifdef DEBUG_ENCODING
    xmlGenericError(xmlGenericErrorContext,
	    "No handler found for encoding %d\n", enc);
#endif
    return(NULL);
}

/**
 * xmlFindCharEncodingHandler:
 * @name:  a string describing the char encoding.
 *
 * Search in the registered set the handler able to read/write that encoding.
 *
 * Returns the handler or NULL if not found
 */
xmlCharEncodingHandlerPtr
xmlFindCharEncodingHandler(const char *name) {
    const char *nalias;
    const char *norig;
    xmlCharEncoding alias;
#ifdef LIBXML_ICONV_ENABLED
    xmlCharEncodingHandlerPtr enc;
    iconv_t icv_in, icv_out;
#endif /* LIBXML_ICONV_ENABLED */
    char upper[100];
    int i;

    if (handlers == NULL) xmlInitCharEncodingHandlers();
    if (name == NULL) return(xmlDefaultCharEncodingHandler);
    if (name[0] == 0) return(xmlDefaultCharEncodingHandler);

    /*
     * Do the alias resolution
     */
    norig = name;
    nalias = xmlGetEncodingAlias(name);
    if (nalias != NULL)
	name = nalias;

    /*
     * Check first for directly registered encoding names
     */
    for (i = 0;i < 99;i++) {
        upper[i] = toupper(name[i]);
	if (upper[i] == 0) break;
    }
    upper[i] = 0;

    for (i = 0;i < nbCharEncodingHandler; i++)
        if (!strcmp(upper, handlers[i]->name)) {
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
		    "Found registered handler for encoding %s\n", name);
#endif
	    return(handlers[i]);
	}

#ifdef LIBXML_ICONV_ENABLED
    /* check whether iconv can handle this */
    icv_in = iconv_open("UTF-8", name);
    icv_out = iconv_open(name, "UTF-8");
    if ((icv_in != (iconv_t) -1) && (icv_out != (iconv_t) -1)) {
	    enc = (xmlCharEncodingHandlerPtr)
	          xmlMalloc(sizeof(xmlCharEncodingHandler));
	    if (enc == NULL) {
	        iconv_close(icv_in);
	        iconv_close(icv_out);
		return(NULL);
	    }
	    enc->name = xmlMemStrdup(name);
	    enc->input = NULL;
	    enc->output = NULL;
	    enc->iconv_in = icv_in;
	    enc->iconv_out = icv_out;
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
		    "Found iconv handler for encoding %s\n", name);
#endif
	    return enc;
    } else if ((icv_in != (iconv_t) -1) || icv_out != (iconv_t) -1) {
	    xmlGenericError(xmlGenericErrorContext,
		    "iconv : problems with filters for '%s'\n", name);
    }
#endif /* LIBXML_ICONV_ENABLED */

#ifdef DEBUG_ENCODING
    xmlGenericError(xmlGenericErrorContext,
	    "No handler found for encoding %s\n", name);
#endif

    /*
     * Fallback using the canonical names
     */
    alias = xmlParseCharEncoding(norig);
    if (alias != XML_CHAR_ENCODING_ERROR) {
        const char* canon;
        canon = xmlGetCharEncodingName(alias);
        if ((canon != NULL) && (strcmp(name, canon))) {
	    return(xmlFindCharEncodingHandler(canon));
        }
    }

    return(NULL);
}

/************************************************************************
 *									*
 *		ICONV based generic conversion functions		*
 *									*
 ************************************************************************/

#ifdef LIBXML_ICONV_ENABLED
/**
 * xmlIconvWrapper:
 * @cd:		iconv converter data structure
 * @out:  a pointer to an array of bytes to store the result
 * @outlen:  the length of @out
 * @in:  a pointer to an array of ISO Latin 1 chars
 * @inlen:  the length of @in
 *
 * Returns 0 if success, or 
 *     -1 by lack of space, or
 *     -2 if the transcoding fails (for *in is not valid utf8 string or
 *        the result of transformation can't fit into the encoding we want), or
 *     -3 if there the last byte can't form a single output char.
 *     
 * The value of @inlen after return is the number of octets consumed
 *     as the return value is positive, else unpredictable.
 * The value of @outlen after return is the number of ocetes consumed.
 */
static int
xmlIconvWrapper(iconv_t cd,
    unsigned char *out, int *outlen,
    const unsigned char *in, int *inlen) {

    size_t icv_inlen = *inlen, icv_outlen = *outlen;
    const char *icv_in = (const char *) in;
    char *icv_out = (char *) out;
    int ret;

    ret = iconv(cd, (char **) &icv_in, &icv_inlen, &icv_out, &icv_outlen);
    if (in != NULL) {
        *inlen -= icv_inlen;
        *outlen -= icv_outlen;
    } else {
        *inlen = 0;
        *outlen = 0;
    }
    if ((icv_inlen != 0) || (ret == -1)) {
#ifdef EILSEQ
        if (errno == EILSEQ) {
            return -2;
        } else
#endif
#ifdef E2BIG
        if (errno == E2BIG) {
            return -1;
        } else
#endif
#ifdef EINVAL
        if (errno == EINVAL) {
            return -3;
        } else
#endif
        {
            return -3;
        }
    }
    return 0;
}
#endif /* LIBXML_ICONV_ENABLED */

/************************************************************************
 *									*
 *		The real API used by libxml for on-the-fly conversion	*
 *									*
 ************************************************************************/

/**
 * xmlCharEncFirstLine:
 * @handler:	char enconding transformation data structure
 * @out:  an xmlBuffer for the output.
 * @in:  an xmlBuffer for the input
 *     
 * Front-end for the encoding handler input function, but handle only
 * the very first line, i.e. limit itself to 45 chars.
 *     
 * Returns the number of byte written if success, or 
 *     -1 general error
 *     -2 if the transcoding fails (for *in is not valid utf8 string or
 *        the result of transformation can't fit into the encoding we want), or
 */
int
xmlCharEncFirstLine(xmlCharEncodingHandler *handler, xmlBufferPtr out,
                 xmlBufferPtr in) {
    int ret = -2;
    int written;
    int toconv;

    if (handler == NULL) return(-1);
    if (out == NULL) return(-1);
    if (in == NULL) return(-1);

    written = out->size - out->use;
    toconv = in->use;
    if (toconv * 2 >= written) {
        xmlBufferGrow(out, toconv);
	written = out->size - out->use - 1;
    }

    /*
     * echo '<?xml version="1.0" encoding="UCS4"?>' | wc -c => 38
     * 45 chars should be sufficient to reach the end of the encoding
     * declaration without going too far inside the document content.
     */
    written = 45;

    if (handler->input != NULL) {
	ret = handler->input(&out->content[out->use], &written,
	                     in->content, &toconv);
	xmlBufferShrink(in, toconv);
	out->use += written;
	out->content[out->use] = 0;
    }
#ifdef LIBXML_ICONV_ENABLED
    else if (handler->iconv_in != NULL) {
	ret = xmlIconvWrapper(handler->iconv_in, &out->content[out->use],
	                      &written, in->content, &toconv);
	xmlBufferShrink(in, toconv);
	out->use += written;
	out->content[out->use] = 0;
	if (ret == -1) ret = -3;
    }
#endif /* LIBXML_ICONV_ENABLED */
#ifdef DEBUG_ENCODING
    switch (ret) {
        case 0:
	    xmlGenericError(xmlGenericErrorContext,
		    "converted %d bytes to %d bytes of input\n",
	            toconv, written);
	    break;
        case -1:
	    xmlGenericError(xmlGenericErrorContext,"converted %d bytes to %d bytes of input, %d left\n",
	            toconv, written, in->use);
	    break;
        case -2:
	    xmlGenericError(xmlGenericErrorContext,
		    "input conversion failed due to input error\n");
	    break;
        case -3:
	    xmlGenericError(xmlGenericErrorContext,"converted %d bytes to %d bytes of input, %d left\n",
	            toconv, written, in->use);
	    break;
	default:
	    xmlGenericError(xmlGenericErrorContext,"Unknown input conversion failed %d\n", ret);
    }
#endif /* DEBUG_ENCODING */
    /*
     * Ignore when input buffer is not on a boundary
     */
    if (ret == -3) ret = 0;
    if (ret == -1) ret = 0;
    return(ret);
}

/**
 * xmlCharEncInFunc:
 * @handler:	char encoding transformation data structure
 * @out:  an xmlBuffer for the output.
 * @in:  an xmlBuffer for the input
 *     
 * Generic front-end for the encoding handler input function
 *     
 * Returns the number of byte written if success, or 
 *     -1 general error
 *     -2 if the transcoding fails (for *in is not valid utf8 string or
 *        the result of transformation can't fit into the encoding we want), or
 */
int
xmlCharEncInFunc(xmlCharEncodingHandler * handler, xmlBufferPtr out,
                 xmlBufferPtr in)
{
    int ret = -2;
    int written;
    int toconv;

    if (handler == NULL)
        return (-1);
    if (out == NULL)
        return (-1);
    if (in == NULL)
        return (-1);

    toconv = in->use;
    if (toconv == 0)
        return (0);
    written = out->size - out->use;
    if (toconv * 2 >= written) {
        xmlBufferGrow(out, out->size + toconv * 2);
        written = out->size - out->use - 1;
    }
    if (handler->input != NULL) {
        ret = handler->input(&out->content[out->use], &written,
                             in->content, &toconv);
        xmlBufferShrink(in, toconv);
        out->use += written;
        out->content[out->use] = 0;
    }
#ifdef LIBXML_ICONV_ENABLED
    else if (handler->iconv_in != NULL) {
        ret = xmlIconvWrapper(handler->iconv_in, &out->content[out->use],
                              &written, in->content, &toconv);
        xmlBufferShrink(in, toconv);
        out->use += written;
        out->content[out->use] = 0;
        if (ret == -1)
            ret = -3;
    }
#endif /* LIBXML_ICONV_ENABLED */
    switch (ret) {
        case 0:
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
                            "converted %d bytes to %d bytes of input\n",
                            toconv, written);
#endif
            break;
        case -1:
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
                         "converted %d bytes to %d bytes of input, %d left\n",
                            toconv, written, in->use);
#endif
            break;
        case -3:
#ifdef DEBUG_ENCODING
            xmlGenericError(xmlGenericErrorContext,
                        "converted %d bytes to %d bytes of input, %d left\n",
                            toconv, written, in->use);
#endif
            break;
        case -2:
            xmlGenericError(xmlGenericErrorContext,
                            "input conversion failed due to input error\n");
            xmlGenericError(xmlGenericErrorContext,
                            "Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
                            in->content[0], in->content[1],
                            in->content[2], in->content[3]);
    }
    /*
     * Ignore when input buffer is not on a boundary
     */
    if (ret == -3)
        ret = 0;
    return (written);
}

/**
 * xmlCharEncOutFunc:
 * @handler:	char enconding transformation data structure
 * @out:  an xmlBuffer for the output.
 * @in:  an xmlBuffer for the input
 *     
 * Generic front-end for the encoding handler output function
 * a first call with @in == NULL has to be made firs to initiate the 
 * output in case of non-stateless encoding needing to initiate their
 * state or the output (like the BOM in UTF16).
 * In case of UTF8 sequence conversion errors for the given encoder,
 * the content will be automatically remapped to a CharRef sequence.
 *     
 * Returns the number of byte written if success, or 
 *     -1 general error
 *     -2 if the transcoding fails (for *in is not valid utf8 string or
 *        the result of transformation can't fit into the encoding we want), or
 */
int
xmlCharEncOutFunc(xmlCharEncodingHandler *handler, xmlBufferPtr out,
                  xmlBufferPtr in) {
    int ret = -2;
    int written;
    int writtentot = 0;
    int toconv;
    int output = 0;

    if (handler == NULL) return(-1);
    if (out == NULL) return(-1);

retry:
    
    written = out->size - out->use;

    /*
     * First specific handling of in = NULL, i.e. the initialization call
     */
    if (in == NULL) {
        toconv = 0;
	if (handler->output != NULL) {
	    ret = handler->output(&out->content[out->use], &written,
				  NULL, &toconv);
	    out->use += written;
	    out->content[out->use] = 0;
	}
#ifdef LIBXML_ICONV_ENABLED
	else if (handler->iconv_out != NULL) {
	    ret = xmlIconvWrapper(handler->iconv_out, &out->content[out->use],
				  &written, NULL, &toconv);
	    out->use += written;
	    out->content[out->use] = 0;
	}
#endif /* LIBXML_ICONV_ENABLED */
#ifdef DEBUG_ENCODING
	xmlGenericError(xmlGenericErrorContext,
		"initialized encoder\n");
#endif
        return(0);
    }

    /*
     * Conversion itself.
     */
    toconv = in->use;
    if (toconv == 0)
	return(0);
    if (toconv * 2 >= written) {
        xmlBufferGrow(out, toconv * 2);
	written = out->size - out->use - 1;
    }
    if (handler->output != NULL) {
	ret = handler->output(&out->content[out->use], &written,
	                      in->content, &toconv);
	xmlBufferShrink(in, toconv);
	out->use += written;
	writtentot += written;
	out->content[out->use] = 0;
    }
#ifdef LIBXML_ICONV_ENABLED
    else if (handler->iconv_out != NULL) {
	ret = xmlIconvWrapper(handler->iconv_out, &out->content[out->use],
	                      &written, in->content, &toconv);
	xmlBufferShrink(in, toconv);
	out->use += written;
	writtentot += written;
	out->content[out->use] = 0;
	if (ret == -1) {
	    if (written > 0) {
		/*
		 * Can be a limitation of iconv
		 */
		goto retry;
	    }
	    ret = -3;
	}
    }
#endif /* LIBXML_ICONV_ENABLED */
    else {
	xmlGenericError(xmlGenericErrorContext,
		"xmlCharEncOutFunc: no output function !\n");
	return(-1);
    }

    if (ret >= 0) output += ret;

    /*
     * Attempt to handle error cases
     */
    switch (ret) {
        case 0:
#ifdef DEBUG_ENCODING
	    xmlGenericError(xmlGenericErrorContext,
		    "converted %d bytes to %d bytes of output\n",
	            toconv, written);
#endif
	    break;
        case -1:
#ifdef DEBUG_ENCODING
	    xmlGenericError(xmlGenericErrorContext,
		    "output conversion failed by lack of space\n");
#endif
	    break;
        case -3:
	    xmlGenericError(xmlGenericErrorContext,"converted %d bytes to %d bytes of output %d left\n",
	            toconv, written, in->use);
	    break;
        case -2: {
	    int len = in->use;
	    const xmlChar *utf = (const xmlChar *) in->content;
	    int cur;

	    cur = xmlGetUTF8Char(utf, &len);
	    if (cur > 0) {
		xmlChar charref[20];

#ifdef DEBUG_ENCODING
		xmlGenericError(xmlGenericErrorContext,
			"handling output conversion error\n");
		xmlGenericError(xmlGenericErrorContext,
			"Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			in->content[0], in->content[1],
			in->content[2], in->content[3]);
#endif
		/*
		 * Removes the UTF8 sequence, and replace it by a charref
		 * and continue the transcoding phase, hoping the error
		 * did not mangle the encoder state.
		 */
		snprintf((char *) charref, sizeof(charref), "&#%d;", cur);
		xmlBufferShrink(in, len);
		xmlBufferAddHead(in, charref, -1);

		goto retry;
	    } else {
		xmlGenericError(xmlGenericErrorContext,
			"output conversion failed due to conv error\n");
		xmlGenericError(xmlGenericErrorContext,
			"Bytes: 0x%02X 0x%02X 0x%02X 0x%02X\n",
			in->content[0], in->content[1],
			in->content[2], in->content[3]);
		in->content[0] = ' ';
	    }
	    break;
	}
    }
    return(ret);
}

/**
 * xmlCharEncCloseFunc:
 * @handler:	char enconding transformation data structure
 *     
 * Generic front-end for encoding handler close function
 *
 * Returns 0 if success, or -1 in case of error
 */
int
xmlCharEncCloseFunc(xmlCharEncodingHandler *handler) {
    int ret = 0;
    if (handler == NULL) return(-1);
    if (handler->name == NULL) return(-1);
#ifdef LIBXML_ICONV_ENABLED
    /*
     * Iconv handlers can be used only once, free the whole block.
     * and the associated icon resources.
     */
    if ((handler->iconv_out != NULL) || (handler->iconv_in != NULL)) {
	if (handler->name != NULL)
	    xmlFree(handler->name);
	handler->name = NULL;
	if (handler->iconv_out != NULL) {
	    if (iconv_close(handler->iconv_out))
		ret = -1;
	    handler->iconv_out = NULL;
	}
	if (handler->iconv_in != NULL) {
	    if (iconv_close(handler->iconv_in))
		ret = -1;
	    handler->iconv_in = NULL;
	}
	xmlFree(handler);
    }
#endif /* LIBXML_ICONV_ENABLED */
#ifdef DEBUG_ENCODING
    if (ret)
        xmlGenericError(xmlGenericErrorContext,
		"failed to close the encoding handler\n");
    else
        xmlGenericError(xmlGenericErrorContext,
		"closed the encoding handler\n");
#endif

    return(ret);
}

