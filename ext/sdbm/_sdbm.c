/*
 * sdbm - ndbm work-alike hashed database library
 * based on Per-Aake Larson's Dynamic Hashing algorithms. BIT 18 (1978).
 * author: oz@nexus.yorku.ca
 * status: public domain.
 *
 * core routines
 */

#include "ruby/config.h"
#include "ruby/defines.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "sdbm.h"

/*
 * sdbm - ndbm work-alike hashed database library
 * tuning and portability constructs [not nearly enough]
 * author: oz@nexus.yorku.ca
 */

#define BYTESIZ		8

#ifdef BSD42
#define SEEK_SET	L_SET
#define	memset(s,c,n)	bzero((s), (n))		/* only when c is zero */
#define	memcpy(s1,s2,n)	bcopy((s2), (s1), (n))
#define	memcmp(s1,s2,n)	bcmp((s1),(s2),(n))
#endif

/*
 * important tuning parms (hah)
 */

#ifndef SEEDUPS
#define SEEDUPS 1	/* always detect duplicates */
#endif
#ifndef BADMESS
#define BADMESS 1	/* generate a message for worst case:
			   cannot make room after SPLTMAX splits */
#endif

/*
 * misc
 */
#ifdef DEBUG
#define debug(x)	printf x
#else
#define debug(x)
#endif

#ifdef BIG_E
#define GET_SHORT(p, i)	(((unsigned)((unsigned char *)(p))[(i)*2] << 8) + (((unsigned char *)(p))[(i)*2 + 1]))
#define PUT_SHORT(p, i, s) (((unsigned char *)(p))[(i)*2] = (unsigned char)((s) >> 8), ((unsigned char *)(p))[(i)*2 + 1] = (unsigned char)(s))
#else
#define GET_SHORT(p, i)	((p)[(i)])
#define PUT_SHORT(p, i, s)	((p)[(i)] = (s))
#endif

/*#include "pair.h"*/
static int   fitpair proto((char *, int));
static void  putpair proto((char *, datum, datum));
static datum getpair proto((char *, datum));
static int   delpair proto((char *, datum));
static int   chkpage proto((char *));
static datum getnkey proto((char *, int));
static void  splpage proto((char *, char *, long));
#if SEEDUPS
static int   duppair proto((char *, datum));
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef DOSISH
#include <io.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef BSD42
#include <sys/file.h>
#else
#include <fcntl.h>
/*#include <memory.h>*/
#endif
#ifndef O_BINARY
#define O_BINARY	0
#endif

#include <errno.h>
#ifndef EPERM
#define EPERM	EACCES
#endif
#include <string.h>

#ifdef __STDC__
#include <stddef.h>
#endif

#ifndef NULL
#define NULL	0
#endif

/*
 * externals
 */
#if !defined(__sun) && !defined(_WIN32) && !defined(__CYGWIN__) && !defined(errno)
extern int errno;
#endif

/*
 * forward
 */
static int getdbit proto((DBM *, long));
static int setdbit proto((DBM *, long));
static int getpage proto((DBM *, long));
static datum getnext proto((DBM *));
static int makroom proto((DBM *, long, int));

/*
 * useful macros
 */
#define bad(x)		((x).dptr == NULL || (x).dsize < 0)
#define exhash(item)	sdbm_hash((item).dptr, (item).dsize)
#define ioerr(db)	((db)->flags |= DBM_IOERR)

#define OFF_PAG(off)	(long) (off) * PBLKSIZ
#define OFF_DIR(off)	(long) (off) * DBLKSIZ

static long masks[] = {
	000000000000L, 000000000001L, 000000000003L,
	000000000007L, 000000000017L, 000000000037L,
	000000000077L, 000000000177L, 000000000377L,
	000000000777L, 000000001777L, 000000003777L,
	000000007777L, 000000017777L, 000000037777L,
	000000077777L, 000000177777L, 000000377777L,
	000000777777L, 000001777777L, 000003777777L,
	000007777777L, 000017777777L, 000037777777L,
	000077777777L, 000177777777L, 000377777777L,
	000777777777L, 001777777777L, 003777777777L,
	007777777777L, 017777777777L
};

