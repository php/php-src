/*
 * PostgreSQL is released under the PostgreSQL License, a liberal Open Source
 * license, similar to the BSD or MIT licenses.
 * PostgreSQL Database Management System (formerly known as Postgres, then as
 * Postgres95)
 *
 * Portions Copyright (c) 1996-2015, The PostgreSQL Global Development Group
 *
 * Portions Copyright (c) 1994, The Regents of the University of California
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without a written
 * agreement is hereby granted, provided that the above copyright notice
 * and this paragraph and the following two paragraphs appear in all copies.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
 * EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN
 * "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * The following code is adopted from the PostgreSQL's ps_status(.h/.c).
 */

#include "ps_title.h"
#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <stdlib.h>

#ifdef PHP_WIN32
#include "config.w32.h"
#include <windows.h>
#include <process.h>
#else
#include "php_config.h"
extern char** environ;
#endif

#ifdef HAVE_SYS_PSTAT_H
#include <sys/pstat.h> /* for HP-UX */
#endif
#ifdef HAVE_PS_STRINGS
#include <machine/vmparam.h> /* for old BSD */
#include <sys/exec.h>
#endif
#if defined(DARWIN)
#include <crt_externs.h>
#endif

/*
 * Ways of updating ps display:
 *
 * PS_USE_SETPROCTITLE
 *         use the function setproctitle(const char *, ...)
 *         (newer BSD systems)
 * PS_USE_PSTAT
 *         use the pstat(PSTAT_SETCMD, )
 *         (HPUX)
 * PS_USE_PS_STRINGS
 *         assign PS_STRINGS->ps_argvstr = "string"
 *         (some BSD systems)
 * PS_USE_CHANGE_ARGV
 *         assign argv[0] = "string"
 *         (some other BSD systems)
 * PS_USE_CLOBBER_ARGV
 *         write over the argv and environment area
 *         (Linux and most SysV-like systems)
 * PS_USE_WIN32
 *         push the string out as the name of a Windows event
 * PS_USE_NONE
 *         don't update ps display
 *         (This is the default, as it is safest.)
 */
#if defined(HAVE_SETPROCTITLE)
#define PS_USE_SETPROCTITLE
#elif defined(HAVE_SYS_PSTAT_H) && defined(PSTAT_SETCMD)
#define PS_USE_PSTAT
#elif defined(HAVE_PS_STRINGS)
#define PS_USE_PS_STRINGS
#elif defined(BSD) && !defined(DARWIN)
#define PS_USE_CHANGE_ARGV
#elif defined(__linux__) || defined(_AIX) || defined(__sgi) || (defined(sun) && !defined(BSD)) || defined(ultrix) || defined(__osf__) || defined(DARWIN)
#define PS_USE_CLOBBER_ARGV
#elif defined(PHP_WIN32)
#define PS_USE_WIN32
#else
#define PS_USE_NONE
#endif

/* Different systems want the buffer padded differently */
#if defined(_AIX) || defined(__linux__) || defined(DARWIN)
#define PS_PADDING '\0'
#else
#define PS_PADDING ' '
#endif

#ifdef PS_USE_WIN32
static char windows_error_details[64];
static char ps_buffer[MAX_PATH];
static const size_t ps_buffer_size = MAX_PATH;
typedef BOOL (WINAPI *MySetConsoleTitle)(LPCTSTR);
typedef DWORD (WINAPI *MyGetConsoleTitle)(LPTSTR, DWORD);
#elif defined(PS_USE_CLOBBER_ARGV)
static char *ps_buffer;         /* will point to argv area */
static size_t ps_buffer_size;   /* space determined at run time */
static char *empty_environ[] = {0}; /* empty environment */
#else
#define PS_BUFFER_SIZE 256
static char ps_buffer[PS_BUFFER_SIZE];
static const size_t ps_buffer_size = PS_BUFFER_SIZE;
#endif

static size_t ps_buffer_cur_len; /* actual string length in ps_buffer */

/* save the original argv[] location here */
static int save_argc;
static char** save_argv;

/*
 * This holds the 'locally' allocated environ from the save_ps_args method.
 * This is subsequently free'd at exit.
 */
#if defined(PS_USE_CLOBBER_ARGV)
static char** frozen_environ, **new_environ;
#endif

/*
 * Call this method early, before any code has used the original argv passed in
 * from main().
 * If needed, this code will make deep copies of argv and environ and return
 * these to the caller for further use. The original argv is then 'clobbered'
 * to store the process title.
 */
