#ifndef RUBY_WIN32_DIR_H
#define RUBY_WIN32_DIR_H

#ifdef __BORLANDC__
#  ifndef WIN32_DIR_H_
#    define WIN32_DIR_H_
#    include <sys/types.h>
#  endif
#endif

struct direct
{
    long d_namlen;
    ino_t d_ino;
    char *d_name;
    char d_isdir; /* directory */
    char d_isrep; /* reparse point */
};
typedef struct {
    WCHAR *start;
    WCHAR *curr;
    long size;
    long nfiles;
    long loc;  /* [0, nfiles) */
    struct direct dirstr;
    char *bits;  /* used for d_isdir and d_isrep */
} DIR;


DIR*           rb_w32_opendir(const char*);
DIR*           rb_w32_uopendir(const char*);
struct direct* rb_w32_readdir(DIR *);
struct direct* rb_w32_readdir_with_enc(DIR *, rb_encoding *);
long           rb_w32_telldir(DIR *);
void           rb_w32_seekdir(DIR *, long);
void           rb_w32_rewinddir(DIR *);
void           rb_w32_closedir(DIR *);

#define opendir   rb_w32_opendir
#define readdir   rb_w32_readdir
#define telldir   rb_w32_telldir
#define seekdir   rb_w32_seekdir
#define rewinddir rb_w32_rewinddir
#define closedir  rb_w32_closedir

#endif /* RUBY_WIN32_DIR_H */
