/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Alex Belits <abelits@phobos.illtel.denver.co.us>             |
   +----------------------------------------------------------------------+
 */

/* $Id$ */
#include "php.h"

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <errno.h>
#include <ctype.h>

#if FHTTPD
#include <servproc.h>
#include <signal.h>

struct http_server *server = NULL;
struct request *req = NULL;
struct httpresponse *response = NULL;
int headermade = 0;
int global_alarmflag = 0;
int idle_timeout = IDLE_TIMEOUT;
int exit_status = 0;
char **currentheader = NULL;
char *headerfirstline = NULL;
int headerlines = 0;
static int headerlinesallocated = 0;

void alarmhandler(SIGACTARGS)
{
	global_alarmflag = 1;
}

void setalarm(int t)
{
	struct sigaction tmpsigaction;
	global_alarmflag = 0;
	if (t){
		bzero((char *) &tmpsigaction, sizeof(struct sigaction));
		tmpsigaction.sa_handler = alarmhandler;
		sigaddset(&tmpsigaction.sa_mask, SIGALRM);
		tmpsigaction.sa_flags = 0;
		sigaction(SIGALRM, &tmpsigaction, NULL);
		alarm(t);
	}
}

int checkinput(int h)
{
	fd_set readfd;
	FD_ZERO(&readfd);
	FD_SET(h, &readfd);
	return select(h + 1, &readfd, NULL, NULL, NULL) > 0;
}


PHPAPI void php_fhttpd_free_header(void)
{
	int i;

	if (headerfirstline) {
		free(headerfirstline);
		headerfirstline = NULL;
	}
	if (currentheader) {
		for (i = 0; i < headerlines; i++) {
			free(currentheader[i]);
		}
		free(currentheader);
		currentheader = NULL;
	}
	headerlines = 0;
	headerlinesallocated = 0;
	headermade = 0;
}


PHPAPI void php_fhttpd_puts_header(char *s)
{
	char *p0, *p1, *p2, *p3, **p;
	int l;

	if (!s || !*s || *s == '\r' || *s == '\n')
		return;
	l = strlen(s);
	p2 = strchr(s, '\r');
	p3 = strchr(s, '\n');
	p0 = strchr(s, ':');
	p1 = strchr(s, ' ');
	if (p0 && (!p1 || p1 > p0)) {
		if (!headerlinesallocated) {
			currentheader = (char **) malloc(10 * sizeof(char *));
			if (currentheader)
				headerlinesallocated = 10;
		} else {
			if (headerlinesallocated <= headerlines) {
				p = (char **) realloc(currentheader, (headerlinesallocated + 10) * sizeof(char *));
				if (p) {
					currentheader = p;
					headerlinesallocated += 10;
				}
			}
		}

		if (headerlinesallocated > headerlines) {
			currentheader[headerlines] = malloc(l + 3);
			if (currentheader[headerlines]) {
				strcpy(currentheader[headerlines], s);
				if (!p3) {
					if (p2) {
						(currentheader[headerlines] + (p2 - s))[1] = '\n';
						(currentheader[headerlines] + (p2 - s))[2] = 0;
					} else {
						currentheader[headerlines][l] = '\r';
						currentheader[headerlines][l + 1] = '\n';
						currentheader[headerlines][l + 2] = 0;
					}
				}
				headerlines++;
				headermade = 1;
			}
		}
	} else {
		if (headerfirstline)
			free(headerfirstline);
		headerfirstline = malloc(l + 3);
		if (headerfirstline) {
			strcpy(headerfirstline, s);
			if (!p3) {
				if (p2) {
					(headerfirstline + (p2 - s))[1] = '\n';
					(headerfirstline + (p2 - s))[2] = 0;
				} else {
					headerfirstline[l] = '\r';
					headerfirstline[l + 1] = '\n';
					headerfirstline[l + 2] = 0;
				}
			}
		}
		headermade = 1;
	}
}

void fhttpd_flush(void)
{
}

PHPAPI void php_fhttpd_puts(char *s)
{
	putlinetoresponse(response, s);
}

PHPAPI void php_fhttpd_putc(char c)
{
	writetoresponse(response, &c, 1);
}

PHPAPI int php_fhttpd_write(char *a, int n)
{
	return writetoresponse(response, a, n);
}
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