char** save_ps_args(int argc, char** argv)
{
    save_argc = argc;
    save_argv = argv;

#if defined(PS_USE_CLOBBER_ARGV)
    /*
     * If we're going to overwrite the argv area, count the available space.
     * Also move the environment to make additional room.
     */
    {
        char* end_of_area = NULL;
        int non_contiguous_area = 0;
        int i;

        /*
         * check for contiguous argv strings
         */
        for (i = 0; (non_contiguous_area == 0) && (i < argc); i++)
        {
            if (i != 0 && end_of_area + 1 != argv[i])
                non_contiguous_area = 1;
            end_of_area = argv[i] + strlen(argv[i]);
        }

        /*
         * check for contiguous environ strings following argv
         */
        for (i = 0; (non_contiguous_area == 0) && (environ[i] != NULL); i++)
        {
            if (end_of_area + 1 != environ[i])
                non_contiguous_area = 1;
            end_of_area = environ[i] + strlen(environ[i]);
        }

        if (non_contiguous_area != 0)
            goto clobber_error;

        ps_buffer = argv[0];
        ps_buffer_size = end_of_area - argv[0];

        /*
         * move the environment out of the way
         */
        new_environ = (char **) malloc((i + 1) * sizeof(char *));
        frozen_environ = (char **) malloc((i + 1) * sizeof(char *));
        if (!new_environ || !frozen_environ)
            goto clobber_error;
        for (i = 0; environ[i] != NULL; i++)
        {
            new_environ[i] = strdup(environ[i]);
            if (!new_environ[i])
                goto clobber_error;
        }
        new_environ[i] = NULL;
        environ = new_environ;
        memcpy((char *)frozen_environ, (char *)new_environ, sizeof(char *) * (i + 1));

    }
#endif /* PS_USE_CLOBBER_ARGV */

#if defined(PS_USE_CHANGE_ARGV) || defined(PS_USE_CLOBBER_ARGV)
    /*
     * If we're going to change the original argv[] then make a copy for
     * argument parsing purposes.
     *
     * (NB: do NOT think to remove the copying of argv[]!
     * On some platforms, getopt() keeps pointers into the argv array, and
     * will get horribly confused when it is re-called to analyze a subprocess'
     * argument string if the argv storage has been clobbered meanwhile.
     * Other platforms have other dependencies on argv[].)
     */
    {
        char** new_argv;
        int i;

        new_argv = (char **) malloc((argc + 1) * sizeof(char *));
        if (!new_argv)
            goto clobber_error;
        for (i = 0; i < argc; i++)
        {
            new_argv[i] = strdup(argv[i]);
            if (!new_argv[i]) {
				free(new_argv);
                goto clobber_error;
			}
        }
        new_argv[argc] = NULL;

#if defined(DARWIN)
        /*
         * Darwin (and perhaps other NeXT-derived platforms?) has a static
         * copy of the argv pointer, which we may fix like so:
         */
        *_NSGetArgv() = new_argv;
#endif

        argv = new_argv;

    }
#endif /* PS_USE_CHANGE_ARGV or PS_USE_CLOBBER_ARGV */

#if defined(PS_USE_CLOBBER_ARGV)
    {
        /* make extra argv slots point at end_of_area (a NUL) */
        int i;
        for (i = 1; i < save_argc; i++)
            save_argv[i] = ps_buffer + ps_buffer_size;
    }
#endif /* PS_USE_CLOBBER_ARGV */

#ifdef PS_USE_CHANGE_ARGV
    save_argv[0] = ps_buffer; /* ps_buffer here is a static const array of size PS_BUFFER_SIZE */
    save_argv[1] = NULL;
#endif /* PS_USE_CHANGE_ARGV */

    return argv;

#if defined(PS_USE_CHANGE_ARGV) || defined(PS_USE_CLOBBER_ARGV)
clobber_error:
    /* probably can't happen?!
     * if we ever get here, argv still points to originally passed
     * in argument
     */
    save_argv = NULL;
    save_argc = 0;
    ps_buffer = NULL;
    ps_buffer_size = 0;
    return argv;
#endif /* PS_USE_CHANGE_ARGV or PS_USE_CLOBBER_ARGV */
}

/*
 * Returns PS_TITLE_SUCCESS if the OS supports this functionality
 * and the init function was called.
 * Otherwise returns NOT_AVAILABLE or NOT_INITIALIZED
 */
int is_ps_title_available()
{
#ifdef PS_USE_NONE
    return PS_TITLE_NOT_AVAILABLE; /* disabled functionality */
#endif

    if (!save_argv)
        return PS_TITLE_NOT_INITIALIZED;

#ifdef PS_USE_CLOBBER_ARGV
    if (!ps_buffer)
        return PS_TITLE_BUFFER_NOT_AVAILABLE;
#endif /* PS_USE_CLOBBER_ARGV */

    return PS_TITLE_SUCCESS;
}

/*
 * Convert error codes into error strings
 */
const char* ps_title_errno(int rc)
{
    switch(rc)
    {
    case PS_TITLE_SUCCESS:
        return "Success";

    case PS_TITLE_NOT_AVAILABLE:
        return "Not available on this OS";

    case PS_TITLE_NOT_INITIALIZED:
        return "Not initialized correctly";

    case PS_TITLE_BUFFER_NOT_AVAILABLE:
        return "Buffer not contiguous";

#ifdef PS_USE_WIN32
    case PS_TITLE_WINDOWS_ERROR:
        sprintf(windows_error_details, "Windows error code: %lu", GetLastError());
        return windows_error_details;
#endif
    }

    return "Unknown error code";
}

