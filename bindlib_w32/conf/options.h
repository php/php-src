/* options.h - specify the conditionally-compiled features
 * vix 28mar92 [moved out of the Makefile because they were getting too big]
 *
 * $Id$
 */

/*
 * ++Copyright++
 * -
 * Copyright (c) 
 *    The Regents of the University of California.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 * 	This product includes software developed by the University of
 * 	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * -
 * --Copyright--
 */

/* Key:
 *	ucb = U C Berkeley 4.8.3 release
 *	vix = Paul Vixie of Digital
 *	del = Don Lewis of Harris
 *	mcsun = Piet Beertema of EUNet
 *	asp = Andrew Partan of UUNet
 *	pma = Paul Albitz of Hewlett Packard
 *	bb = Bryan Beecher of UMich
 *	mpa = Mark Andrews of CSIRO - DMS
 *	rossc = Ross Cartlidge of The Univeritsy of Sydney
 *	mtr = Marshall Rose of TPC.INT
 *      bg = Benoit Grange of INRIA
 *      ckd = Christopher Davis of Kapor Enterprises
 *      gns = Greg Shapiro of WPI
 */

#define DEBUG		/* enable -d flag and SIGUSR[12] support (ucb) */
/*#define ALLOW_T_UNSPEC /* enable the "unspec" RR type for old athena (ucb) */
/*#define INVQ		/* enable inverse queries (nslookup) (ucb/vix) */
/*#define DSTORAGE	/* debug malloc overruns using storage.o (ucb/vix) */
/*#define DMALLOC	/* trace malloc orphans using dmalloc.o (vix) */
#define XFRNETS  	/* enable "xfrnets" command in named.boot (vix) */
#define PID_FIX   	/* be careful about overwriting named.pid file (del) */
#define FWD_LOOP	/* try to break out of forwarding loops (del) */
#define NO_GLUE  	/* don't accept or send out-of-zone glue (del) */
#define	BOGUSNS  	/* detect bogus nameservers (mcsun) */
#define QRYLOG  	/* enable SIGWINCH for query logging (bb) */
/*#define YPKLUDGE  	/* deal effectively with broken "ypserv -i" (mcsun) */
#define TRACEROOT  	/* trace bogus root servers and ignore them (pma,bb) */
/*#define LOCALDOM	/* permit "domain" directive in named.boot (ucb) */
#define FORCED_RELOAD	/* refresh secondary zones on SIGHUP (pma) */
#define SLAVE_FORWARD	/* use sensible timeouts on slave forwarders (pma) */
#define WANT_PIDFILE	/* if you want the named.pid file (ucb/arc) */
#define DOTTED_SERIAL	/* if you want to be able to specify dotted serial#s */
#define SENSIBLE_DOTS	/* if you want dotted serial#s to make numeric sense */
#define NCACHE		/* negative caching (anant@isi.edu) */
/*#define VALIDATE	/* validation procedure (anant@isi.edu) (BUGGY!) */
/*#define SHORT_FNAMES	/* file names used in named-xfer need to be short */
#define RESOLVSORT	/* allow sorting of addresses in gethostbyname (mpa) */
#define STUBS		/* allow transfers of NS only for a zone (mpa) */
#ifndef LOGFAC
#define	LOGFAC LOG_DAEMON /* what syslog facility should named use? */
#endif
#define SECURE_ZONES	/* if you want to inhibit world access to zones (gns)*/
#define ROUND_ROBIN	/* rotate databuf list after each access (mtr) */
#define ADDAUTH 	/* return NS and glue w/ authorative answers (mpa) */
#define RFC1535		/* use RFC 1535 default for "search" list (vix) */
#define GEN_AXFR	/* distinct zones within each class */
#define LAME_DELEGATION	/* lame delegations (original-del,reworked-bb&del)*/
#define LAME_LOGGING LOG_DEBUG /* log lame delegations, set log level */
#define GETSER_LOGGING LOG_INFO /* log errors/timeouts getting serial number */
#define RETURNSOA	/* good code that the world might be ready for now */
#define CLEANCACHE	/* useful and necessary in the face of NCACHE */
#define PURGE_ZONE	/* remove all traces of a zone when reloading (mpa) */
#define STATS		/* keep nameserver statistics; uses more memory */
#ifndef WINNT
#define RENICE  	/* named-xfer should run at normal priority */
#endif
#define XSTATS	/* extended statistics, syslogged periodically (bg) */
/*#define BIND_NOTIFY	/* experimental - do not enable in customer products */
#define LOC_RR		/* support for LOC record parsing (ckd/vix) */
#define SORT_RESPONSE	/* should we try to sort responses optimally? (vix) */

/*--------------------------------------------*
 * no user-servicable parts beyond this point *
 *--------------------------------------------*/

/* if DSTORAGE is defined, we need to disable DMALLOC and remap
 * malloc and free to storage.o's exported names.  storage.o also
 * includes a calloc and a realloc, but once we drag in its malloc
 * and free we'll get the others automatically and so will never
 * pull in those routines from libc.a.
 */
#ifdef DSTORAGE
# ifdef DMALLOC
#  undef DMALLOC
# endif /*DMALLOC*/
# define malloc	rt_malloc
# define free rt_free
#endif /*DSTORAGE*/

/* if DMALLOC is defined, grab the header file which will remap
 * all the malloc-style names to those exported by dmalloc.o.  note
 * that DMALLOC also changes the function signatures of several
 * functions in private named source modules, and that this file
 * (options.h) must be included before any other private *.h files
 * since those *.h files have some conditional remapping to do.
 */
#ifdef DMALLOC
# include "dmalloc.h"
#endif

#ifdef LAME_LOGGING
# define LAME_DELEGATION
#endif

#if defined(XSTATS) && !defined(STATS)
# define STATS
#endif