datum nullitem = {NULL, 0};

DBM *
sdbm_open(register char *file, register int flags, register int mode)
{
	register DBM *db;
	register char *dirname;
	register char *pagname;
	register size_t n;

	if (file == NULL || !*file)
		return errno = EINVAL, (DBM *) NULL;
/*
 * need space for two seperate filenames
 */
	n = strlen(file) * 2 + strlen(DIRFEXT) + strlen(PAGFEXT) + 2;

	if ((dirname = malloc(n)) == NULL)
		return errno = ENOMEM, (DBM *) NULL;
/*
 * build the file names
 */
	dirname = strcat(strcpy(dirname, file), DIRFEXT);
	pagname = strcpy(dirname + strlen(dirname) + 1, file);
	pagname = strcat(pagname, PAGFEXT);

	db = sdbm_prep(dirname, pagname, flags, mode);
	free((char *) dirname);
	return db;
}

static int
fd_set_cloexec(int fd)
{
  /* MinGW don't have F_GETFD and FD_CLOEXEC.  [ruby-core:40281] */
#ifdef F_GETFD
    int flags, ret;
    flags = fcntl(fd, F_GETFD); /* should not fail except EBADF. */
    if (flags == -1) {
        return -1;
    }
    if (2 < fd) {
        if (!(flags & FD_CLOEXEC)) {
            flags |= FD_CLOEXEC;
            ret = fcntl(fd, F_SETFD, flags);
            if (ret == -1) {
                return -1;
            }
        }
    }
#endif
    return 0;
}

DBM *
sdbm_prep(char *dirname, char *pagname, int flags, int mode)
{
	register DBM *db;
	struct stat dstat;

	if ((db = (DBM *) malloc(sizeof(DBM))) == NULL)
		return errno = ENOMEM, (DBM *) NULL;

        db->pagf = -1;
        db->dirf = -1;
        db->flags = 0;
        db->hmask = 0;
        db->blkptr = 0;
        db->keyptr = 0;
/*
 * adjust user flags so that WRONLY becomes RDWR,
 * as required by this package. Also set our internal
 * flag for RDONLY.
 */
	if (flags & O_WRONLY)
		flags = (flags & ~O_WRONLY) | O_RDWR;
	if (flags & O_RDONLY)
		db->flags = DBM_RDONLY;
/*
 * open the files in sequence, and stat the dirfile.
 * If we fail anywhere, undo everything, return NULL.
 */
	flags |= O_BINARY;
#ifdef O_CLOEXEC
        flags |= O_CLOEXEC;
#endif

	if ((db->pagf = open(pagname, flags, mode)) == -1) goto err;
        if (fd_set_cloexec(db->pagf) == -1) goto err;
        if ((db->dirf = open(dirname, flags, mode)) == -1) goto err;
        if (fd_set_cloexec(db->dirf) == -1) goto err;
/*
 * need the dirfile size to establish max bit number.
 */
        if (fstat(db->dirf, &dstat) == -1) goto err;
/*
 * zero size: either a fresh database, or one with a single,
 * unsplit data page: dirpage is all zeros.
 */
        db->dirbno = (!dstat.st_size) ? 0 : -1;
        db->pagbno = -1;
        db->maxbno = dstat.st_size * (long) BYTESIZ;

        (void) memset(db->pagbuf, 0, PBLKSIZ);
        (void) memset(db->dirbuf, 0, DBLKSIZ);
/*
 * success
 */
        return db;

    err:
        if (db->pagf != -1)
                (void) close(db->pagf);
        if (db->dirf != -1)
                (void) close(db->dirf);
	free((char *) db);
	return (DBM *) NULL;
}

