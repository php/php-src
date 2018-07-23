/*
 * This file modified from sources for imap4 for use
 * in PHP 3
 */
/*
 * Program:   Unix compatibility routines
 *
 * Author:  Mark Crispin
 *      Networks and Distributed Computing
 *      Computing & Communications
 *      University of Washington
 *      Administration Building, AG-44
 *      Seattle, WA  98195
 *      Internet: MRC@CAC.Washington.EDU
 *
 * Date:    14 September 1996
 * Last Edited: 22 October 1996
 *
 * Copyright 1996 by the University of Washington
 *
 *  Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appears in all copies and that both the
 * above copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Washington not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  This software is made available
 * "as is", and
 * THE UNIVERSITY OF WASHINGTON DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND IN
 * NO EVENT SHALL THE UNIVERSITY OF WASHINGTON BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE) OR STRICT LIABILITY, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */


/*              DEDICATION

 *  This file is dedicated to my dog, Unix, also known as Yun-chan and
 * Unix J. Terwilliker Jehosophat Aloysius Monstrosity Animal Beast.  Unix
 * passed away at the age of 11 1/2 on September 14, 1996, 12:18 PM PDT, after
 * a two-month bout with cirrhosis of the liver.
 *
 *  He was a dear friend, and I miss him terribly.
 *
 *  Lift a leg, Yunie.  Luv ya forever!!!!
 */

#include "php.h"				/*php specific */
#include "syslog.h"
#include <stdio.h>
#include <fcntl.h>
#include <process.h>

#include "php_win32_globals.h"
#include "wsyslog.h"
#include "codepage.h"

void closelog(void)
{
	if (INVALID_HANDLE_VALUE != PW32G(log_source)) {
		DeregisterEventSource(PW32G(log_source));
		PW32G(log_source) = INVALID_HANDLE_VALUE;
	}
	if (PW32G(log_header)) {
		free(PW32G(log_header));
		PW32G(log_header) = NULL;
	}
}

/* Emulator for BSD syslog() routine
 * Accepts: priority
 *      message
 *      parameters
 */

void syslog(int priority, const char *message, ...)
{
	va_list args;

	va_start(args, message);	/* initialize vararg mechanism */
	vsyslog(priority, message, args);
	va_end(args);
}

void vsyslog(int priority, const char *message, va_list args)
{
	LPTSTR strs[2];
	unsigned short etype;
	char *tmp = NULL;
	DWORD evid;
	wchar_t *strsw[2];

	switch (priority) {			/* translate UNIX type into NT type */
		case LOG_ALERT:
			etype = EVENTLOG_ERROR_TYPE;
			evid = PHP_SYSLOG_ERROR_TYPE;
			break;
		case LOG_INFO:
			etype = EVENTLOG_INFORMATION_TYPE;
			evid = PHP_SYSLOG_INFO_TYPE;
			break;
		default:
			etype = EVENTLOG_WARNING_TYPE;
			evid = PHP_SYSLOG_WARNING_TYPE;
	}

	vspprintf(&tmp, 0, message, args);	/* build message */

	strsw[0] = php_win32_cp_any_to_w(PW32G(log_header));
	strsw[1] = php_win32_cp_any_to_w(tmp);

	/* report the event */
	if (strsw[0] && strsw[1]) {
		ReportEventW(PW32G(log_source), etype, (unsigned short) priority, evid, NULL, 2, 0, strsw, NULL);
		free(strsw[0]);
		free(strsw[1]);
		efree(tmp);
		return;
	}

	free(strsw[0]);
	free(strsw[1]);

	strs[0] = PW32G(log_header);	/* write header */
	strs[1] = tmp;				/* then the message */

	ReportEventA(PW32G(log_source), etype, (unsigned short) priority, evid, NULL, 2, 0, strs, NULL);
	efree(tmp);
}

/* Emulator for BSD openlog() routine
 * Accepts: identity
 *      options
 *      facility
 */

void openlog(const char *ident, int logopt, int facility)
{
	size_t header_len;

	closelog();

	PW32G(log_source) = RegisterEventSource(NULL, "PHP-" PHP_VERSION);
	header_len = strlen(ident) + 2 + 11;
	PW32G(log_header) = malloc(header_len*sizeof(char));
	sprintf_s(PW32G(log_header), header_len, (logopt & LOG_PID) ? "%s[%d]" : "%s", ident, getpid());
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
