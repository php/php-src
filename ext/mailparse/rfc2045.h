/*
** Copyright 1998 - 2000 Double Precision, Inc.  See COPYING for
** distribution information.
*/

/*
** $Id$
*/
#ifndef	rfc2045_h
#define	rfc2045_h

#include "php_mailparse.h"
#include "ext/mbstring/mbfilter.h"

#define	RFC2045CHARSET	"us-ascii"
#define	RFC2045MIMEMSG	"This is a MIME-formatted message.\n"


#ifdef  __cplusplus
extern "C" {
#endif

#define	RFC2045_ISMIME1(p)	((p) && atoi(p) == 1)
#define	RFC2045_ISMIME1DEF(p)	(!(p) || atoi(p) == 1)

struct rfc2045;

/* callback for de/encoding */
typedef int (*rfc2045_decode_user_func_t)(const char *p, size_t n, void *ptr);
typedef	int	(*rfc2045_decode_func_t)(struct rfc2045 * part, const char * buf, size_t n);

/* the attributes of a given header */
struct rfc2045attr {
	struct rfc2045attr *next;
	char *name;
	char *value;
};

struct rfc2045 {
	struct rfc2045 *parent;
	unsigned pindex;
	struct rfc2045 *next;

	off_t	startpos,	/* At which offset in msg this section starts */
		endpos,		/* Where it ends */
		startbody,	/* Where the body of the msg starts */
		endbody;	/* endpos - trailing CRLF terminator */
	off_t	nlines;		/* Number of lines in message */
	off_t	nbodylines;	/* Number of lines only in the body */
	char *mime_version;
	char *content_type;
	struct rfc2045attr *content_type_attr;	/* Content-Type: attributes */

	char *content_disposition;
	char *boundary;
	struct rfc2045attr *content_disposition_attr;
	char *content_transfer_encoding;
	/* Set if content_transfer_encoding is 8bit */
	int content_8bit;
	char *content_id;
	char *content_description;
	char *content_language;
	char *content_md5;
	char *content_base;
	char *content_location;
	struct  rfc2045ac *rfc2045acptr;
	int	has8bitchars;	/* For rewriting */
	int	haslongline;	/* For rewriting */
	unsigned rfcviolation;	/* Boo-boos */

#define	RFC2045_ERR8BITHEADER	1	/* 8 bit characters in headers */
#define	RFC2045_ERR8BITCONTENT	2	/* 8 bit contents, but no 8bit content-transfer-encoding */
#define	RFC2045_ERR2COMPLEX	4	/* Too many nested contents */
#define RFC2045_ERRNOMIMEVERSION 8	/* missing Mime-Version header, but boundary set in content type */
	unsigned numparts;	/* # of parts allocated */

	char	*rw_transfer_encoding;	/* For rewriting */

#define	RFC2045_RW_7BIT	1
#define	RFC2045_RW_8BIT	2

	/* Subsections */

	struct rfc2045 *firstpart, *lastpart;

	/* Working area */

	char *workbuf;
	size_t workbufsize;
	size_t workbuflen;
	int	workinheader;
	int	workclosed;
	int	isdummy;
	int	informdata;	/* In a middle of a long form-data part */
	char *header;
	size_t headersize;
	size_t headerlen;

	zval * headerhash;	/* a record of all of the headers */

	/* decoding filter to use */
	mbfl_convert_filter * decode_filter;
	/* "user" function to accept the decoding output */
	rfc2045_decode_user_func_t udecode_func;
	/* this is passed as the last param to the user decode func */
	void	*misc_decode_ptr;
} ;


struct rfc2045 *rfc2045_alloc();
void rfc2045_parse(struct rfc2045 *, const char *, size_t);
void rfc2045_free(struct rfc2045 *);


const char *rfc2045_contentname(const struct rfc2045 *);
void rfc2045_mimeinfo(const struct rfc2045 *,
	const char **,
	const char **,
	const char **);
const char *rfc2045_boundary(const struct rfc2045 *);
char *rfc2045_related_start(const struct rfc2045 *);
const char *rfc2045_content_id(const struct rfc2045 *);
const char *rfc2045_content_description(const struct rfc2045 *);
const char *rfc2045_content_language(const struct rfc2045 *);
const char *rfc2045_content_md5(const struct rfc2045 *);

void rfc2045_dispositioninfo(const struct rfc2045 *,
	const char **,
	const char **,
	const char **);

void rfc2045_mimepos(const struct rfc2045 *, off_t *, off_t *, off_t *,
	off_t *, off_t *);
unsigned rfc2045_mimepartcount(const struct rfc2045 *);

struct rfc2045id {
	struct rfc2045id *next;
	int idnum;
} ;

void rfc2045_decode(struct rfc2045 *,
	void (*)(struct rfc2045 *, struct rfc2045id *, void *),
	void *);

struct rfc2045 *rfc2045_find(struct rfc2045 *, const char *);




/* begin an en/decoding process */
void rfc2045_cdecode_start(struct rfc2045 *, rfc2045_decode_user_func_t cb, void *);
int rfc2045_cdecode(struct rfc2045 *, const char *, size_t);
int rfc2045_cdecode_end(struct rfc2045 *);

struct  rfc2045ac {
	void (*start_section)(struct rfc2045ac *, struct rfc2045 *);
	void (*section_contents)(struct rfc2045ac *, const char *, size_t);
	void (*end_section)(struct rfc2045ac *);
	/* private vars used in acprep */
	int curlinepos;
	struct rfc2045 *currwp;
	enum {
		raw,
		quotedprint,
		qpseeneq,
		qpseeneqh,
		base64
	} curstate;
	int statechar;


};

struct rfc2045 *rfc2045_alloc_ac();
int rfc2045_ac_check(struct rfc2045 *, int);
int rfc2045_rewrite(struct rfc2045 *, int, int, const char *);
int rfc2045_rewrite_func(struct rfc2045 *p, int,
	int (*)(const char *, int, void *), void *,
	const char *);

/* Internal functions */

int rfc2045_try_boundary(struct rfc2045 *, int, const char *);
char *rfc2045_mk_boundary(struct rfc2045 *, int);
const char *rfc2045_getattr(const struct rfc2045attr *, const char *);
void rfc2045_setattr(struct rfc2045attr **, const char *, const char *);

/* MIME content base/location */

char *rfc2045_content_base(struct rfc2045 *p);
	/* This joins Content-Base: and Content-Location:, as best as I
	** can figure it out.
	*/

char *rfc2045_append_url(const char *, const char *);
	/* Do this with two arbitrary URLs */

#ifdef  __cplusplus
}
#endif

#endif