void
sdbm_close(register DBM *db)
{
	if (db == NULL)
		errno = EINVAL;
	else {
		(void) close(db->dirf);
		(void) close(db->pagf);
		free((char *) db);
	}
}

datum
sdbm_fetch(register DBM *db, datum key)
{
	if (db == NULL || bad(key))
		return errno = EINVAL, nullitem;

	if (getpage(db, exhash(key)))
		return getpair(db->pagbuf, key);

	return ioerr(db), nullitem;
}

int
sdbm_delete(register DBM *db, datum key)
{
	if (db == NULL || bad(key))
		return errno = EINVAL, -1;
	if (sdbm_rdonly(db))
		return errno = EPERM, -1;

	if (getpage(db, exhash(key))) {
		if (!delpair(db->pagbuf, key))
			return -1;
/*
 * update the page file
 */
		if (lseek(db->pagf, OFF_PAG(db->pagbno), SEEK_SET) < 0
		    || write(db->pagf, db->pagbuf, PBLKSIZ) < 0)
			return ioerr(db), -1;

		return 0;
	}

	return ioerr(db), -1;
}

int
sdbm_store(register DBM *db, datum key, datum val, int flags)
{
	int need;
	register long hash;

	if (db == NULL || bad(key))
		return errno = EINVAL, -1;
	if (sdbm_rdonly(db))
		return errno = EPERM, -1;

	need = key.dsize + val.dsize;
/*
 * is the pair too big (or too small) for this database ??
 */
	if (need < 0 || need > PAIRMAX)
		return errno = EINVAL, -1;

	if (getpage(db, (hash = exhash(key)))) {
/*
 * if we need to replace, delete the key/data pair
 * first. If it is not there, ignore.
 */
		if (flags == DBM_REPLACE)
			(void) delpair(db->pagbuf, key);
#if SEEDUPS
		else if (duppair(db->pagbuf, key))
			return 1;
#endif
/*
 * if we do not have enough room, we have to split.
 */
		if (!fitpair(db->pagbuf, need))
			if (!makroom(db, hash, need))
				return ioerr(db), -1;
/*
 * we have enough room or split is successful. insert the key,
 * and update the page file.
 */
		(void) putpair(db->pagbuf, key, val);

		if (lseek(db->pagf, OFF_PAG(db->pagbno), SEEK_SET) < 0
		    || write(db->pagf, db->pagbuf, PBLKSIZ) < 0)
			return ioerr(db), -1;
	/*
	 * success
	 */
		return 0;
	}

	return ioerr(db), -1;
}

/*
 * makroom - make room by splitting the overfull page
 * this routine will attempt to make room for SPLTMAX times before
 * giving up.
 */
