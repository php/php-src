
	/* $Id: zlog.h,v 1.7 2008/05/22 21:08:32 anight Exp $ */
	/* (c) 2004-2007 Andrei Nigmatulin */

#ifndef ZLOG_H
#define ZLOG_H 1

#define ZLOG_STUFF		__func__, __LINE__

struct timeval;

int zlog_set_fd(int new_fd);
int zlog_set_level(int new_value);

size_t zlog_print_time(struct timeval *tv, char *timebuf, size_t timebuf_len);

void zlog(const char *function, int line, int flags, const char *fmt, ...)
		__attribute__ ((format(printf,4,5)));

enum {
	ZLOG_DEBUG			= 1,
	ZLOG_NOTICE			= 2,
	ZLOG_WARNING		= 3,
	ZLOG_ERROR			= 4,
	ZLOG_ALERT			= 5,
};

#define ZLOG_LEVEL_MASK 7

#define ZLOG_HAVE_ERRNO 0x100

#define ZLOG_SYSERROR (ZLOG_ERROR | ZLOG_HAVE_ERRNO)

#endif
