/*

	   Encode or decode file as MIME base64 (RFC 1341)

			    by John Walker
		       http://www.fourmilab.ch/

		This program is in the public domain.

*/


struct buffer_st {
  char *data;
  int length;
  char *ptr;
  int offset;
};

void buffer_new(struct buffer_st *b);
void buffer_add(struct buffer_st *b, char c);
void buffer_delete(struct buffer_st *b);

void base64_encode_xmlrpc(struct buffer_st *b, const char *source, int length);
void base64_decode_xmlrpc(struct buffer_st *b, const char *source, int length);

/*
#define DEBUG_MALLOC
 */

#ifdef DEBUG_MALLOC
void *_malloc_real(size_t s, char *file, int line);
void _free_real(void *p, char *file, int line);

#define malloc(s)	_malloc_real(s,__FILE__,__LINE__)
#define free(p)		_free_real(p, __FILE__,__LINE__)
#endif