static int
makroom(register DBM *db, long int hash, int need)
{
	long newp;
	char twin[PBLKSIZ];
#if defined _WIN32 && !defined __CYGWIN__
	char zer[PBLKSIZ];
	long oldtail;
#endif
	char *pag = db->pagbuf;
	char *new = twin;
	register int smax = SPLTMAX;

	do {
/*
 * split the current page
 */
		(void) splpage(pag, new, db->hmask + 1);
/*
 * address of the new page
 */
		newp = (hash & db->hmask) | (db->hmask + 1);
		debug(("newp: %ld\n", newp));
/*
 * write delay, read avoidence/cache shuffle:
 * select the page for incoming pair: if key is to go to the new page,
 * write out the previous one, and copy the new one over, thus making
 * it the current page. If not, simply write the new page, and we are
 * still looking at the page of interest. current page is not updated
 * here, as sdbm_store will do so, after it inserts the incoming pair.
 */

#if defined _WIN32 && !defined __CYGWIN__
	/*
	 * Fill hole with 0 if made it.
	 * (hole is NOT read as 0)
	 */
	oldtail = lseek(db->pagf, 0L, SEEK_END);
	memset(zer, 0, PBLKSIZ);
	while (OFF_PAG(newp) > oldtail) {
		if (lseek(db->pagf, 0L, SEEK_END) < 0 ||
		    write(db->pagf, zer, PBLKSIZ) < 0) {

			return 0;
		}
		oldtail += PBLKSIZ;
	}
#endif

		if (hash & (db->hmask + 1)) {
			if (lseek(db->pagf, OFF_PAG(db->pagbno), SEEK_SET) < 0
			    || write(db->pagf, db->pagbuf, PBLKSIZ) < 0)
				return 0;
			db->pagbno = newp;
			(void) memcpy(pag, new, PBLKSIZ);
		}
		else if (lseek(db->pagf, OFF_PAG(newp), SEEK_SET) < 0
			 || write(db->pagf, new, PBLKSIZ) < 0)
			return 0;

		if (!setdbit(db, db->curbit))
			return 0;
/*
 * see if we have enough room now
 */
		if (fitpair(pag, need))
			return 1;
/*
 * try again... update curbit and hmask as getpage would have
 * done. because of our update of the current page, we do not
 * need to read in anything. BUT we have to write the current
 * [deferred] page out, as the window of failure is too great.
 */
		db->curbit = 2 * db->curbit +
			((hash & (db->hmask + 1)) ? 2 : 1);
		db->hmask |= (db->hmask + 1);

		if (lseek(db->pagf, OFF_PAG(db->pagbno), SEEK_SET) < 0
		    || write(db->pagf, db->pagbuf, PBLKSIZ) < 0)
			return 0;

	} while (--smax);
/*
 * if we are here, this is real bad news. After SPLTMAX splits,
 * we still cannot fit the key. say goodnight.
 */
#if BADMESS
	(void) (write(2, "sdbm: cannot insert after SPLTMAX attempts.\n", 44) < 0);
#endif
	return 0;

}

/*
 * the following two routines will break if
 * deletions aren't taken into account. (ndbm bug)
 */
datum
sdbm_firstkey(register DBM *db)
{
	if (db == NULL)
		return errno = EINVAL, nullitem;
/*
 * start at page 0
 */
	(void) memset(db->pagbuf, 0, PBLKSIZ);
	if (lseek(db->pagf, OFF_PAG(0), SEEK_SET) < 0
	    || read(db->pagf, db->pagbuf, PBLKSIZ) < 0)
		return ioerr(db), nullitem;
	db->pagbno = 0;
	db->blkptr = 0;
	db->keyptr = 0;

	return getnext(db);
}

datum
sdbm_nextkey(register DBM *db)
{
	if (db == NULL)
		return errno = EINVAL, nullitem;
	return getnext(db);
}

/*
 * all important binary trie traversal
 */
static int
getpage(register DBM *db, register long int hash)
{
	register int hbit;
	register long dbit;
	register long pagb;

	dbit = 0;
	hbit = 0;
	while (dbit < db->maxbno && getdbit(db, dbit))
		dbit = 2 * dbit + ((hash & ((long) 1 << hbit++)) ? 2 : 1);

	debug(("dbit: %d...", dbit));

	db->curbit = dbit;
	db->hmask = masks[hbit];

	pagb = hash & db->hmask;
/*
 * see if the block we need is already in memory.
 * note: this lookaside cache has about 10% hit rate.
 */
	if (pagb != db->pagbno) {
/*
 * note: here, we assume a "hole" is read as 0s.
 * if not, must zero pagbuf first.
 */
		(void) memset(db->pagbuf, 0, PBLKSIZ);

		if (lseek(db->pagf, OFF_PAG(pagb), SEEK_SET) < 0
		    || read(db->pagf, db->pagbuf, PBLKSIZ) < 0)
			return 0;
		if (!chkpage(db->pagbuf)) {
			return 0;
		}
		db->pagbno = pagb;

		debug(("pag read: %d\n", pagb));
	}
	return 1;
}