/*
 * Set a new process title.
 * Returns the appropriate error code if if there's an error
 * (like the functionality is compile time disabled, or the
 * save_ps_args() was not called.
 * Else returns 0 on success.
 */
int set_ps_title(const char* title)
{
    int rc = is_ps_title_available();
    if (rc != PS_TITLE_SUCCESS)
        return rc;

    strncpy(ps_buffer, title, ps_buffer_size);
    ps_buffer[ps_buffer_size - 1] = '\0';
    ps_buffer_cur_len = strlen(ps_buffer);

#ifdef PS_USE_SETPROCTITLE
    setproctitle("%s", ps_buffer);
#endif

#ifdef PS_USE_PSTAT
    {
        union pstun pst;

        pst.pst_command = ps_buffer;
        pstat(PSTAT_SETCMD, pst, ps_buffer_cur_len, 0, 0);
    }
#endif /* PS_USE_PSTAT */

#ifdef PS_USE_PS_STRINGS
    PS_STRINGS->ps_nargvstr = 1;
    PS_STRINGS->ps_argvstr = ps_buffer;
#endif /* PS_USE_PS_STRINGS */

#ifdef PS_USE_CLOBBER_ARGV
    /* pad unused memory */
    if (ps_buffer_cur_len < ps_buffer_size)
    {
        memset(ps_buffer + ps_buffer_cur_len, PS_PADDING,
               ps_buffer_size - ps_buffer_cur_len);
    }
#endif /* PS_USE_CLOBBER_ARGV */

#ifdef PS_USE_WIN32
    {
    	MySetConsoleTitle set_title = NULL;
	HMODULE hMod = LoadLibrary("kernel32.dll");

	if (!hMod) {
            return PS_TITLE_WINDOWS_ERROR;
	}

	/* NOTE we don't use _UNICODE*/
	set_title = (MySetConsoleTitle)GetProcAddress(hMod, "SetConsoleTitleA");
	if (!set_title) {
            return PS_TITLE_WINDOWS_ERROR;
	}

        if (!set_title(ps_buffer)) {
            return PS_TITLE_WINDOWS_ERROR;
	}
    }
#endif /* PS_USE_WIN32 */

    return PS_TITLE_SUCCESS;
}

/*
 * Returns the current ps_buffer value into string.  On some platforms
 * the string will not be null-terminated, so return the effective
 * length into *displen.
 * The return code indicates the error.
 */
int get_ps_title(int *displen, const char** string)
{
    int rc = is_ps_title_available();
    if (rc != PS_TITLE_SUCCESS)
        return rc;

#ifdef PS_USE_WIN32
    {
    	MyGetConsoleTitle get_title = NULL;
	HMODULE hMod = LoadLibrary("kernel32.dll");

	if (!hMod) {
            return PS_TITLE_WINDOWS_ERROR;
	}

	/* NOTE we don't use _UNICODE*/
	get_title = (MyGetConsoleTitle)GetProcAddress(hMod, "GetConsoleTitleA");
	if (!get_title) {
            return PS_TITLE_WINDOWS_ERROR;
	}

        if (!(ps_buffer_cur_len = get_title(ps_buffer, (DWORD)ps_buffer_size))) {
            return PS_TITLE_WINDOWS_ERROR;
	}
    }
#endif
    *displen = (int)ps_buffer_cur_len;
    *string = ps_buffer;
    return PS_TITLE_SUCCESS;
}

/*
 * Clean up the allocated argv and environ if applicable. Only call
 * this right before exiting.
 * This isn't needed per-se because the OS will clean-up anyway, but
 * having and calling this will ensure Valgrind doesn't output 'false
 * positives'.
 */
void cleanup_ps_args(char **argv)
{
#ifndef PS_USE_NONE
    if (save_argv)
    {
        save_argv = NULL;
        save_argc = 0;

#ifdef PS_USE_CLOBBER_ARGV
        {
            int i;
            for (i = 0; frozen_environ[i] != NULL; i++)
                free(frozen_environ[i]);
            free(frozen_environ);
            free(new_environ);
            /* leave a sane environment behind since some atexit() handlers
                call getenv(). */
            environ = empty_environ;
        }
#endif /* PS_USE_CLOBBER_ARGV */

#if defined(PS_USE_CHANGE_ARGV) || defined(PS_USE_CLOBBER_ARGV)
        {
            int i;
            for (i=0; argv[i] != NULL; i++)
                free(argv[i]);
            free(argv);
        }
#endif /* PS_USE_CHANGE_ARGV or PS_USE_CLOBBER_ARGV */
    }
#endif /* PS_USE_NONE */

    return;
}
