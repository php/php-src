/*
 *	@(#)pathnames.h	5.4 (Berkeley) 6/1/90
 *	$Id$
 */

/*
 * ++Copyright++ 1989
 * -
 * Copyright (c) 1989
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


#ifdef _PATH_XFER
# define _PATH_XFER_PREDEFINED	/* probably from Makefile */
#endif

#ifdef WINNT
#define	_PATH_BOOT	"%windir%\\NAMED.BOOT"
#define	_ALT_PATH_BOOT	"%windir%\\NAMED.INI"
#ifndef _PATH_XFER
# define _PATH_XFER	"%windir%\\SYSTEM32\\XFER.EXE"
#endif
#ifndef _PATH_PIDFILE
# define _PATH_PIDFILE	"%windir%\\NAMED.PID"
#endif
/* following paths prepended with GetTempPath() */
#define	_PATH_DEBUG	"NAMED.RUN" 
#define	_PATH_DUMPFILE	"NAMDUMP.DB"
#define	_PATH_STATS	"NAMED.STS"
#define	_PATH_XFERTRACE	"XFER.TRC"
#define _PATH_XFERDDT	"XFER.DDT"
#define	_PATH_TMPXFER	"XFER.DDT.XXXXXX"
#else

#if defined (__sgi) && !defined(_SYSTYPE_SVR4) && !defined(__SYSTYPE_SVR4)
#define	_PATH_BOOT	"/usr/etc/named.d/named.boot"
#else
#define	_PATH_BOOT	"/etc/named.boot"
#endif

#if defined(BSD) && BSD >= 198810

#include <paths.h>
#ifndef _PATH_XFER
# define _PATH_XFER	"/usr/libexec/named-xfer"
#endif
#define	_PATH_DEBUG	"named.run"
#define	_PATH_DUMPFILE	"named_dump.db"
#ifndef _PATH_PIDFILE
# define _PATH_PIDFILE	"/var/run/named.pid"
#endif
#define	_PATH_STATS	"named.stats"
#define	_PATH_XFERTRACE	"xfer.trace"
#define _PATH_XFERDDT	"xfer.ddt"
#define	_PATH_TMPXFER	"xfer.ddt.XXXXXX"
#define	_PATH_TMPDIR	"/var/tmp"

#else /* BSD */

#define	_PATH_DEVNULL	"/dev/null"
#define	_PATH_TTY	"/dev/tty"
#ifndef _PATH_XFER
# define _PATH_XFER	"/etc/named-xfer"
#endif
#define	_PATH_DEBUG	"named.run"
#define	_PATH_DUMPFILE	"named_dump.db"
#ifndef _PATH_PIDFILE
# define _PATH_PIDFILE	"/etc/named.pid"
#endif
#define	_PATH_STATS	"named.stats"
#define	_PATH_XFERTRACE	"xfer.trace"
#define _PATH_XFERDDT	"xfer.ddt"
#define	_PATH_TMPXFER	"xfer.ddt.XXXXXX"
#define	_PATH_TMPDIR	"/usr/tmp"
#endif /* BSD */

#ifndef _PATH_XFER_PREDEFINED
# if defined(__sgi) || defined(NeXT) || defined(__ultrix)
#  undef _PATH_XFER
#  define _PATH_XFER	"/usr/etc/named-xfer"
# endif
# if defined(__osf__)
#  undef _PATH_XFER
#  define _PATH_XFER	"/usr/sbin/named-xfer"
# endif
# ifdef sun
#  undef _PATH_XFER
#  define _PATH_XFER	"/usr/etc/in.named-xfer"
# endif
#else
# undef _PATH_XFER_PREDEFINED
#endif /*_PATH_XFER_PREDEFINED*/

#endif /* WINNT */