static int
getdbit(register DBM *db, register long int dbit)
{
	register long c;
	register long dirb;

	c = dbit / BYTESIZ;
	dirb = c / DBLKSIZ;

	if (dirb != db->dirbno) {
		if (lseek(db->dirf, OFF_DIR(dirb), SEEK_SET) < 0
		    || read(db->dirf, db->dirbuf, DBLKSIZ) < 0)
			return 0;
		db->dirbno = dirb;

		debug(("dir read: %d\n", dirb));
	}

	return db->dirbuf[c % DBLKSIZ] & (1 << (dbit % BYTESIZ));
}

static int
setdbit(register DBM *db, register long int dbit)
{
	register long c;
	register long dirb;

	c = dbit / BYTESIZ;
	dirb = c / DBLKSIZ;

	if (dirb != db->dirbno) {
		if (lseek(db->dirf, OFF_DIR(dirb), SEEK_SET) < 0
		    || read(db->dirf, db->dirbuf, DBLKSIZ) < 0)
			return 0;
		db->dirbno = dirb;

		debug(("dir read: %d\n", dirb));
	}

	db->dirbuf[c % DBLKSIZ] |= (1 << (dbit % BYTESIZ));

	if (dbit >= db->maxbno)
		db->maxbno += (long) DBLKSIZ * BYTESIZ;

	if (lseek(db->dirf, OFF_DIR(dirb), SEEK_SET) < 0
	    || write(db->dirf, db->dirbuf, DBLKSIZ) < 0)
		return 0;

	return 1;
}

/*
 * getnext - get the next key in the page, and if done with
 * the page, try the next page in sequence
 */
static datum
getnext(register DBM *db)
{
	datum key;

	for (;;) {
		db->keyptr++;
		key = getnkey(db->pagbuf, db->keyptr);
		if (key.dptr != NULL)
			return key;
/*
 * we either run out, or there is nothing on this page..
 * try the next one... If we lost our position on the
 * file, we will have to seek.
 */
		db->keyptr = 0;
		if (db->pagbno != db->blkptr++)
			if (lseek(db->pagf, OFF_PAG(db->blkptr), SEEK_SET) < 0)
				break;
		db->pagbno = db->blkptr;
		if (read(db->pagf, db->pagbuf, PBLKSIZ) <= 0)
			break;
		if (!chkpage(db->pagbuf)) {
			break;
		}
	}

	return ioerr(db), nullitem;
}

/* pair.c */
/*
 * sdbm - ndbm work-alike hashed database library
 * based on Per-Aake Larson's Dynamic Hashing algorithms. BIT 18 (1978).
 * author: oz@nexus.yorku.ca
 * status: public domain.
 *
 * page-level routines
 */

#ifndef BSD42
/*#include <memory.h>*/
#endif

#define exhash(item)	sdbm_hash((item).dptr, (item).dsize)

/*
 * forward
 */
static int seepair proto((char *, int, char *, int));

/*
 * page format:
 *	+------------------------------+
 * ino	| n | keyoff | datoff | keyoff |
 * 	+------------+--------+--------+
 *	| datoff | - - - ---->	       |
 *	+--------+---------------------+
 *	|	 F R E E A R E A       |
 *	+--------------+---------------+
 *	|  <---- - - - | data          |
 *	+--------+-----+----+----------+
 *	|  key   | data     | key      |
 *	+--------+----------+----------+
 *
 * calculating the offsets for free area:  if the number
 * of entries (ino[0]) is zero, the offset to the END of
 * the free area is the block size. Otherwise, it is the
 * nth (ino[ino[0]]) entry's offset.
 */

static int
fitpair(char *pag, int need)
{
	register int n;
	register int off;
	register int free;
	register short *ino = (short *) pag;

	off = ((n = GET_SHORT(ino,0)) > 0) ? GET_SHORT(ino,n) : PBLKSIZ;
	free = off - (n + 1) * (int)sizeof(short);
	need += 2 * (int)sizeof(short);

	debug(("free %d need %d\n", free, need));

	return need <= free;
}

static void
putpair(char *pag, datum key, datum val)
{
	register int n;
	register int off;
	register short *ino = (short *) pag;

	off = ((n = GET_SHORT(ino,0)) > 0) ? GET_SHORT(ino,n) : PBLKSIZ;
/*
 * enter the key first
 */
	off -= key.dsize;
	if (key.dsize)
		(void) memcpy(pag + off, key.dptr, key.dsize);
	PUT_SHORT(ino,n + 1,off);
/*
 * now the data
 */
	off -= val.dsize;
	if (val.dsize)
		(void) memcpy(pag + off, val.dptr, val.dsize);
	PUT_SHORT(ino,n + 2,off);
/*
 * adjust item count
 */
	PUT_SHORT(ino,0,GET_SHORT(ino,0) + 2);
}

static datum
getpair(char *pag, datum key)
{
	register int i;
	register int n;
	datum val;
	register short *ino = (short *) pag;

	if ((n = GET_SHORT(ino,0)) == 0)
		return nullitem;

	if ((i = seepair(pag, n, key.dptr, key.dsize)) == 0)
		return nullitem;

	val.dptr = pag + GET_SHORT(ino,i + 1);
	val.dsize = GET_SHORT(ino,i) - GET_SHORT(ino,i + 1);
	return val;
}

#if SEEDUPS
static int
duppair(char *pag, datum key)
{
	register short *ino = (short *) pag;
	return GET_SHORT(ino,0) > 0 &&
		   seepair(pag, GET_SHORT(ino,0), key.dptr, key.dsize) > 0;
}
#endif

static datum
getnkey(char *pag, int num)
{
	datum key;
	register int off;
	register short *ino = (short *) pag;

	num = num * 2 - 1;
	if (GET_SHORT(ino,0) == 0 || num > GET_SHORT(ino,0))
		return nullitem;

	off = (num > 1) ? GET_SHORT(ino,num - 1) : PBLKSIZ;

	key.dptr = pag + GET_SHORT(ino,num);
	key.dsize = off - GET_SHORT(ino,num);

	return key;
}

static int
delpair(char *pag, datum key)
{
	register int n;
	register int i;
	register short *ino = (short *) pag;

	if ((n = GET_SHORT(ino,0)) == 0)
		return 0;

	if ((i = seepair(pag, n, key.dptr, key.dsize)) == 0)
		return 0;
/*
 * found the key. if it is the last entry
 * [i.e. i == n - 1] we just adjust the entry count.
 * hard case: move all data down onto the deleted pair,
 * shift offsets onto deleted offsets, and adjust them.
 * [note: 0 < i < n]
 */
	if (i < n - 1) {
		register int m;
		register char *dst = pag + (i == 1 ? PBLKSIZ : GET_SHORT(ino,i - 1));
		register char *src = pag + GET_SHORT(ino,i + 1);
		register ptrdiff_t   zoo = dst - src;

		debug(("free-up %"PRIdPTRDIFF" ", zoo));
/*
 * shift data/keys down
 */
		m = GET_SHORT(ino,i + 1) - GET_SHORT(ino,n);
#ifdef DUFF
#define MOVB 	*--dst = *--src

		if (m > 0) {
			register int loop = (m + 8 - 1) >> 3;

			switch (m & (8 - 1)) {
			case 0:	do {
				MOVB;	case 7:	MOVB;
			case 6:	MOVB;	case 5:	MOVB;
			case 4:	MOVB;	case 3:	MOVB;
			case 2:	MOVB;	case 1:	MOVB;
				} while (--loop);
			}
		}
#else
#ifdef MEMMOVE
		memmove(dst, src, m);
#else
		while (m--)
			*--dst = *--src;
#endif
#endif
/*
 * adjust offset index up
 */
		while (i < n - 1) {
			PUT_SHORT(ino,i, GET_SHORT(ino,i + 2) + zoo);
			i++;
		}
	}
	PUT_SHORT(ino, 0, GET_SHORT(ino, 0) - 2);
	return 1;
}

/*
 * search for the key in the page.
 * return offset index in the range 0 < i < n.
 * return 0 if not found.
 */
static int
seepair(char *pag, register int n, register char *key, register int siz)
{
	register int i;
	register int off = PBLKSIZ;
	register short *ino = (short *) pag;

	for (i = 1; i < n; i += 2) {
		if (siz == off - GET_SHORT(ino,i) &&
		    memcmp(key, pag + GET_SHORT(ino,i), siz) == 0)
			return i;
		off = GET_SHORT(ino,i + 1);
	}
	return 0;
}

static void
splpage(char *pag, char *new, long int sbit)
{
	datum key;
	datum val;

	register int n;
	register int off = PBLKSIZ;
	char cur[PBLKSIZ];
	register short *ino = (short *) cur;

	(void) memcpy(cur, pag, PBLKSIZ);
	(void) memset(pag, 0, PBLKSIZ);
	(void) memset(new, 0, PBLKSIZ);

	n = GET_SHORT(ino,0);
	for (ino++; n > 0; ino += 2) {
		key.dptr = cur + GET_SHORT(ino,0);
		key.dsize = off - GET_SHORT(ino,0);
		val.dptr = cur + GET_SHORT(ino,1);
		val.dsize = GET_SHORT(ino,0) - GET_SHORT(ino,1);
/*
 * select the page pointer (by looking at sbit) and insert
 */
		(void) putpair((exhash(key) & sbit) ? new : pag, key, val);

		off = GET_SHORT(ino,1);
		n -= 2;
	}

	debug(("%d split %d/%d\n", ((short *) cur)[0] / 2,
	       ((short *) new)[0] / 2,
	       ((short *) pag)[0] / 2));
}

/*
 * check page sanity:
 * number of entries should be something
 * reasonable, and all offsets in the index should be in order.
 * this could be made more rigorous.
 */
static int
chkpage(char *pag)
{
	register int n;
	register int off;
	register short *ino = (short *) pag;

	if ((n = GET_SHORT(ino,0)) < 0 || n > PBLKSIZ / (int)sizeof(short))
		return 0;

	if (n > 0) {
		off = PBLKSIZ;
		for (ino++; n > 0; ino += 2) {
			if (GET_SHORT(ino,0) > off || GET_SHORT(ino,1) > off ||
			    GET_SHORT(ino,1) > GET_SHORT(ino,0))
				return 0;
			off = GET_SHORT(ino,1);
			n -= 2;
		}
	}
	return 1;
}

/* hash.c */
/*
 * sdbm - ndbm work-alike hashed database library
 * based on Per-Aake Larson's Dynamic Hashing algorithms. BIT 18 (1978).
 * author: oz@nexus.yorku.ca
 * status: public domain. keep it that way.
 *
 * hashing routine
 */

/*
 * polynomial conversion ignoring overflows
 * [this seems to work remarkably well, in fact better
 * then the ndbm hash function. Replace at your own risk]
 * use: 65599	nice.
 *      65587   even better.
 */
long
sdbm_hash(register char *str, register int len)
{
	register unsigned long n = 0;

#ifdef DUFF

#define HASHC	n = *str++ + 65599 * n

	if (len > 0) {
		register int loop = (len + 8 - 1) >> 3;

		switch(len & (8 - 1)) {
		case 0:	do {
			HASHC;	case 7:	HASHC;
		case 6:	HASHC;	case 5:	HASHC;
		case 4:	HASHC;	case 3:	HASHC;
		case 2:	HASHC;	case 1:	HASHC;
			} while (--loop);
		}

	}
#else
	while (len--)
		n = ((*str++) & 255) + 65587L * n;
#endif
	return n;
}
