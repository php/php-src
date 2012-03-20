/*
 *  Copyright (c) 1993, Intergraph Corporation
 *
 *  You may distribute under the terms of either the GNU General Public
 *  License or the Artistic License, as specified in the perl README file.
 *
 *  Various Unix compatibility functions and NT specific functions.
 *
 *  Some of this code was derived from the MSDOS port(s) and the OS/2 port.
 *
 */
/*
  The parts licensed under above copyright notice are marked as "Artistic or
  GPL".
  Another parts are licensed under Ruby's License.

  Copyright (C) 1993-2011 Yukihiro Matsumoto
  Copyright (C) 2000  Network Applied Communication Laboratory, Inc.
  Copyright (C) 2000  Information-technology Promotion Agency, Japan
 */

#include "ruby/ruby.h"
#include "ruby/encoding.h"
#include "dln.h"
#include <fcntl.h>
#include <process.h>
#include <sys/stat.h>
/* #include <sys/wait.h> */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>

#include <windows.h>
#include <winbase.h>
#include <wincon.h>
#include <share.h>
#include <shlobj.h>
#include <mbstring.h>
#if _MSC_VER >= 1400
#include <crtdbg.h>
#include <rtcapi.h>
#endif
#ifdef __MINGW32__
#include <mswsock.h>
#endif
#include "ruby/win32.h"
#include "win32/dir.h"
#define isdirsep(x) ((x) == '/' || (x) == '\\')

#undef stat
#undef fclose
#undef close
#undef setsockopt

#if defined __BORLANDC__
#  define _filbuf _fgetc
#  define _flsbuf _fputc
#  define enough_to_get(n) (--(n) >= 0)
#  define enough_to_put(n) (++(n) < 0)
#else
#  define enough_to_get(n) (--(n) >= 0)
#  define enough_to_put(n) (--(n) >= 0)
#endif

#ifdef WIN32_DEBUG
#define Debug(something) something
#else
#define Debug(something) /* nothing */
#endif

#define TO_SOCKET(x)	_get_osfhandle(x)

static struct ChildRecord *CreateChild(const WCHAR *, const WCHAR *, SECURITY_ATTRIBUTES *, HANDLE, HANDLE, HANDLE);
static int has_redirection(const char *);
int rb_w32_wait_events(HANDLE *events, int num, DWORD timeout);
static int rb_w32_open_osfhandle(intptr_t osfhandle, int flags);
static int wstati64(const WCHAR *path, struct stati64 *st);
VALUE rb_w32_conv_from_wchar(const WCHAR *wstr, rb_encoding *enc);

#define RUBY_CRITICAL(expr) do { expr; } while (0)

/* errno mapping */
static struct {
    DWORD winerr;
    int err;
} errmap[] = {
    {	ERROR_INVALID_FUNCTION,		EINVAL		},
    {	ERROR_FILE_NOT_FOUND,		ENOENT		},
    {	ERROR_PATH_NOT_FOUND,		ENOENT		},
    {	ERROR_TOO_MANY_OPEN_FILES,	EMFILE		},
    {	ERROR_ACCESS_DENIED,		EACCES		},
    {	ERROR_INVALID_HANDLE,		EBADF		},
    {	ERROR_ARENA_TRASHED,		ENOMEM		},
    {	ERROR_NOT_ENOUGH_MEMORY,	ENOMEM		},
    {	ERROR_INVALID_BLOCK,		ENOMEM		},
    {	ERROR_BAD_ENVIRONMENT,		E2BIG		},
    {	ERROR_BAD_FORMAT,		ENOEXEC		},
    {	ERROR_INVALID_ACCESS,		EINVAL		},
    {	ERROR_INVALID_DATA,		EINVAL		},
    {	ERROR_INVALID_DRIVE,		ENOENT		},
    {	ERROR_CURRENT_DIRECTORY,	EACCES		},
    {	ERROR_NOT_SAME_DEVICE,		EXDEV		},
    {	ERROR_NO_MORE_FILES,		ENOENT		},
    {	ERROR_WRITE_PROTECT,		EROFS		},
    {	ERROR_BAD_UNIT,			ENODEV		},
    {	ERROR_NOT_READY,		ENXIO		},
    {	ERROR_BAD_COMMAND,		EACCES		},
    {	ERROR_CRC,			EACCES		},
    {	ERROR_BAD_LENGTH,		EACCES		},
    {	ERROR_SEEK,			EIO		},
    {	ERROR_NOT_DOS_DISK,		EACCES		},
    {	ERROR_SECTOR_NOT_FOUND,		EACCES		},
    {	ERROR_OUT_OF_PAPER,		EACCES		},
    {	ERROR_WRITE_FAULT,		EIO		},
    {	ERROR_READ_FAULT,		EIO		},
    {	ERROR_GEN_FAILURE,		EACCES		},
    {	ERROR_LOCK_VIOLATION,		EACCES		},
    {	ERROR_SHARING_VIOLATION,	EACCES		},
    {	ERROR_WRONG_DISK,		EACCES		},
    {	ERROR_SHARING_BUFFER_EXCEEDED,	EACCES		},
    {	ERROR_BAD_NETPATH,		ENOENT		},
    {	ERROR_NETWORK_ACCESS_DENIED,	EACCES		},
    {	ERROR_BAD_NET_NAME,		ENOENT		},
    {	ERROR_FILE_EXISTS,		EEXIST		},
    {	ERROR_CANNOT_MAKE,		EACCES		},
    {	ERROR_FAIL_I24,			EACCES		},
    {	ERROR_INVALID_PARAMETER,	EINVAL		},
    {	ERROR_NO_PROC_SLOTS,		EAGAIN		},
    {	ERROR_DRIVE_LOCKED,		EACCES		},
    {	ERROR_BROKEN_PIPE,		EPIPE		},
    {	ERROR_DISK_FULL,		ENOSPC		},
    {	ERROR_INVALID_TARGET_HANDLE,	EBADF		},
    {	ERROR_INVALID_HANDLE,		EINVAL		},
    {	ERROR_WAIT_NO_CHILDREN,		ECHILD		},
    {	ERROR_CHILD_NOT_COMPLETE,	ECHILD		},
    {	ERROR_DIRECT_ACCESS_HANDLE,	EBADF		},
    {	ERROR_NEGATIVE_SEEK,		EINVAL		},
    {	ERROR_SEEK_ON_DEVICE,		EACCES		},
    {	ERROR_DIR_NOT_EMPTY,		ENOTEMPTY	},
    {	ERROR_DIRECTORY,		ENOTDIR		},
    {	ERROR_NOT_LOCKED,		EACCES		},
    {	ERROR_BAD_PATHNAME,		ENOENT		},
    {	ERROR_MAX_THRDS_REACHED,	EAGAIN		},
    {	ERROR_LOCK_FAILED,		EACCES		},
    {	ERROR_ALREADY_EXISTS,		EEXIST		},
    {	ERROR_INVALID_STARTING_CODESEG,	ENOEXEC		},
    {	ERROR_INVALID_STACKSEG,		ENOEXEC		},
    {	ERROR_INVALID_MODULETYPE,	ENOEXEC		},
    {	ERROR_INVALID_EXE_SIGNATURE,	ENOEXEC		},
    {	ERROR_EXE_MARKED_INVALID,	ENOEXEC		},
    {	ERROR_BAD_EXE_FORMAT,		ENOEXEC		},
    {	ERROR_ITERATED_DATA_EXCEEDS_64k,ENOEXEC		},
    {	ERROR_INVALID_MINALLOCSIZE,	ENOEXEC		},
    {	ERROR_DYNLINK_FROM_INVALID_RING,ENOEXEC		},
    {	ERROR_IOPL_NOT_ENABLED,		ENOEXEC		},
    {	ERROR_INVALID_SEGDPL,		ENOEXEC		},
    {	ERROR_AUTODATASEG_EXCEEDS_64k,	ENOEXEC		},
    {	ERROR_RING2SEG_MUST_BE_MOVABLE,	ENOEXEC		},
    {	ERROR_RELOC_CHAIN_XEEDS_SEGLIM,	ENOEXEC		},
    {	ERROR_INFLOOP_IN_RELOC_CHAIN,	ENOEXEC		},
    {	ERROR_FILENAME_EXCED_RANGE,	ENOENT		},
    {	ERROR_NESTING_NOT_ALLOWED,	EAGAIN		},
#ifndef ERROR_PIPE_LOCAL
#define ERROR_PIPE_LOCAL	229L
#endif
    {	ERROR_PIPE_LOCAL,		EPIPE		},
    {	ERROR_BAD_PIPE,			EPIPE		},
    {	ERROR_PIPE_BUSY,		EAGAIN		},
    {	ERROR_NO_DATA,			EPIPE		},
    {	ERROR_PIPE_NOT_CONNECTED,	EPIPE		},
    {	ERROR_OPERATION_ABORTED,	EINTR		},
    {	ERROR_NOT_ENOUGH_QUOTA,		ENOMEM		},
    {	ERROR_MOD_NOT_FOUND,		ENOENT		},
    {	WSAEINTR,			EINTR		},
    {	WSAEBADF,			EBADF		},
    {	WSAEACCES,			EACCES		},
    {	WSAEFAULT,			EFAULT		},
    {	WSAEINVAL,			EINVAL		},
    {	WSAEMFILE,			EMFILE		},
    {	WSAEWOULDBLOCK,			EWOULDBLOCK	},
    {	WSAEINPROGRESS,			EINPROGRESS	},
    {	WSAEALREADY,			EALREADY	},
    {	WSAENOTSOCK,			ENOTSOCK	},
    {	WSAEDESTADDRREQ,		EDESTADDRREQ	},
    {	WSAEMSGSIZE,			EMSGSIZE	},
    {	WSAEPROTOTYPE,			EPROTOTYPE	},
    {	WSAENOPROTOOPT,			ENOPROTOOPT	},
    {	WSAEPROTONOSUPPORT,		EPROTONOSUPPORT	},
    {	WSAESOCKTNOSUPPORT,		ESOCKTNOSUPPORT	},
    {	WSAEOPNOTSUPP,			EOPNOTSUPP	},
    {	WSAEPFNOSUPPORT,		EPFNOSUPPORT	},
    {	WSAEAFNOSUPPORT,		EAFNOSUPPORT	},
    {	WSAEADDRINUSE,			EADDRINUSE	},
    {	WSAEADDRNOTAVAIL,		EADDRNOTAVAIL	},
    {	WSAENETDOWN,			ENETDOWN	},
    {	WSAENETUNREACH,			ENETUNREACH	},
    {	WSAENETRESET,			ENETRESET	},
    {	WSAECONNABORTED,		ECONNABORTED	},
    {	WSAECONNRESET,			ECONNRESET	},
    {	WSAENOBUFS,			ENOBUFS		},
    {	WSAEISCONN,			EISCONN		},
    {	WSAENOTCONN,			ENOTCONN	},
    {	WSAESHUTDOWN,			ESHUTDOWN	},
    {	WSAETOOMANYREFS,		ETOOMANYREFS	},
    {	WSAETIMEDOUT,			ETIMEDOUT	},
    {	WSAECONNREFUSED,		ECONNREFUSED	},
    {	WSAELOOP,			ELOOP		},
    {	WSAENAMETOOLONG,		ENAMETOOLONG	},
    {	WSAEHOSTDOWN,			EHOSTDOWN	},
    {	WSAEHOSTUNREACH,		EHOSTUNREACH	},
    {	WSAEPROCLIM,			EPROCLIM	},
    {	WSAENOTEMPTY,			ENOTEMPTY	},
    {	WSAEUSERS,			EUSERS		},
    {	WSAEDQUOT,			EDQUOT		},
    {	WSAESTALE,			ESTALE		},
    {	WSAEREMOTE,			EREMOTE		},
};

/* License: Ruby's */
int
rb_w32_map_errno(DWORD winerr)
{
    int i;

    if (winerr == 0) {
	return 0;
    }

    for (i = 0; i < (int)(sizeof(errmap) / sizeof(*errmap)); i++) {
	if (errmap[i].winerr == winerr) {
	    return errmap[i].err;
	}
    }

    if (winerr >= WSABASEERR) {
	return winerr;
    }
    return EINVAL;
}

#define map_errno rb_w32_map_errno

static const char *NTLoginName;

static OSVERSIONINFO osver;

/* License: Artistic or GPL */
static void
get_version(void)
{
    memset(&osver, 0, sizeof(OSVERSIONINFO));
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osver);
}

#ifdef _M_IX86
/* License: Artistic or GPL */
DWORD
rb_w32_osid(void)
{
    return osver.dwPlatformId;
}
#endif

/* License: Artistic or GPL */
DWORD
rb_w32_osver(void)
{
    return osver.dwMajorVersion;
}

#define IsWinNT() rb_w32_iswinnt()
#define IsWin95() rb_w32_iswin95()
#ifdef WIN95
#define IfWin95(win95, winnt) (IsWin95() ? (win95) : (winnt))
#else
#define IfWin95(win95, winnt) (winnt)
#endif

/* simulate flock by locking a range on the file */

/* License: Artistic or GPL */
#define LK_ERR(f,i) \
    do {								\
	if (f)								\
	    i = 0;							\
	else {								\
	    DWORD err = GetLastError();					\
	    if (err == ERROR_LOCK_VIOLATION || err == ERROR_IO_PENDING)	\
		errno = EWOULDBLOCK;					\
	    else if (err == ERROR_NOT_LOCKED)				\
		i = 0;							\
	    else							\
		errno = map_errno(err);					\
	}								\
    } while (0)
#define LK_LEN      ULONG_MAX

/* License: Artistic or GPL */
static uintptr_t
flock_winnt(uintptr_t self, int argc, uintptr_t* argv)
{
    OVERLAPPED o;
    int i = -1;
    const HANDLE fh = (HANDLE)self;
    const int oper = argc;

    memset(&o, 0, sizeof(o));

    switch(oper) {
      case LOCK_SH:		/* shared lock */
	LK_ERR(LockFileEx(fh, 0, 0, LK_LEN, LK_LEN, &o), i);
	break;
      case LOCK_EX:		/* exclusive lock */
	LK_ERR(LockFileEx(fh, LOCKFILE_EXCLUSIVE_LOCK, 0, LK_LEN, LK_LEN, &o), i);
	break;
      case LOCK_SH|LOCK_NB:	/* non-blocking shared lock */
	LK_ERR(LockFileEx(fh, LOCKFILE_FAIL_IMMEDIATELY, 0, LK_LEN, LK_LEN, &o), i);
	break;
      case LOCK_EX|LOCK_NB:	/* non-blocking exclusive lock */
	LK_ERR(LockFileEx(fh,
			  LOCKFILE_EXCLUSIVE_LOCK|LOCKFILE_FAIL_IMMEDIATELY,
			  0, LK_LEN, LK_LEN, &o), i);
	break;
      case LOCK_UN:		/* unlock lock */
      case LOCK_UN|LOCK_NB:	/* unlock is always non-blocking, I hope */
	LK_ERR(UnlockFileEx(fh, 0, LK_LEN, LK_LEN, &o), i);
	break;
      default:            /* unknown */
	errno = EINVAL;
	break;
    }
    return i;
}

#ifdef WIN95
/* License: Artistic or GPL */
static uintptr_t
flock_win95(uintptr_t self, int argc, uintptr_t* argv)
{
    int i = -1;
    const HANDLE fh = (HANDLE)self;
    const int oper = argc;

    switch(oper) {
      case LOCK_EX:
	do {
	    LK_ERR(LockFile(fh, 0, 0, LK_LEN, LK_LEN), i);
	} while (i && errno == EWOULDBLOCK);
	break;
      case LOCK_EX|LOCK_NB:
	LK_ERR(LockFile(fh, 0, 0, LK_LEN, LK_LEN), i);
	break;
      case LOCK_UN:
      case LOCK_UN|LOCK_NB:
	LK_ERR(UnlockFile(fh, 0, 0, LK_LEN, LK_LEN), i);
	break;
      default:
	errno = EINVAL;
	break;
    }
    return i;
}
#endif

#undef LK_ERR

/* License: Artistic or GPL */
int
flock(int fd, int oper)
{
#ifdef WIN95
    static asynchronous_func_t locker = NULL;

    if (!locker) {
	if (IsWinNT())
	    locker = flock_winnt;
	else
	    locker = flock_win95;
    }
#else
    const asynchronous_func_t locker = flock_winnt;
#endif

    return rb_w32_asynchronize(locker,
			      (VALUE)_get_osfhandle(fd), oper, NULL,
			      (DWORD)-1);
}

/* License: Ruby's */
static inline WCHAR *
translate_wchar(WCHAR *p, int from, int to)
{
    for (; *p; p++) {
	if (*p == from)
	    *p = to;
    }
    return p;
}

/* License: Ruby's */
static inline char *
translate_char(char *p, int from, int to)
{
    while (*p) {
	if ((unsigned char)*p == from)
	    *p = to;
	p = CharNext(p);
    }
    return p;
}

#ifndef CSIDL_LOCAL_APPDATA
#define CSIDL_LOCAL_APPDATA 28
#endif
#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA 35
#endif
#ifndef CSIDL_WINDOWS
#define CSIDL_WINDOWS	36
#endif
#ifndef CSIDL_SYSTEM
#define CSIDL_SYSTEM	37
#endif
#ifndef CSIDL_PROFILE
#define CSIDL_PROFILE 40
#endif

/* License: Ruby's */
static BOOL
get_special_folder(int n, WCHAR *env)
{
    LPITEMIDLIST pidl;
    LPMALLOC alloc;
    BOOL f = FALSE;
    if (SHGetSpecialFolderLocation(NULL, n, &pidl) == 0) {
	f = SHGetPathFromIDListW(pidl, env);
	SHGetMalloc(&alloc);
	alloc->lpVtbl->Free(alloc, pidl);
	alloc->lpVtbl->Release(alloc);
    }
    return f;
}

/* License: Ruby's */
static void
regulate_path(WCHAR *path)
{
    WCHAR *p = translate_wchar(path, L'\\', L'/');
    if (p - path == 2 && path[1] == L':') {
	*p++ = L'/';
	*p = L'\0';
    }
}

/* License: Ruby's */
static FARPROC
get_proc_address(const char *module, const char *func, HANDLE *mh)
{
    HANDLE h;
    FARPROC ptr;

    if (mh)
	h = LoadLibrary(module);
    else
	h = GetModuleHandle(module);
    if (!h)
	return NULL;

    ptr = GetProcAddress(h, func);
    if (mh) {
	if (ptr)
	    *mh = h;
	else
	    FreeLibrary(h);
    }
    return ptr;
}

/* License: Ruby's */
static UINT
get_system_directory(WCHAR *path, UINT len)
{
    typedef UINT WINAPI wgetdir_func(WCHAR*, UINT);
    FARPROC ptr =
	get_proc_address("kernel32", "GetSystemWindowsDirectoryW", NULL);
    if (ptr)
	return (*(wgetdir_func *)ptr)(path, len);
    return GetWindowsDirectoryW(path, len);
}

#define numberof(array) (sizeof(array) / sizeof(*array))

/* License: Ruby's */
VALUE
rb_w32_special_folder(int type)
{
    WCHAR path[_MAX_PATH];

    if (!get_special_folder(type, path)) return Qnil;
    regulate_path(path);
    return rb_w32_conv_from_wchar(path, rb_filesystem_encoding());
}

/* License: Ruby's */
UINT
rb_w32_system_tmpdir(WCHAR *path, UINT len)
{
    static const WCHAR temp[] = L"temp";
    WCHAR *p;

    if (!get_special_folder(CSIDL_LOCAL_APPDATA, path)) {
	if (get_system_directory(path, len)) return 0;
    }
    p = translate_wchar(path, L'\\', L'/');
    if (*(p - 1) != L'/') *p++ = L'/';
    if (p - path + numberof(temp) >= len) return 0;
    memcpy(p, temp, sizeof(temp));
    return p - path + numberof(temp) - 1;
}

/* License: Ruby's */
static void
init_env(void)
{
    static const WCHAR TMPDIR[] = L"TMPDIR";
    struct {WCHAR name[6], eq, val[_MAX_PATH];} wk;
    DWORD len;
    BOOL f;
#define env wk.val
#define set_env_val(vname) do { \
	typedef char namesizecheck[numberof(wk.name) < numberof(vname) - 1 ? -1 : 1]; \
	WCHAR *const buf = wk.name + numberof(wk.name) - numberof(vname) + 1; \
	MEMCPY(buf, vname, WCHAR, numberof(vname) - 1); \
	_wputenv(buf); \
    } while (0)

    wk.eq = L'=';

    if (!GetEnvironmentVariableW(L"HOME", env, numberof(env))) {
	f = FALSE;
	if (GetEnvironmentVariableW(L"HOMEDRIVE", env, numberof(env)))
	    len = lstrlenW(env);
	else
	    len = 0;
	if (GetEnvironmentVariableW(L"HOMEPATH", env + len, numberof(env) - len) || len) {
	    f = TRUE;
	}
	else if (GetEnvironmentVariableW(L"USERPROFILE", env, numberof(env))) {
	    f = TRUE;
	}
	else if (get_special_folder(CSIDL_PROFILE, env)) {
	    f = TRUE;
	}
	else if (get_special_folder(CSIDL_PERSONAL, env)) {
	    f = TRUE;
	}
	if (f) {
	    regulate_path(env);
	    set_env_val(L"HOME");
	}
    }

    if (!GetEnvironmentVariableW(L"USER", env, numberof(env))) {
	if (!GetEnvironmentVariableW(L"USERNAME", env, numberof(env)) &&
	    !GetUserNameW(env, (len = numberof(env), &len))) {
	    NTLoginName = "<Unknown>";
	    return;
	}
	set_env_val(L"USER");
    }
    NTLoginName = strdup(rb_w32_getenv("USER"));

    if (!GetEnvironmentVariableW(TMPDIR, env, numberof(env)) &&
	!GetEnvironmentVariableW(L"TMP", env, numberof(env)) &&
	!GetEnvironmentVariableW(L"TEMP", env, numberof(env)) &&
	rb_w32_system_tmpdir(env, numberof(env))) {
	set_env_val(TMPDIR);
    }

#undef env
#undef set_env_val
}


typedef BOOL (WINAPI *cancel_io_t)(HANDLE);
static cancel_io_t cancel_io = NULL;

/* License: Ruby's */
static void
init_func(void)
{
    if (!cancel_io)
	cancel_io = (cancel_io_t)get_proc_address("kernel32", "CancelIo", NULL);
}

static void init_stdhandle(void);

#if RT_VER >= 80
/* License: Ruby's */
static void
invalid_parameter(const wchar_t *expr, const wchar_t *func, const wchar_t *file, unsigned int line, uintptr_t dummy)
{
    // nothing to do
}

int ruby_w32_rtc_error;

/* License: Ruby's */
static int __cdecl
rtc_error_handler(int e, const char *src, int line, const char *exe, const char *fmt, ...)
{
    va_list ap;
    VALUE str;

    if (!ruby_w32_rtc_error) return 0;
    str = rb_sprintf("%s:%d: ", src, line);
    va_start(ap, fmt);
    rb_str_vcatf(str, fmt, ap);
    va_end(ap);
    rb_str_cat(str, "\n", 1);
    rb_write_error2(RSTRING_PTR(str), RSTRING_LEN(str));
    return 0;
}
#endif

static CRITICAL_SECTION select_mutex;
static int NtSocketsInitialized = 0;
static st_table *socklist = NULL;
static char *envarea;
static char *uenvarea;

/* License: Ruby's */
static void
exit_handler(void)
{
    if (NtSocketsInitialized) {
	WSACleanup();
	if (socklist) {
	    st_free_table(socklist);
	    socklist = NULL;
	}
	DeleteCriticalSection(&select_mutex);
	NtSocketsInitialized = 0;
    }
    if (envarea) {
	FreeEnvironmentStrings(envarea);
	envarea = NULL;
    }
    if (uenvarea) {
	free(uenvarea);
	uenvarea = NULL;
    }
}

/* License: Artistic or GPL */
static void
StartSockets(void)
{
    WORD version;
    WSADATA retdata;

    //
    // initalize the winsock interface and insure that it's
    // cleaned up at exit.
    //
    version = MAKEWORD(2, 0);
    if (WSAStartup(version, &retdata))
	rb_fatal ("Unable to locate winsock library!\n");
    if (LOBYTE(retdata.wVersion) != 2)
	rb_fatal("could not find version 2 of winsock dll\n");

    InitializeCriticalSection(&select_mutex);

    NtSocketsInitialized = 1;
}

/* License: Ruby's */
static inline int
socklist_insert(SOCKET sock, int flag)
{
    if (!socklist)
	socklist = st_init_numtable();
    return st_insert(socklist, (st_data_t)sock, (st_data_t)flag);
}

/* License: Ruby's */
static inline int
socklist_lookup(SOCKET sock, int *flagp)
{
    st_data_t data;
    int ret;

    if (!socklist)
	return 0;
    ret = st_lookup(socklist, (st_data_t)sock, (st_data_t *)&data);
    if (ret && flagp)
	*flagp = (int)data;

    return ret;
}

/* License: Ruby's */
static inline int
socklist_delete(SOCKET *sockp, int *flagp)
{
    st_data_t key;
    st_data_t data;
    int ret;

    if (!socklist)
	return 0;
    key = (st_data_t)*sockp;
    if (flagp)
	data = (st_data_t)*flagp;
    ret = st_delete(socklist, &key, &data);
    if (ret) {
	*sockp = (SOCKET)key;
	if (flagp)
	    *flagp = (int)data;
    }

    return ret;
}

//
// Initialization stuff
//
/* License: Ruby's */
void
rb_w32_sysinit(int *argc, char ***argv)
{
#if RT_VER >= 80
    static void set_pioinfo_extra(void);

    _CrtSetReportMode(_CRT_ASSERT, 0);
    _set_invalid_parameter_handler(invalid_parameter);
    _RTC_SetErrorFunc(rtc_error_handler);
    set_pioinfo_extra();
#endif

    get_version();

    //
    // subvert cmd.exe's feeble attempt at command line parsing
    //
    *argc = rb_w32_cmdvector(GetCommandLine(), argv);

    //
    // Now set up the correct time stuff
    //

    tzset();

    init_env();

    init_func();

    init_stdhandle();

    atexit(exit_handler);

    // Initialize Winsock
    StartSockets();
}

char *
getlogin(void)
{
    return (char *)NTLoginName;
}

#define MAXCHILDNUM 256	/* max num of child processes */

/* License: Ruby's */
static struct ChildRecord {
    HANDLE hProcess;	/* process handle */
    rb_pid_t pid;	/* process id */
} ChildRecord[MAXCHILDNUM];

/* License: Ruby's */
#define FOREACH_CHILD(v) do { \
    struct ChildRecord* v; \
    for (v = ChildRecord; v < ChildRecord + sizeof(ChildRecord) / sizeof(ChildRecord[0]); ++v)
#define END_FOREACH_CHILD } while (0)

/* License: Ruby's */
static struct ChildRecord *
FindChildSlot(rb_pid_t pid)
{

    FOREACH_CHILD(child) {
	if (child->pid == pid) {
	    return child;
	}
    } END_FOREACH_CHILD;
    return NULL;
}

/* License: Ruby's */
static struct ChildRecord *
FindChildSlotByHandle(HANDLE h)
{

    FOREACH_CHILD(child) {
	if (child->hProcess == h) {
	    return child;
	}
    } END_FOREACH_CHILD;
    return NULL;
}

/* License: Ruby's */
static void
CloseChildHandle(struct ChildRecord *child)
{
    HANDLE h = child->hProcess;
    child->hProcess = NULL;
    child->pid = 0;
    CloseHandle(h);
}

/* License: Ruby's */
static struct ChildRecord *
FindFreeChildSlot(void)
{
    FOREACH_CHILD(child) {
	if (!child->pid) {
	    child->pid = -1;	/* lock the slot */
	    child->hProcess = NULL;
	    return child;
	}
    } END_FOREACH_CHILD;
    return NULL;
}


/*
  ruby -lne 'BEGIN{$cmds = Hash.new(0); $mask = 1}'
   -e '$cmds[$_.downcase] |= $mask' -e '$mask <<= 1 if ARGF.eof'
   -e 'END{$cmds.sort.each{|n,f|puts "    \"\\#{f.to_s(8)}\" #{n.dump} + 1,"}}'
   98cmd ntcmd
 */
static const char *const szInternalCmds[] = {
    "\2" "assoc" + 1,
    "\3" "break" + 1,
    "\3" "call" + 1,
    "\3" "cd" + 1,
    "\1" "chcp" + 1,
    "\3" "chdir" + 1,
    "\3" "cls" + 1,
    "\2" "color" + 1,
    "\3" "copy" + 1,
    "\1" "ctty" + 1,
    "\3" "date" + 1,
    "\3" "del" + 1,
    "\3" "dir" + 1,
    "\3" "echo" + 1,
    "\2" "endlocal" + 1,
    "\3" "erase" + 1,
    "\3" "exit" + 1,
    "\3" "for" + 1,
    "\2" "ftype" + 1,
    "\3" "goto" + 1,
    "\3" "if" + 1,
    "\1" "lfnfor" + 1,
    "\1" "lh" + 1,
    "\1" "lock" + 1,
    "\3" "md" + 1,
    "\3" "mkdir" + 1,
    "\2" "move" + 1,
    "\3" "path" + 1,
    "\3" "pause" + 1,
    "\2" "popd" + 1,
    "\3" "prompt" + 1,
    "\2" "pushd" + 1,
    "\3" "rd" + 1,
    "\3" "rem" + 1,
    "\3" "ren" + 1,
    "\3" "rename" + 1,
    "\3" "rmdir" + 1,
    "\3" "set" + 1,
    "\2" "setlocal" + 1,
    "\3" "shift" + 1,
    "\2" "start" + 1,
    "\3" "time" + 1,
    "\2" "title" + 1,
    "\1" "truename" + 1,
    "\3" "type" + 1,
    "\1" "unlock" + 1,
    "\3" "ver" + 1,
    "\3" "verify" + 1,
    "\3" "vol" + 1,
};

/* License: Ruby's */
static int
internal_match(const void *key, const void *elem)
{
    return strcmp(key, *(const char *const *)elem);
}

/* License: Ruby's */
static int
is_command_com(const char *interp)
{
    int i = strlen(interp) - 11;

    if ((i == 0 || i > 0 && isdirsep(interp[i-1])) &&
	strcasecmp(interp+i, "command.com") == 0) {
	return 1;
    }
    return 0;
}

static int internal_cmd_match(const char *cmdname, int nt);

/* License: Ruby's */
static int
is_internal_cmd(const char *cmd, int nt)
{
    char cmdname[9], *b = cmdname, c;

    do {
	if (!(c = *cmd++)) return 0;
    } while (isspace(c));
    if (c == '@')
	return 1;
    while (isalpha(c)) {
	*b++ = tolower(c);
	if (b == cmdname + sizeof(cmdname)) return 0;
	c = *cmd++;
    }
    if (c == '.') c = *cmd;
    switch (c) {
      case '<': case '>': case '|':
	return 1;
      case '\0': case ' ': case '\t': case '\n':
	break;
      default:
	return 0;
    }
    *b = 0;
    return internal_cmd_match(cmdname, nt);
}

/* License: Ruby's */
static int
internal_cmd_match(const char *cmdname, int nt)
{
    char **nm;

    nm = bsearch(cmdname, szInternalCmds,
		 sizeof(szInternalCmds) / sizeof(*szInternalCmds),
		 sizeof(*szInternalCmds),
		 internal_match);
    if (!nm || !(nm[0][-1] & (nt ? 2 : 1)))
	return 0;
    return 1;
}

/* License: Ruby's */
SOCKET
rb_w32_get_osfhandle(int fh)
{
    return _get_osfhandle(fh);
}

/* License: Ruby's */
static int
join_argv(char *cmd, char *const *argv, BOOL escape)
{
    const char *p, *s;
    char *q, *const *t;
    int len, n, bs, quote;

    for (t = argv, q = cmd, len = 0; p = *t; t++) {
	quote = 0;
	s = p;
	if (!*p || strpbrk(p, " \t\"'")) {
	    quote = 1;
	    len++;
	    if (q) *q++ = '"';
	}
	for (bs = 0; *p; ++p) {
	    switch (*p) {
	      case '\\':
		++bs;
		break;
	      case '"':
		len += n = p - s;
		if (q) {
		    memcpy(q, s, n);
		    q += n;
		}
		s = p;
		len += ++bs;
		if (q) {
		    memset(q, '\\', bs);
		    q += bs;
		}
		bs = 0;
		break;
	      case '<': case '>': case '|': case '^':
		if (escape && !quote) {
		    len += (n = p - s) + 1;
		    if (q) {
			memcpy(q, s, n);
			q += n;
			*q++ = '^';
		    }
		    s = p;
		    break;
		}
	      default:
		bs = 0;
		p = CharNext(p) - 1;
		break;
	    }
	}
	len += (n = p - s) + 1;
	if (quote) len++;
	if (q) {
	    memcpy(q, s, n);
	    q += n;
	    if (quote) *q++ = '"';
	    *q++ = ' ';
	}
    }
    if (q > cmd) --len;
    if (q) {
	if (q > cmd) --q;
	*q = '\0';
    }
    return len;
}

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#else
# define MAXPATHLEN 512
#endif

/* License: Ruby's */
#define STRNDUPV(ptr, v, src, len)					\
    (((char *)memcpy(((ptr) = ALLOCV((v), (len) + 1)), (src), (len)))[len] = 0)

/* License: Ruby's */
static int
check_spawn_mode(int mode)
{
    switch (mode) {
      case P_NOWAIT:
      case P_OVERLAY:
	return 0;
      default:
	errno = EINVAL;
	return -1;
    }
}

/* License: Ruby's */
static rb_pid_t
child_result(struct ChildRecord *child, int mode)
{
    DWORD exitcode;

    if (!child) {
	return -1;
    }

    switch (mode) {
      case P_NOWAIT:
	return child->pid;
      case P_OVERLAY:
	WaitForSingleObject(child->hProcess, INFINITE);
	GetExitCodeProcess(child->hProcess, &exitcode);
	CloseChildHandle(child);
	_exit(exitcode);
      default:
	return -1;	/* not reached */
    }
}

/* License: Ruby's */
static struct ChildRecord *
CreateChild(const WCHAR *cmd, const WCHAR *prog, SECURITY_ATTRIBUTES *psa,
	    HANDLE hInput, HANDLE hOutput, HANDLE hError)
{
    BOOL fRet;
    DWORD  dwCreationFlags;
    STARTUPINFOW aStartupInfo;
    PROCESS_INFORMATION aProcessInformation;
    SECURITY_ATTRIBUTES sa;
    struct ChildRecord *child;

    if (!cmd && !prog) {
	errno = EFAULT;
	return NULL;
    }

    child = FindFreeChildSlot();
    if (!child) {
	errno = EAGAIN;
	return NULL;
    }

    if (!psa) {
	sa.nLength              = sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle       = TRUE;
	psa = &sa;
    }

    memset(&aStartupInfo, 0, sizeof(aStartupInfo));
    memset(&aProcessInformation, 0, sizeof(aProcessInformation));
    aStartupInfo.cb = sizeof(aStartupInfo);
    aStartupInfo.dwFlags = STARTF_USESTDHANDLES;
    if (hInput) {
	aStartupInfo.hStdInput  = hInput;
    }
    else {
	aStartupInfo.hStdInput  = GetStdHandle(STD_INPUT_HANDLE);
    }
    if (hOutput) {
	aStartupInfo.hStdOutput = hOutput;
    }
    else {
	aStartupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    if (hError) {
	aStartupInfo.hStdError = hError;
    }
    else {
	aStartupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    }

    dwCreationFlags = (CREATE_NEW_PROCESS_GROUP | NORMAL_PRIORITY_CLASS);

    if (lstrlenW(cmd) > 32767) {
	child->pid = 0;		/* release the slot */
	errno = E2BIG;
	return NULL;
    }

    RUBY_CRITICAL({
	fRet = CreateProcessW(prog, (WCHAR *)cmd, psa, psa,
			      psa->bInheritHandle, dwCreationFlags, NULL, NULL,
			      &aStartupInfo, &aProcessInformation);
	errno = map_errno(GetLastError());
    });

    if (!fRet) {
	child->pid = 0;		/* release the slot */
	return NULL;
    }

    CloseHandle(aProcessInformation.hThread);

    child->hProcess = aProcessInformation.hProcess;
    child->pid = (rb_pid_t)aProcessInformation.dwProcessId;

    if (!IsWinNT()) {
	/* On Win9x, make pid positive similarly to cygwin and perl */
	child->pid = -child->pid;
    }

    return child;
}

/* License: Ruby's */
static int
is_batch(const char *cmd)
{
    int len = strlen(cmd);
    if (len <= 4) return 0;
    cmd += len - 4;
    if (*cmd++ != '.') return 0;
    if (strcasecmp(cmd, "bat") == 0) return 1;
    if (strcasecmp(cmd, "cmd") == 0) return 1;
    return 0;
}

static UINT filecp(void);
static WCHAR *mbstr_to_wstr(UINT, const char *, int, long *);
static char *wstr_to_mbstr(UINT, const WCHAR *, int, long *);
#define acp_to_wstr(str, plen) mbstr_to_wstr(CP_ACP, str, -1, plen)
#define wstr_to_acp(str, plen) wstr_to_mbstr(CP_ACP, str, -1, plen)
#define filecp_to_wstr(str, plen) mbstr_to_wstr(filecp(), str, -1, plen)
#define wstr_to_filecp(str, plen) wstr_to_mbstr(filecp(), str, -1, plen)
#define utf8_to_wstr(str, plen) mbstr_to_wstr(CP_UTF8, str, -1, plen)
#define wstr_to_utf8(str, plen) wstr_to_mbstr(CP_UTF8, str, -1, plen)

/* License: Artistic or GPL */
rb_pid_t
rb_w32_spawn(int mode, const char *cmd, const char *prog)
{
    char fbuf[MAXPATHLEN];
    char *p = NULL;
    const char *shell = NULL;
    WCHAR *wcmd, *wshell;
    rb_pid_t ret;
    VALUE v = 0;
    VALUE v2 = 0;

    if (check_spawn_mode(mode)) return -1;

    if (prog) {
	if (!(p = dln_find_exe_r(prog, NULL, fbuf, sizeof(fbuf)))) {
	    shell = prog;
	}
	else {
	    shell = p;
	    translate_char(p, '/', '\\');
	}
    }
    else {
	int redir = -1;
	int nt;
	while (ISSPACE(*cmd)) cmd++;
	if ((shell = getenv("RUBYSHELL")) && (redir = has_redirection(cmd))) {
	    char *tmp = ALLOCV(v, strlen(shell) + strlen(cmd) + sizeof(" -c ") + 2);
	    sprintf(tmp, "%s -c \"%s\"", shell, cmd);
	    cmd = tmp;
	}
	else if ((shell = getenv("COMSPEC")) &&
		 (nt = !is_command_com(shell),
		  (redir < 0 ? has_redirection(cmd) : redir) ||
		  is_internal_cmd(cmd, nt))) {
	    char *tmp = ALLOCV(v, strlen(shell) + strlen(cmd) + sizeof(" /c ") + (nt ? 2 : 0));
	    sprintf(tmp, nt ? "%s /c \"%s\"" : "%s /c %s", shell, cmd);
	    cmd = tmp;
	}
	else {
	    int len = 0, quote = (*cmd == '"') ? '"' : (*cmd == '\'') ? '\'' : 0;
	    for (prog = cmd + !!quote;; prog = CharNext(prog)) {
		if (!*prog) {
		    len = prog - cmd;
		    shell = cmd;
		    break;
		}
		if ((unsigned char)*prog == quote) {
		    len = prog++ - cmd - 1;
		    STRNDUPV(p, v2, cmd + 1, len);
		    shell = p;
		    break;
		}
		if (quote) continue;
		if (ISSPACE(*prog) || strchr("<>|*?\"", *prog)) {
		    len = prog - cmd;
		    STRNDUPV(p, v2, cmd, len);
		    shell = p;
		    break;
		}
	    }
	    shell = dln_find_exe_r(shell, NULL, fbuf, sizeof(fbuf));
	    if (!shell) {
		shell = p ? p : cmd;
	    }
	    else {
		len = strlen(shell);
		if (strchr(shell, ' ')) quote = -1;
		if (shell == fbuf) {
		    p = fbuf;
		}
		else if (shell != p && strchr(shell, '/')) {
		    STRNDUPV(p, v2, shell, len);
		    shell = p;
		}
		if (p) translate_char(p, '/', '\\');
		if (is_batch(shell)) {
		    int alen = strlen(prog);
		    cmd = p = ALLOCV(v, len + alen + (quote ? 2 : 0) + 1);
		    if (quote) *p++ = '"';
		    memcpy(p, shell, len);
		    p += len;
		    if (quote) *p++ = '"';
		    memcpy(p, prog, alen + 1);
		    shell = 0;
		}
	    }
	}
    }

    /* assume ACP */
    wcmd = cmd ? acp_to_wstr(cmd, NULL) : NULL;
    if (v) ALLOCV_END(v);
    wshell = shell ? acp_to_wstr(shell, NULL) : NULL;
    if (v2) ALLOCV_END(v2);

    ret = child_result(CreateChild(wcmd, wshell, NULL, NULL, NULL, NULL), mode);
    free(wshell);
    free(wcmd);
    return ret;
}

/* License: Artistic or GPL */
rb_pid_t
rb_w32_aspawn(int mode, const char *prog, char *const *argv)
{
    int c_switch = 0;
    size_t len;
    BOOL ntcmd = FALSE, tmpnt;
    const char *shell;
    char *cmd, fbuf[MAXPATHLEN];
    WCHAR *wcmd, *wprog;
    rb_pid_t ret;
    VALUE v = 0;

    if (check_spawn_mode(mode)) return -1;

    if (!prog) prog = argv[0];
    if ((shell = getenv("COMSPEC")) &&
	internal_cmd_match(prog, tmpnt = !is_command_com(shell))) {
	ntcmd = tmpnt;
	prog = shell;
	c_switch = 1;
    }
    else if ((cmd = dln_find_exe_r(prog, NULL, fbuf, sizeof(fbuf)))) {
	if (cmd == prog) strlcpy(cmd = fbuf, prog, sizeof(fbuf));
	translate_char(cmd, '/', '\\');
	prog = cmd;
    }
    else if (strchr(prog, '/')) {
	len = strlen(prog);
	if (len < sizeof(fbuf))
	    strlcpy(cmd = fbuf, prog, sizeof(fbuf));
	else
	    STRNDUPV(cmd, v, prog, len);
	translate_char(cmd, '/', '\\');
	prog = cmd;
    }
    if (c_switch || is_batch(prog)) {
	char *progs[2];
	progs[0] = (char *)prog;
	progs[1] = NULL;
	len = join_argv(NULL, progs, ntcmd);
	if (c_switch) len += 3;
	else ++argv;
	if (argv[0]) len += join_argv(NULL, argv, ntcmd);
	cmd = ALLOCV(v, len);
	join_argv(cmd, progs, ntcmd);
	if (c_switch) strlcat(cmd, " /c", len);
	if (argv[0]) join_argv(cmd + strlcat(cmd, " ", len), argv, ntcmd);
	prog = c_switch ? shell : 0;
    }
    else {
	len = join_argv(NULL, argv, FALSE);
	cmd = ALLOCV(v, len);
	join_argv(cmd, argv, FALSE);
    }

    /* assume ACP */
    wcmd = cmd ? acp_to_wstr(cmd, NULL) : NULL;
    if (v) ALLOCV_END(v);
    wprog = prog ? acp_to_wstr(prog, NULL) : NULL;

    ret = child_result(CreateChild(wcmd, wprog, NULL, NULL, NULL, NULL), mode);
    free(wprog);
    free(wcmd);
    return ret;
}

/* License: Artistic or GPL */
typedef struct _NtCmdLineElement {
    struct _NtCmdLineElement *next;
    char *str;
    int len;
    int flags;
} NtCmdLineElement;

//
// Possible values for flags
//

#define NTGLOB   0x1	// element contains a wildcard
#define NTMALLOC 0x2	// string in element was malloc'ed
#define NTSTRING 0x4	// element contains a quoted string

/* License: Ruby's */
static int
insert(const char *path, VALUE vinfo, void *enc)
{
    NtCmdLineElement *tmpcurr;
    NtCmdLineElement ***tail = (NtCmdLineElement ***)vinfo;

    tmpcurr = (NtCmdLineElement *)malloc(sizeof(NtCmdLineElement));
    if (!tmpcurr) return -1;
    MEMZERO(tmpcurr, NtCmdLineElement, 1);
    tmpcurr->len = strlen(path);
    tmpcurr->str = strdup(path);
    if (!tmpcurr->str) return -1;
    tmpcurr->flags |= NTMALLOC;
    **tail = tmpcurr;
    *tail = &tmpcurr->next;

    return 0;
}

/* License: Artistic or GPL */
static NtCmdLineElement **
cmdglob(NtCmdLineElement *patt, NtCmdLineElement **tail)
{
    char buffer[MAXPATHLEN], *buf = buffer;
    char *p;
    NtCmdLineElement **last = tail;
    int status;

    if (patt->len >= MAXPATHLEN)
	if (!(buf = malloc(patt->len + 1))) return 0;

    strlcpy(buf, patt->str, patt->len + 1);
    buf[patt->len] = '\0';
    for (p = buf; *p; p = CharNext(p))
	if (*p == '\\')
	    *p = '/';
    status = ruby_brace_glob(buf, 0, insert, (VALUE)&tail);
    if (buf != buffer)
	free(buf);

    if (status || last == tail) return 0;
    if (patt->flags & NTMALLOC)
	free(patt->str);
    free(patt);
    return tail;
}

//
// Check a command string to determine if it has I/O redirection
// characters that require it to be executed by a command interpreter
//

/* License: Artistic or GPL */
static int
has_redirection(const char *cmd)
{
    char quote = '\0';
    const char *ptr;

    //
    // Scan the string, looking for redirection characters (< or >), pipe
    // character (|) or newline (\n) that are not in a quoted string
    //

    for (ptr = cmd; *ptr;) {
	switch (*ptr) {
	  case '\'':
	  case '\"':
	    if (!quote)
		quote = *ptr;
	    else if (quote == *ptr)
		quote = '\0';
	    ptr++;
	    break;

	  case '>':
	  case '<':
	  case '|':
	  case '\n':
	    if (!quote)
		return TRUE;
	    ptr++;
	    break;

	  case '%':
	    if (*++ptr != '_' && !ISALPHA(*ptr)) break;
	    while (*++ptr == '_' || ISALNUM(*ptr));
	    if (*ptr++ == '%') return TRUE;
	    break;

	  case '\\':
	    ptr++;
	  default:
	    ptr = CharNext(ptr);
	    break;
	}
    }
    return FALSE;
}

/* License: Ruby's */
static inline char *
skipspace(char *ptr)
{
    while (ISSPACE(*ptr))
	ptr++;
    return ptr;
}

/* License: Artistic or GPL */
int
rb_w32_cmdvector(const char *cmd, char ***vec)
{
    int globbing, len;
    int elements, strsz, done;
    int slashes, escape;
    char *ptr, *base, *buffer, *cmdline;
    char **vptr;
    char quote;
    NtCmdLineElement *curr, **tail;
    NtCmdLineElement *cmdhead = NULL, **cmdtail = &cmdhead;

    //
    // just return if we don't have a command line
    //

    while (ISSPACE(*cmd))
	cmd++;
    if (!*cmd) {
	*vec = NULL;
	return 0;
    }

    ptr = cmdline = strdup(cmd);

    //
    // Ok, parse the command line, building a list of CmdLineElements.
    // When we've finished, and it's an input command (meaning that it's
    // the processes argv), we'll do globing and then build the argument
    // vector.
    // The outer loop does one interation for each element seen.
    // The inner loop does one interation for each character in the element.
    //

    while (*(ptr = skipspace(ptr))) {
	base = ptr;
	quote = slashes = globbing = escape = 0;
	for (done = 0; !done && *ptr; ) {
	    //
	    // Switch on the current character. We only care about the
	    // white-space characters, the  wild-card characters, and the
	    // quote characters.
	    //

	    switch (*ptr) {
	      case '\\':
		if (quote != '\'') slashes++;
	        break;

	      case ' ':
	      case '\t':
	      case '\n':
		//
		// if we're not in a string, then we're finished with this
		// element
		//

		if (!quote) {
		    *ptr = 0;
		    done = 1;
		}
		break;

	      case '*':
	      case '?':
	      case '[':
	      case '{':
		//
		// record the fact that this element has a wildcard character
		// N.B. Don't glob if inside a single quoted string
		//

		if (quote != '\'')
		    globbing++;
		slashes = 0;
		break;

	      case '\'':
	      case '\"':
		//
		// if we're already in a string, see if this is the
		// terminating close-quote. If it is, we're finished with
		// the string, but not neccessarily with the element.
		// If we're not already in a string, start one.
		//

		if (!(slashes & 1)) {
		    if (!quote)
			quote = *ptr;
		    else if (quote == *ptr) {
			if (quote == '"' && quote == ptr[1])
			    ptr++;
			quote = '\0';
		    }
		}
		escape++;
		slashes = 0;
		break;

	      default:
		ptr = CharNext(ptr);
		slashes = 0;
		continue;
	    }
	    ptr++;
	}

	//
	// when we get here, we've got a pair of pointers to the element,
	// base and ptr. Base points to the start of the element while ptr
	// points to the character following the element.
	//

	len = ptr - base;
	if (done) --len;

	//
	// if it's an input vector element and it's enclosed by quotes,
	// we can remove them.
	//

	if (escape) {
	    char *p = base, c;
	    slashes = quote = 0;
	    while (p < base + len) {
		switch (c = *p) {
		  case '\\':
		    p++;
		    if (quote != '\'') slashes++;
		    break;

		  case '\'':
		  case '"':
		    if (!(slashes & 1) && quote && quote != c) {
			p++;
			slashes = 0;
			break;
		    }
		    memcpy(p - ((slashes + 1) >> 1), p + (~slashes & 1),
			   base + len - p);
		    len -= ((slashes + 1) >> 1) + (~slashes & 1);
		    p -= (slashes + 1) >> 1;
		    if (!(slashes & 1)) {
			if (quote) {
			    if (quote == '"' && quote == *p)
				p++;
			    quote = '\0';
			}
			else
			    quote = c;
		    }
		    else
			p++;
		    slashes = 0;
		    break;

		  default:
		    p = CharNext(p);
		    slashes = 0;
		    break;
		}
	    }
	}

	curr = (NtCmdLineElement *)calloc(sizeof(NtCmdLineElement), 1);
	if (!curr) goto do_nothing;
	curr->str = base;
	curr->len = len;

	if (globbing && (tail = cmdglob(curr, cmdtail))) {
	    cmdtail = tail;
	}
	else {
	    *cmdtail = curr;
	    cmdtail = &curr->next;
	}
    }

    //
    // Almost done!
    // Count up the elements, then allocate space for a vector of pointers
    // (argv) and a string table for the elements.
    //

    for (elements = 0, strsz = 0, curr = cmdhead; curr; curr = curr->next) {
	elements++;
	strsz += (curr->len + 1);
    }

    len = (elements+1)*sizeof(char *) + strsz;
    buffer = (char *)malloc(len);
    if (!buffer) {
      do_nothing:
	while (curr = cmdhead) {
	    cmdhead = curr->next;
	    if (curr->flags & NTMALLOC) free(curr->str);
	    free(curr);
	}
	free(cmdline);
	for (vptr = *vec; *vptr; ++vptr);
	return vptr - *vec;
    }

    //
    // make vptr point to the start of the buffer
    // and ptr point to the area we'll consider the string table.
    //
    //   buffer (*vec)
    //   |
    //   V       ^---------------------V
    //   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //   |   |       | ....  | NULL  |   | ..... |\0 |   | ..... |\0 |...
    //   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    //   |-  elements+1             -| ^ 1st element   ^ 2nd element

    vptr = (char **) buffer;

    ptr = buffer + (elements+1) * sizeof(char *);

    while (curr = cmdhead) {
	strlcpy(ptr, curr->str, curr->len + 1);
	*vptr++ = ptr;
	ptr += curr->len + 1;
	cmdhead = curr->next;
	if (curr->flags & NTMALLOC) free(curr->str);
	free(curr);
    }
    *vptr = 0;

    *vec = (char **) buffer;
    free(cmdline);
    return elements;
}

//
// UNIX compatible directory access functions for NT
//

#define PATHLEN 1024

//
// The idea here is to read all the directory names into a string table
// (separated by nulls) and when one of the other dir functions is called
// return the pointer to the current file name.
//

/* License: Ruby's */
#define GetBit(bits, i) ((bits)[(i) / CHAR_BIT] &  (1 << (i) % CHAR_BIT))
#define SetBit(bits, i) ((bits)[(i) / CHAR_BIT] |= (1 << (i) % CHAR_BIT))

#define BitOfIsDir(n) ((n) * 2)
#define BitOfIsRep(n) ((n) * 2 + 1)
#define DIRENT_PER_CHAR (CHAR_BIT / 2)

/* License: Artistic or GPL */
static HANDLE
open_dir_handle(const WCHAR *filename, WIN32_FIND_DATAW *fd)
{
    HANDLE fh;
    static const WCHAR wildcard[] = L"\\*";
    WCHAR *scanname;
    WCHAR *p;
    int len;
    VALUE v;

    //
    // Create the search pattern
    //
    len = lstrlenW(filename);
    scanname = ALLOCV_N(WCHAR, v, len + sizeof(wildcard) / sizeof(WCHAR));
    lstrcpyW(scanname, filename);
    p = CharPrevW(scanname, scanname + len);
    if (*p == L'/' || *p == L'\\' || *p == L':')
	lstrcatW(scanname, wildcard + 1);
    else
	lstrcatW(scanname, wildcard);

    //
    // do the FindFirstFile call
    //
    fh = FindFirstFileW(scanname, fd);
    ALLOCV_END(v);
    if (fh == INVALID_HANDLE_VALUE) {
	errno = map_errno(GetLastError());
    }
    return fh;
}

/* License: Artistic or GPL */
static DIR *
opendir_internal(WCHAR *wpath, const char *filename)
{
    struct stati64 sbuf;
    WIN32_FIND_DATAW fd;
    HANDLE fh;
    DIR *p;
    long len;
    long idx;
    WCHAR *tmpW;
    char *tmp;

    //
    // check to see if we've got a directory
    //
    if (wstati64(wpath, &sbuf) < 0) {
	return NULL;
    }
    if (!(sbuf.st_mode & S_IFDIR) &&
	(!ISALPHA(filename[0]) || filename[1] != ':' || filename[2] != '\0' ||
	 ((1 << ((filename[0] & 0x5f) - 'A')) & GetLogicalDrives()) == 0)) {
	errno = ENOTDIR;
	return NULL;
    }
    fh = open_dir_handle(wpath, &fd);
    if (fh == INVALID_HANDLE_VALUE) {
	return NULL;
    }

    //
    // Get us a DIR structure
    //
    p = calloc(sizeof(DIR), 1);
    if (p == NULL)
	return NULL;

    idx = 0;

    //
    // loop finding all the files that match the wildcard
    // (which should be all of them in this directory!).
    // the variable idx should point one past the null terminator
    // of the previous string found.
    //
    do {
	len = lstrlenW(fd.cFileName) + 1;

	//
	// bump the string table size by enough for the
	// new name and it's null terminator
	//
	tmpW = realloc(p->start, (idx + len) * sizeof(WCHAR));
	if (!tmpW) {
	  error:
	    rb_w32_closedir(p);
	    FindClose(fh);
	    errno = ENOMEM;
	    return NULL;
	}

	p->start = tmpW;
	memcpy(&p->start[idx], fd.cFileName, len * sizeof(WCHAR));

	if (p->nfiles % DIRENT_PER_CHAR == 0) {
	    tmp = realloc(p->bits, p->nfiles / DIRENT_PER_CHAR + 1);
	    if (!tmp)
		goto error;
	    p->bits = tmp;
	    p->bits[p->nfiles / DIRENT_PER_CHAR] = 0;
	}
	if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	    SetBit(p->bits, BitOfIsDir(p->nfiles));
	if (fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
	    SetBit(p->bits, BitOfIsRep(p->nfiles));

	p->nfiles++;
	idx += len;
    } while (FindNextFileW(fh, &fd));
    FindClose(fh);
    p->size = idx;
    p->curr = p->start;
    return p;
}

/* License: Ruby's */
static inline UINT
filecp(void)
{
    UINT cp = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
    return cp;
}

/* License: Ruby's */
static char *
wstr_to_mbstr(UINT cp, const WCHAR *wstr, int clen, long *plen)
{
    char *ptr;
    int len = WideCharToMultiByte(cp, 0, wstr, clen, NULL, 0, NULL, NULL) - 1;
    if (!(ptr = malloc(len + 1))) return 0;
    WideCharToMultiByte(cp, 0, wstr, clen, ptr, len + 1, NULL, NULL);
    if (plen) *plen = len;
    return ptr;
}

/* License: Ruby's */
static WCHAR *
mbstr_to_wstr(UINT cp, const char *str, int clen, long *plen)
{
    WCHAR *ptr;
    int len = MultiByteToWideChar(cp, 0, str, clen, NULL, 0) - 1;
    if (!(ptr = malloc(sizeof(WCHAR) * (len + 1)))) return 0;
    MultiByteToWideChar(cp, 0, str, clen, ptr, len + 1);
    if (plen) *plen = len;
    return ptr;
}

/* License: Ruby's */
DIR *
rb_w32_opendir(const char *filename)
{
    DIR *ret;
    WCHAR *wpath = filecp_to_wstr(filename, NULL);
    if (!wpath)
	return NULL;
    ret = opendir_internal(wpath, filename);
    free(wpath);
    return ret;
}

/* License: Ruby's */
DIR *
rb_w32_uopendir(const char *filename)
{
    DIR *ret;
    WCHAR *wpath = utf8_to_wstr(filename, NULL);
    if (!wpath)
	return NULL;
    ret = opendir_internal(wpath, filename);
    free(wpath);
    return ret;
}

//
// Move to next entry
//

/* License: Artistic or GPL */
static void
move_to_next_entry(DIR *dirp)
{
    if (dirp->curr) {
	dirp->loc++;
	dirp->curr += lstrlenW(dirp->curr) + 1;
	if (dirp->curr >= (dirp->start + dirp->size)) {
	    dirp->curr = NULL;
	}
    }
}

//
// Readdir just returns the current string pointer and bumps the
// string pointer to the next entry.
//
/* License: Ruby's */
static BOOL
win32_direct_conv(const WCHAR *file, struct direct *entry, rb_encoding *dummy)
{
    if (!(entry->d_name = wstr_to_filecp(file, &entry->d_namlen)))
	return FALSE;
    return TRUE;
}

/* License: Ruby's */
VALUE
rb_w32_conv_from_wchar(const WCHAR *wstr, rb_encoding *enc)
{
    static rb_encoding *utf16 = (rb_encoding *)-1;
    VALUE src;

    if (utf16 == (rb_encoding *)-1) {
	utf16 = rb_enc_find("UTF-16LE");
	if (utf16 == rb_ascii8bit_encoding())
	    utf16 = NULL;
    }
    if (!utf16)
	/* maybe miniruby */
	return Qnil;

    src = rb_enc_str_new((char *)wstr, lstrlenW(wstr) * sizeof(WCHAR), utf16);
    return rb_str_encode(src, rb_enc_from_encoding(enc), ECONV_UNDEF_REPLACE, Qnil);
}

/* License: Ruby's */
char *
rb_w32_conv_from_wstr(const WCHAR *wstr, long *lenp, rb_encoding *enc)
{
    VALUE str = rb_w32_conv_from_wchar(wstr, enc);
    long len;
    char *ptr;

    if (NIL_P(str)) return wstr_to_filecp(wstr, lenp);
    *lenp = len = RSTRING_LEN(str);
    memcpy(ptr = malloc(len + 1), RSTRING_PTR(str), len);
    ptr[len] = '\0';
    return ptr;
}

/* License: Ruby's */
static BOOL
ruby_direct_conv(const WCHAR *file, struct direct *entry, rb_encoding *enc)
{
    if (!(entry->d_name = rb_w32_conv_from_wstr(file, &entry->d_namlen, enc)))
	return FALSE;
    return TRUE;
}

/* License: Artistic or GPL */
static struct direct *
readdir_internal(DIR *dirp, BOOL (*conv)(const WCHAR *, struct direct *, rb_encoding *), rb_encoding *enc)
{
    static int dummy = 0;

    if (dirp->curr) {

	//
	// first set up the structure to return
	//
	if (dirp->dirstr.d_name)
	    free(dirp->dirstr.d_name);
	conv(dirp->curr, &dirp->dirstr, enc);

	//
	// Fake inode
	//
	dirp->dirstr.d_ino = dummy++;

	//
	// Attributes
	//
	dirp->dirstr.d_isdir = GetBit(dirp->bits, BitOfIsDir(dirp->loc));
	dirp->dirstr.d_isrep = GetBit(dirp->bits, BitOfIsRep(dirp->loc));

	//
	// Now set up for the next call to readdir
	//

	move_to_next_entry(dirp);

	return &(dirp->dirstr);

    }
    else
	return NULL;
}

/* License: Ruby's */
struct direct  *
rb_w32_readdir(DIR *dirp)
{
    return readdir_internal(dirp, win32_direct_conv, NULL);
}

/* License: Ruby's */
struct direct  *
rb_w32_readdir_with_enc(DIR *dirp, rb_encoding *enc)
{
    if (enc == rb_ascii8bit_encoding())
	return readdir_internal(dirp, win32_direct_conv, NULL);
    else
	return readdir_internal(dirp, ruby_direct_conv, enc);
}

//
// Telldir returns the current string pointer position
//

/* License: Artistic or GPL */
long
rb_w32_telldir(DIR *dirp)
{
    return dirp->loc;
}

//
// Seekdir moves the string pointer to a previously saved position
// (Saved by telldir).

/* License: Ruby's */
void
rb_w32_seekdir(DIR *dirp, long loc)
{
    if (dirp->loc > loc) rb_w32_rewinddir(dirp);

    while (dirp->curr && dirp->loc < loc) {
	move_to_next_entry(dirp);
    }
}

//
// Rewinddir resets the string pointer to the start
//

/* License: Artistic or GPL */
void
rb_w32_rewinddir(DIR *dirp)
{
    dirp->curr = dirp->start;
    dirp->loc = 0;
}

//
// This just free's the memory allocated by opendir
//

/* License: Artistic or GPL */
void
rb_w32_closedir(DIR *dirp)
{
    if (dirp) {
	if (dirp->dirstr.d_name)
	    free(dirp->dirstr.d_name);
	if (dirp->start)
	    free(dirp->start);
	if (dirp->bits)
	    free(dirp->bits);
	free(dirp);
    }
}

#if (defined _MT || defined __MSVCRT__) && !defined __BORLANDC__
#define MSVCRT_THREADS
#endif
#ifdef MSVCRT_THREADS
# define MTHREAD_ONLY(x) x
# define STHREAD_ONLY(x)
#elif defined(__BORLANDC__)
# define MTHREAD_ONLY(x)
# define STHREAD_ONLY(x)
#else
# define MTHREAD_ONLY(x)
# define STHREAD_ONLY(x) x
#endif

/* License: Ruby's */
typedef struct	{
    intptr_t osfhnd;	/* underlying OS file HANDLE */
    char osfile;	/* attributes of file (e.g., open in text mode?) */
    char pipech;	/* one char buffer for handles opened on pipes */
#ifdef MSVCRT_THREADS
    int lockinitflag;
    CRITICAL_SECTION lock;
#endif
#if RT_VER >= 80
    char textmode;
    char pipech2[2];
#endif
}	ioinfo;

#if !defined _CRTIMP || defined __MINGW32__
#undef _CRTIMP
#define _CRTIMP __declspec(dllimport)
#endif

#if !defined(__BORLANDC__)
EXTERN_C _CRTIMP ioinfo * __pioinfo[];
static inline ioinfo* _pioinfo(int);

#define IOINFO_L2E			5
#define IOINFO_ARRAY_ELTS	(1 << IOINFO_L2E)
#define _osfhnd(i)  (_pioinfo(i)->osfhnd)
#define _osfile(i)  (_pioinfo(i)->osfile)
#define _pipech(i)  (_pioinfo(i)->pipech)

#if RT_VER >= 80
static size_t pioinfo_extra = 0;	/* workaround for VC++8 SP1 */

/* License: Ruby's */
static void
set_pioinfo_extra(void)
{
    int fd;

    fd = _open("NUL", O_RDONLY);
    for (pioinfo_extra = 0; pioinfo_extra <= 64; pioinfo_extra += sizeof(void *)) {
	if (_osfhnd(fd) == _get_osfhandle(fd)) {
	    break;
	}
    }
    _close(fd);

    if (pioinfo_extra > 64) {
	/* not found, maybe something wrong... */
	pioinfo_extra = 0;
    }
}
#else
#define pioinfo_extra 0
#endif

static inline ioinfo*
_pioinfo(int fd)
{
    const size_t sizeof_ioinfo = sizeof(ioinfo) + pioinfo_extra;
    return (ioinfo*)((char*)__pioinfo[fd >> IOINFO_L2E] +
		     (fd & (IOINFO_ARRAY_ELTS - 1)) * sizeof_ioinfo);
}

#define _set_osfhnd(fh, osfh) (void)(_osfhnd(fh) = osfh)
#define _set_osflags(fh, flags) (_osfile(fh) = (flags))

#define FOPEN			0x01	/* file handle open */
#define FEOFLAG			0x02	/* end of file has been encountered */
#define FPIPE			0x08	/* file handle refers to a pipe */
#define FNOINHERIT		0x10	/* file handle opened O_NOINHERIT */
#define FAPPEND			0x20	/* file handle opened O_APPEND */
#define FDEV			0x40	/* file handle refers to device */
#define FTEXT			0x80	/* file handle is in text mode */

static int is_socket(SOCKET);
static int is_console(SOCKET);

/* License: Ruby's */
int
rb_w32_io_cancelable_p(int fd)
{
    return cancel_io != NULL && (is_socket(TO_SOCKET(fd)) || !is_console(TO_SOCKET(fd)));
}

/* License: Ruby's */
static int
rb_w32_open_osfhandle(intptr_t osfhandle, int flags)
{
    int fh;
    char fileflags;		/* _osfile flags */
    HANDLE hF;

    /* copy relevant flags from second parameter */
    fileflags = FDEV;

    if (flags & O_APPEND)
	fileflags |= FAPPEND;

    if (flags & O_TEXT)
	fileflags |= FTEXT;

    if (flags & O_NOINHERIT)
	fileflags |= FNOINHERIT;

    /* attempt to allocate a C Runtime file handle */
    hF = CreateFile("NUL", 0, 0, NULL, OPEN_ALWAYS, 0, NULL);
    fh = _open_osfhandle((intptr_t)hF, 0);
    CloseHandle(hF);
    if (fh == -1) {
	errno = EMFILE;		/* too many open files */
	_doserrno = 0L;		/* not an OS error */
    }
    else {

	MTHREAD_ONLY(EnterCriticalSection(&(_pioinfo(fh)->lock)));
	/* the file is open. now, set the info in _osfhnd array */
	_set_osfhnd(fh, osfhandle);

	fileflags |= FOPEN;		/* mark as open */

	_set_osflags(fh, fileflags); /* set osfile entry */
	MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fh)->lock));
    }
    return fh;			/* return handle */
}

/* License: Ruby's */
static void
init_stdhandle(void)
{
    int nullfd = -1;
    int keep = 0;
#define open_null(fd)						\
    (((nullfd < 0) ?						\
      (nullfd = open("NUL", O_RDWR)) : 0),		\
     ((nullfd == (fd)) ? (keep = 1) : dup2(nullfd, fd)),	\
     (fd))

    if (fileno(stdin) < 0) {
	stdin->_file = open_null(0);
    }
    else {
	setmode(fileno(stdin), O_BINARY);
    }
    if (fileno(stdout) < 0) {
	stdout->_file = open_null(1);
    }
    if (fileno(stderr) < 0) {
	stderr->_file = open_null(2);
    }
    if (nullfd >= 0 && !keep) close(nullfd);
    setvbuf(stderr, NULL, _IONBF, 0);
}
#else

#define _set_osfhnd(fh, osfh) (void)((fh), (osfh))
#define _set_osflags(fh, flags) (void)((fh), (flags))

/* License: Ruby's */
static void
init_stdhandle(void)
{
}
#endif

/* License: Ruby's */
#ifdef __BORLANDC__
static int
rb_w32_open_osfhandle(intptr_t osfhandle, int flags)
{
    int fd = _open_osfhandle(osfhandle, flags);
    if (fd == -1) {
	errno = EMFILE;		/* too many open files */
	_doserrno = 0L;		/* not an OS error */
    }
    return fd;
}
#endif

#undef getsockopt

/* License: Ruby's */
static int
is_socket(SOCKET sock)
{
    if (socklist_lookup(sock, NULL))
	return TRUE;
    else
	return FALSE;
}

/* License: Ruby's */
int
rb_w32_is_socket(int fd)
{
    return is_socket(TO_SOCKET(fd));
}

//
// Since the errors returned by the socket error function
// WSAGetLastError() are not known by the library routine strerror
// we have to roll our own.
//

#undef strerror

/* License: Artistic or GPL */
char *
rb_w32_strerror(int e)
{
    static char buffer[512];
    DWORD source = 0;
    char *p;

#if defined __BORLANDC__ && defined ENOTEMPTY // _sys_errlist is broken
    switch (e) {
      case ENAMETOOLONG:
	return "Filename too long";
      case ENOTEMPTY:
	return "Directory not empty";
    }
#endif

    if (e < 0 || e > sys_nerr) {
	if (e < 0)
	    e = GetLastError();
#if WSAEWOULDBLOCK != EWOULDBLOCK
	else if (e >= EADDRINUSE && e <= EWOULDBLOCK) {
	    static int s = -1;
	    int i;
	    if (s < 0)
		for (s = 0; s < (int)(sizeof(errmap)/sizeof(*errmap)); s++)
		    if (errmap[s].winerr == WSAEWOULDBLOCK)
			break;
	    for (i = s; i < (int)(sizeof(errmap)/sizeof(*errmap)); i++)
		if (errmap[i].err == e) {
		    e = errmap[i].winerr;
		    break;
		}
	}
#endif
	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
			  FORMAT_MESSAGE_IGNORE_INSERTS, &source, e,
			  MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			  buffer, sizeof(buffer), NULL) == 0 &&
	    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
			  FORMAT_MESSAGE_IGNORE_INSERTS, &source, e, 0,
			  buffer, sizeof(buffer), NULL) == 0)
	    strlcpy(buffer, "Unknown Error", sizeof(buffer));
    }
    else
	strlcpy(buffer, strerror(e), sizeof(buffer));

    p = buffer;
    while ((p = strpbrk(p, "\r\n")) != NULL) {
	memmove(p, p + 1, strlen(p));
    }
    return buffer;
}

//
// various stubs
//


// Ownership
//
// Just pretend that everyone is a superuser. NT will let us know if
// we don't really have permission to do something.
//

#define ROOT_UID	0
#define ROOT_GID	0

/* License: Artistic or GPL */
rb_uid_t
getuid(void)
{
	return ROOT_UID;
}

/* License: Artistic or GPL */
rb_uid_t
geteuid(void)
{
	return ROOT_UID;
}

/* License: Artistic or GPL */
rb_gid_t
getgid(void)
{
	return ROOT_GID;
}

/* License: Artistic or GPL */
rb_gid_t
getegid(void)
{
    return ROOT_GID;
}

/* License: Artistic or GPL */
int
setuid(rb_uid_t uid)
{
    return (uid == ROOT_UID ? 0 : -1);
}

/* License: Artistic or GPL */
int
setgid(rb_gid_t gid)
{
    return (gid == ROOT_GID ? 0 : -1);
}

//
// File system stuff
//

/* License: Artistic or GPL */
int
ioctl(int i, int u, ...)
{
    errno = EINVAL;
    return -1;
}

#undef FD_SET

void
rb_w32_fdset(int fd, fd_set *set)
{
    unsigned int i;
    SOCKET s = TO_SOCKET(fd);

    for (i = 0; i < set->fd_count; i++) {
        if (set->fd_array[i] == s) {
            return;
        }
    }
    if (i == set->fd_count) {
        if (set->fd_count < FD_SETSIZE) {
            set->fd_array[i] = s;
            set->fd_count++;
        }
    }
}

#undef FD_CLR

/* License: Ruby's */
void
rb_w32_fdclr(int fd, fd_set *set)
{
    unsigned int i;
    SOCKET s = TO_SOCKET(fd);

    for (i = 0; i < set->fd_count; i++) {
        if (set->fd_array[i] == s) {
	    memmove(&set->fd_array[i], &set->fd_array[i+1],
		    sizeof(set->fd_array[0]) * (--set->fd_count - i));
            break;
        }
    }
}

#undef FD_ISSET

/* License: Ruby's */
int
rb_w32_fdisset(int fd, fd_set *set)
{
    int ret;
    SOCKET s = TO_SOCKET(fd);
    if (s == (SOCKET)INVALID_HANDLE_VALUE)
        return 0;
    RUBY_CRITICAL(ret = __WSAFDIsSet(s, set));
    return ret;
}

/* License: Ruby's */
void
rb_w32_fd_copy(rb_fdset_t *dst, const fd_set *src, int max)
{
    max = min(src->fd_count, (UINT)max);
    if ((UINT)dst->capa < (UINT)max) {
	dst->capa = (src->fd_count / FD_SETSIZE + 1) * FD_SETSIZE;
	dst->fdset = xrealloc(dst->fdset, sizeof(unsigned int) + sizeof(SOCKET) * dst->capa);
    }

    memcpy(dst->fdset->fd_array, src->fd_array,
	   max * sizeof(src->fd_array[0]));
    dst->fdset->fd_count = src->fd_count;
}

/* License: Ruby's */
void
rb_w32_fd_dup(rb_fdset_t *dst, const rb_fdset_t *src)
{
    if ((UINT)dst->capa < src->fdset->fd_count) {
	dst->capa = (src->fdset->fd_count / FD_SETSIZE + 1) * FD_SETSIZE;
	dst->fdset = xrealloc(dst->fdset, sizeof(unsigned int) + sizeof(SOCKET) * dst->capa);
    }

    memcpy(dst->fdset->fd_array, src->fdset->fd_array,
	   src->fdset->fd_count * sizeof(src->fdset->fd_array[0]));
    dst->fdset->fd_count = src->fdset->fd_count;
}

//
// Networking trampolines
// These are used to avoid socket startup/shutdown overhead in case
// the socket routines aren't used.
//

#undef select

/* License: Ruby's */
static int
extract_fd(rb_fdset_t *dst, fd_set *src, int (*func)(SOCKET))
{
    unsigned int s = 0;
    unsigned int m = 0;
    if (!src) return 0;

    while (s < src->fd_count) {
        SOCKET fd = src->fd_array[s];

	if (!func || (*func)(fd)) {
	    if (dst) { /* move it to dst */
		unsigned int d;

		for (d = 0; d < dst->fdset->fd_count; d++) {
		    if (dst->fdset->fd_array[d] == fd)
			break;
		}
		if (d == dst->fdset->fd_count) {
		    if ((int)dst->fdset->fd_count >= dst->capa) {
			dst->capa = (dst->fdset->fd_count / FD_SETSIZE + 1) * FD_SETSIZE;
			dst->fdset = xrealloc(dst->fdset, sizeof(unsigned int) + sizeof(SOCKET) * dst->capa);
		    }
		    dst->fdset->fd_array[dst->fdset->fd_count++] = fd;
		}
		memmove(
		    &src->fd_array[s],
		    &src->fd_array[s+1],
		    sizeof(src->fd_array[0]) * (--src->fd_count - s));
	    }
	    else {
		m++;
		s++;
	    }
	}
	else s++;
    }

    return dst ? dst->fdset->fd_count : m;
}

/* License: Ruby's */
static int
copy_fd(fd_set *dst, fd_set *src)
{
    unsigned int s;
    if (!src || !dst) return 0;

    for (s = 0; s < src->fd_count; ++s) {
	SOCKET fd = src->fd_array[s];
	unsigned int d;
	for (d = 0; d < dst->fd_count; ++d) {
	    if (dst->fd_array[d] == fd)
		break;
	}
	if (d == dst->fd_count && d < FD_SETSIZE) {
	    dst->fd_array[dst->fd_count++] = fd;
	}
    }

    return dst->fd_count;
}

/* License: Ruby's */
static int
is_not_socket(SOCKET sock)
{
    return !is_socket(sock);
}

/* License: Ruby's */
static int
is_pipe(SOCKET sock) /* DONT call this for SOCKET! it clains it is PIPE. */
{
    int ret;

    RUBY_CRITICAL({
	ret = (GetFileType((HANDLE)sock) == FILE_TYPE_PIPE);
    });

    return ret;
}

/* License: Ruby's */
static int
is_readable_pipe(SOCKET sock) /* call this for pipe only */
{
    int ret;
    DWORD n = 0;

    RUBY_CRITICAL(
	if (PeekNamedPipe((HANDLE)sock, NULL, 0, NULL, &n, NULL)) {
	    ret = (n > 0);
	}
	else {
	    ret = (GetLastError() == ERROR_BROKEN_PIPE); /* pipe was closed */
	}
    );

    return ret;
}

/* License: Ruby's */
static int
is_console(SOCKET sock) /* DONT call this for SOCKET! */
{
    int ret;
    DWORD n = 0;
    INPUT_RECORD ir;

    RUBY_CRITICAL(
	ret = (PeekConsoleInput((HANDLE)sock, &ir, 1, &n))
    );

    return ret;
}

/* License: Ruby's */
static int
is_readable_console(SOCKET sock) /* call this for console only */
{
    int ret = 0;
    DWORD n = 0;
    INPUT_RECORD ir;

    RUBY_CRITICAL(
	if (PeekConsoleInput((HANDLE)sock, &ir, 1, &n) && n > 0) {
	    if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown &&
		ir.Event.KeyEvent.uChar.AsciiChar) {
		ret = 1;
	    }
	    else {
		ReadConsoleInput((HANDLE)sock, &ir, 1, &n);
	    }
	}
    );

    return ret;
}

/* License: Ruby's */
static int
is_invalid_handle(SOCKET sock)
{
    return (HANDLE)sock == INVALID_HANDLE_VALUE;
}

/* License: Artistic or GPL */
static int
do_select(int nfds, fd_set *rd, fd_set *wr, fd_set *ex,
            struct timeval *timeout)
{
    int r = 0;

    if (nfds == 0) {
	if (timeout)
	    rb_w32_sleep(timeout->tv_sec * 1000 + timeout->tv_usec / 1000);
	else
	    rb_w32_sleep(INFINITE);
    }
    else {
	if (!NtSocketsInitialized)
	    StartSockets();

	RUBY_CRITICAL(
	    EnterCriticalSection(&select_mutex);
	    r = select(nfds, rd, wr, ex, timeout);
	    LeaveCriticalSection(&select_mutex);
	    if (r == SOCKET_ERROR) {
		errno = map_errno(WSAGetLastError());
		r = -1;
	    }
	);
    }

    return r;
}

/*
 * rest -= wait
 * return 0 if rest is smaller than wait.
 */
/* License: Ruby's */
int
rb_w32_time_subtract(struct timeval *rest, const struct timeval *wait)
{
    if (rest->tv_sec < wait->tv_sec) {
	return 0;
    }
    while (rest->tv_usec < wait->tv_usec) {
	if (rest->tv_sec <= wait->tv_sec) {
	    return 0;
	}
	rest->tv_sec -= 1;
	rest->tv_usec += 1000 * 1000;
    }
    rest->tv_sec -= wait->tv_sec;
    rest->tv_usec -= wait->tv_usec;
    return rest->tv_sec != 0 || rest->tv_usec != 0;
}

/* License: Ruby's */
static inline int
compare(const struct timeval *t1, const struct timeval *t2)
{
    if (t1->tv_sec < t2->tv_sec)
	return -1;
    if (t1->tv_sec > t2->tv_sec)
	return 1;
    if (t1->tv_usec < t2->tv_usec)
	return -1;
    if (t1->tv_usec > t2->tv_usec)
	return 1;
    return 0;
}

#undef Sleep

int rb_w32_check_interrupt(void *);	/* @internal */

/* @internal */
/* License: Ruby's */
int
rb_w32_select_with_thread(int nfds, fd_set *rd, fd_set *wr, fd_set *ex,
			  struct timeval *timeout, void *th)
{
    int r;
    rb_fdset_t pipe_rd;
    rb_fdset_t cons_rd;
    rb_fdset_t else_rd;
    rb_fdset_t else_wr;
    rb_fdset_t except;
    int nonsock = 0;
    struct timeval limit = {0, 0};

    if (nfds < 0 || (timeout && (timeout->tv_sec < 0 || timeout->tv_usec < 0))) {
	errno = EINVAL;
	return -1;
    }

    if (timeout) {
	if (timeout->tv_sec < 0 ||
	    timeout->tv_usec < 0 ||
	    timeout->tv_usec >= 1000000) {
	    errno = EINVAL;
	    return -1;
	}
	gettimeofday(&limit, NULL);
	limit.tv_sec += timeout->tv_sec;
	limit.tv_usec += timeout->tv_usec;
	if (limit.tv_usec >= 1000000) {
	    limit.tv_usec -= 1000000;
	    limit.tv_sec++;
	}
    }

    // assume else_{rd,wr} (other than socket, pipe reader, console reader)
    // are always readable/writable. but this implementation still has
    // problem. if pipe's buffer is full, writing to pipe will block
    // until some data is read from pipe. but ruby is single threaded system,
    // so whole system will be blocked forever.

    rb_fd_init(&else_rd);
    nonsock += extract_fd(&else_rd, rd, is_not_socket);

    rb_fd_init(&else_wr);
    nonsock += extract_fd(&else_wr, wr, is_not_socket);

    // check invalid handles
    if (extract_fd(NULL, else_rd.fdset, is_invalid_handle) > 0 ||
	extract_fd(NULL, else_wr.fdset, is_invalid_handle) > 0) {
	rb_fd_term(&else_wr);
	rb_fd_term(&else_rd);
	errno = EBADF;
	return -1;
    }

    rb_fd_init(&pipe_rd);
    extract_fd(&pipe_rd, else_rd.fdset, is_pipe); // should not call is_pipe for socket

    rb_fd_init(&cons_rd);
    extract_fd(&cons_rd, else_rd.fdset, is_console); // ditto

    rb_fd_init(&except);
    extract_fd(&except, ex, is_not_socket); // drop only

    r = 0;
    if (rd && (int)rd->fd_count > r) r = (int)rd->fd_count;
    if (wr && (int)wr->fd_count > r) r = (int)wr->fd_count;
    if (ex && (int)ex->fd_count > r) r = (int)ex->fd_count;
    if (nfds > r) nfds = r;

    {
	struct timeval rest;
	struct timeval wait;
	struct timeval zero;
	wait.tv_sec = 0; wait.tv_usec = 10 * 1000; // 10ms
	zero.tv_sec = 0; zero.tv_usec = 0;         //  0ms
	for (;;) {
	    if (th && rb_w32_check_interrupt(th) != WAIT_TIMEOUT) {
		r = -1;
		break;
	    }
	    if (nonsock) {
		// modifying {else,pipe,cons}_rd is safe because
		// if they are modified, function returns immediately.
		extract_fd(&else_rd, pipe_rd.fdset, is_readable_pipe);
		extract_fd(&else_rd, cons_rd.fdset, is_readable_console);
	    }

	    if (else_rd.fdset->fd_count || else_wr.fdset->fd_count) {
		r = do_select(nfds, rd, wr, ex, &zero); // polling
		if (r < 0) break; // XXX: should I ignore error and return signaled handles?
		r += copy_fd(rd, else_rd.fdset);
		r += copy_fd(wr, else_wr.fdset);
		if (ex)
		    r += ex->fd_count;
		break;
	    }
	    else {
		struct timeval *dowait = &wait;

		fd_set orig_rd;
		fd_set orig_wr;
		fd_set orig_ex;
		if (rd) orig_rd = *rd;
		if (wr) orig_wr = *wr;
		if (ex) orig_ex = *ex;
		r = do_select(nfds, rd, wr, ex, &zero);	// polling
		if (r != 0) break; // signaled or error
		if (rd) *rd = orig_rd;
		if (wr) *wr = orig_wr;
		if (ex) *ex = orig_ex;

		if (timeout) {
		    struct timeval now;
		    gettimeofday(&now, NULL);
		    rest = limit;
		    if (!rb_w32_time_subtract(&rest, &now)) break;
		    if (compare(&rest, &wait) < 0) dowait = &rest;
		}
		Sleep(dowait->tv_sec * 1000 + dowait->tv_usec / 1000);
	    }
	}
    }

    rb_fd_term(&except);
    rb_fd_term(&cons_rd);
    rb_fd_term(&pipe_rd);
    rb_fd_term(&else_wr);
    rb_fd_term(&else_rd);

    return r;
}

/* License: Ruby's */
int WSAAPI
rb_w32_select(int nfds, fd_set *rd, fd_set *wr, fd_set *ex,
	      struct timeval *timeout)
{
    return rb_w32_select_with_thread(nfds, rd, wr, ex, timeout, 0);
}

/* License: Ruby's */
static FARPROC
get_wsa_extension_function(SOCKET s, GUID *guid)
{
    DWORD dmy;
    FARPROC ptr = NULL;

    WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER, guid, sizeof(*guid),
	     &ptr, sizeof(ptr), &dmy, NULL, NULL);
    if (!ptr)
	errno = ENOSYS;
    return ptr;
}

#undef accept

/* License: Artistic or GPL */
int WSAAPI
rb_w32_accept(int s, struct sockaddr *addr, int *addrlen)
{
    SOCKET r;
    int fd;

    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	HANDLE h = CreateFile("NUL", 0, 0, NULL, OPEN_ALWAYS, 0, NULL);
	fd = rb_w32_open_osfhandle((intptr_t)h, O_RDWR|O_BINARY|O_NOINHERIT);
	if (fd != -1) {
	    r = accept(TO_SOCKET(s), addr, addrlen);
	    if (r != INVALID_SOCKET) {
		MTHREAD_ONLY(EnterCriticalSection(&(_pioinfo(fd)->lock)));
		_set_osfhnd(fd, r);
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		CloseHandle(h);
		socklist_insert(r, 0);
	    }
	    else {
		errno = map_errno(WSAGetLastError());
		close(fd);
		fd = -1;
	    }
	}
	else
	    CloseHandle(h);
    });
    return fd;
}

#undef bind

/* License: Artistic or GPL */
int WSAAPI
rb_w32_bind(int s, const struct sockaddr *addr, int addrlen)
{
    int r;

    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = bind(TO_SOCKET(s), addr, addrlen);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef connect

/* License: Artistic or GPL */
int WSAAPI
rb_w32_connect(int s, const struct sockaddr *addr, int addrlen)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = connect(TO_SOCKET(s), addr, addrlen);
	if (r == SOCKET_ERROR) {
	    int err = WSAGetLastError();
	    if (err != WSAEWOULDBLOCK)
		errno = map_errno(err);
	    else
		errno = EINPROGRESS;
	}
    });
    return r;
}


#undef getpeername

/* License: Artistic or GPL */
int WSAAPI
rb_w32_getpeername(int s, struct sockaddr *addr, int *addrlen)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = getpeername(TO_SOCKET(s), addr, addrlen);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef getsockname

/* License: Artistic or GPL */
int WSAAPI
rb_w32_getsockname(int s, struct sockaddr *addr, int *addrlen)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = getsockname(TO_SOCKET(s), addr, addrlen);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

/* License: Artistic or GPL */
int WSAAPI
rb_w32_getsockopt(int s, int level, int optname, char *optval, int *optlen)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = getsockopt(TO_SOCKET(s), level, optname, optval, optlen);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef ioctlsocket

/* License: Artistic or GPL */
int WSAAPI
rb_w32_ioctlsocket(int s, long cmd, u_long *argp)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = ioctlsocket(TO_SOCKET(s), cmd, argp);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef listen

/* License: Artistic or GPL */
int WSAAPI
rb_w32_listen(int s, int backlog)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = listen(TO_SOCKET(s), backlog);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef recv
#undef recvfrom
#undef send
#undef sendto

/* License: Ruby's */
static int
finish_overlapped_socket(SOCKET s, WSAOVERLAPPED *wol, int result, DWORD *len, DWORD size)
{
    DWORD flg;
    int err;

    if (result != SOCKET_ERROR)
	*len = size;
    else if ((err = WSAGetLastError()) == WSA_IO_PENDING) {
	switch (rb_w32_wait_events_blocking(&wol->hEvent, 1, INFINITE)) {
	  case WAIT_OBJECT_0:
	    RUBY_CRITICAL(
		result = WSAGetOverlappedResult(s, wol, &size, TRUE, &flg)
		);
	    if (result) {
		*len = size;
		break;
	    }
	    /* thru */
	  default:
	    errno = map_errno(WSAGetLastError());
	    /* thru */
	  case WAIT_OBJECT_0 + 1:
	    /* interrupted */
	    *len = -1;
	    cancel_io((HANDLE)s);
	    break;
	}
    }
    else {
	errno = map_errno(err);
	*len = -1;
    }
    CloseHandle(wol->hEvent);

    return result;
}

/* License: Artistic or GPL */
static int
overlapped_socket_io(BOOL input, int fd, char *buf, int len, int flags,
		     struct sockaddr *addr, int *addrlen)
{
    int r;
    int ret;
    int mode = 0;
    DWORD flg;
    WSAOVERLAPPED wol;
    WSABUF wbuf;
    SOCKET s;

    if (!NtSocketsInitialized)
	StartSockets();

    s = TO_SOCKET(fd);
    socklist_lookup(s, &mode);
    if (!cancel_io || (mode & O_NONBLOCK)) {
	RUBY_CRITICAL({
	    if (input) {
		if (addr && addrlen)
		    r = recvfrom(s, buf, len, flags, addr, addrlen);
		else
		    r = recv(s, buf, len, flags);
	    }
	    else {
		if (addr && addrlen)
		    r = sendto(s, buf, len, flags, addr, *addrlen);
		else
		    r = send(s, buf, len, flags);
	    }
	    if (r == SOCKET_ERROR)
		errno = map_errno(WSAGetLastError());
	});
    }
    else {
	DWORD size;
	DWORD rlen;
	wbuf.len = len;
	wbuf.buf = buf;
	memset(&wol, 0, sizeof(wol));
	RUBY_CRITICAL({
	    wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	    if (input) {
		flg = flags;
		if (addr && addrlen)
		    ret = WSARecvFrom(s, &wbuf, 1, &size, &flg, addr, addrlen,
				      &wol, NULL);
		else
		    ret = WSARecv(s, &wbuf, 1, &size, &flg, &wol, NULL);
	    }
	    else {
		if (addr && addrlen)
		    ret = WSASendTo(s, &wbuf, 1, &size, flags, addr, *addrlen,
				    &wol, NULL);
		else
		    ret = WSASend(s, &wbuf, 1, &size, flags, &wol, NULL);
	    }
	});

	finish_overlapped_socket(s, &wol, ret, &rlen, size);
	r = (int)rlen;
    }

    return r;
}

/* License: Ruby's */
int WSAAPI
rb_w32_recv(int fd, char *buf, int len, int flags)
{
    return overlapped_socket_io(TRUE, fd, buf, len, flags, NULL, NULL);
}

/* License: Ruby's */
int WSAAPI
rb_w32_recvfrom(int fd, char *buf, int len, int flags,
		struct sockaddr *from, int *fromlen)
{
    return overlapped_socket_io(TRUE, fd, buf, len, flags, from, fromlen);
}

/* License: Ruby's */
int WSAAPI
rb_w32_send(int fd, const char *buf, int len, int flags)
{
    return overlapped_socket_io(FALSE, fd, (char *)buf, len, flags, NULL, NULL);
}

/* License: Ruby's */
int WSAAPI
rb_w32_sendto(int fd, const char *buf, int len, int flags,
	      const struct sockaddr *to, int tolen)
{
    return overlapped_socket_io(FALSE, fd, (char *)buf, len, flags,
				(struct sockaddr *)to, &tolen);
}

#if !defined(MSG_TRUNC) && !defined(__MINGW32__)
/* License: Ruby's */
typedef struct {
    SOCKADDR *name;
    int namelen;
    WSABUF *lpBuffers;
    DWORD dwBufferCount;
    WSABUF Control;
    DWORD dwFlags;
} WSAMSG;
#endif
#ifndef WSAID_WSARECVMSG
#define WSAID_WSARECVMSG {0xf689d7c8,0x6f1f,0x436b,{0x8a,0x53,0xe5,0x4f,0xe3,0x51,0xc3,0x22}}
#endif
#ifndef WSAID_WSASENDMSG
#define WSAID_WSASENDMSG {0xa441e712,0x754f,0x43ca,{0x84,0xa7,0x0d,0xee,0x44,0xcf,0x60,0x6d}}
#endif

/* License: Ruby's */
#define msghdr_to_wsamsg(msg, wsamsg) \
    do { \
	int i; \
	(wsamsg)->name = (msg)->msg_name; \
	(wsamsg)->namelen = (msg)->msg_namelen; \
	(wsamsg)->lpBuffers = ALLOCA_N(WSABUF, (msg)->msg_iovlen); \
	(wsamsg)->dwBufferCount = (msg)->msg_iovlen; \
	for (i = 0; i < (msg)->msg_iovlen; ++i) { \
	    (wsamsg)->lpBuffers[i].buf = (msg)->msg_iov[i].iov_base; \
	    (wsamsg)->lpBuffers[i].len = (msg)->msg_iov[i].iov_len; \
	} \
	(wsamsg)->Control.buf = (msg)->msg_control; \
	(wsamsg)->Control.len = (msg)->msg_controllen; \
	(wsamsg)->dwFlags = (msg)->msg_flags; \
    } while (0)

/* License: Ruby's */
int
recvmsg(int fd, struct msghdr *msg, int flags)
{
    typedef int (WSAAPI *WSARecvMsg_t)(SOCKET, WSAMSG *, DWORD *, WSAOVERLAPPED *, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
    static WSARecvMsg_t pWSARecvMsg = NULL;
    WSAMSG wsamsg;
    SOCKET s;
    int mode = 0;
    DWORD len;
    int ret;

    if (!NtSocketsInitialized)
	StartSockets();

    s = TO_SOCKET(fd);

    if (!pWSARecvMsg) {
	static GUID guid = WSAID_WSARECVMSG;
	pWSARecvMsg = (WSARecvMsg_t)get_wsa_extension_function(s, &guid);
	if (!pWSARecvMsg)
	    return -1;
    }

    msghdr_to_wsamsg(msg, &wsamsg);
    wsamsg.dwFlags |= flags;

    socklist_lookup(s, &mode);
    if (!cancel_io || (mode & O_NONBLOCK)) {
	RUBY_CRITICAL({
	    if ((ret = pWSARecvMsg(s, &wsamsg, &len, NULL, NULL)) == SOCKET_ERROR) {
		errno = map_errno(WSAGetLastError());
		len = -1;
	    }
	});
    }
    else {
	DWORD size;
	WSAOVERLAPPED wol;
	memset(&wol, 0, sizeof(wol));
	RUBY_CRITICAL({
	    wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	    ret = pWSARecvMsg(s, &wsamsg, &size, &wol, NULL);
	});

	ret = finish_overlapped_socket(s, &wol, ret, &len, size);
    }
    if (ret == SOCKET_ERROR)
	return -1;

    /* WSAMSG to msghdr */
    msg->msg_name = wsamsg.name;
    msg->msg_namelen = wsamsg.namelen;
    msg->msg_flags = wsamsg.dwFlags;

    return len;
}

/* License: Ruby's */
int
sendmsg(int fd, const struct msghdr *msg, int flags)
{
    typedef int (WSAAPI *WSASendMsg_t)(SOCKET, const WSAMSG *, DWORD, DWORD *, WSAOVERLAPPED *, LPWSAOVERLAPPED_COMPLETION_ROUTINE);
    static WSASendMsg_t pWSASendMsg = NULL;
    WSAMSG wsamsg;
    SOCKET s;
    int mode = 0;
    DWORD len;
    int ret;

    if (!NtSocketsInitialized)
	StartSockets();

    s = TO_SOCKET(fd);

    if (!pWSASendMsg) {
	static GUID guid = WSAID_WSASENDMSG;
	pWSASendMsg = (WSASendMsg_t)get_wsa_extension_function(s, &guid);
	if (!pWSASendMsg)
	    return -1;
    }

    msghdr_to_wsamsg(msg, &wsamsg);

    socklist_lookup(s, &mode);
    if (!cancel_io || (mode & O_NONBLOCK)) {
	RUBY_CRITICAL({
	    if ((ret = pWSASendMsg(s, &wsamsg, flags, &len, NULL, NULL)) == SOCKET_ERROR) {
		errno = map_errno(WSAGetLastError());
		len = -1;
	    }
	});
    }
    else {
	DWORD size;
	WSAOVERLAPPED wol;
	memset(&wol, 0, sizeof(wol));
	RUBY_CRITICAL({
	    wol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	    ret = pWSASendMsg(s, &wsamsg, flags, &size, &wol, NULL);
	});

	finish_overlapped_socket(s, &wol, ret, &len, size);
    }

    return len;
}

#undef setsockopt

/* License: Artistic or GPL */
int WSAAPI
rb_w32_setsockopt(int s, int level, int optname, const char *optval, int optlen)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = setsockopt(TO_SOCKET(s), level, optname, optval, optlen);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef shutdown

/* License: Artistic or GPL */
int WSAAPI
rb_w32_shutdown(int s, int how)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = shutdown(TO_SOCKET(s), how);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

/* License: Ruby's */
static SOCKET
open_ifs_socket(int af, int type, int protocol)
{
    unsigned long proto_buffers_len = 0;
    int error_code;
    SOCKET out = INVALID_SOCKET;

    if (WSAEnumProtocols(NULL, NULL, &proto_buffers_len) == SOCKET_ERROR) {
	error_code = WSAGetLastError();
	if (error_code == WSAENOBUFS) {
	    WSAPROTOCOL_INFO *proto_buffers;
	    int protocols_available = 0;

	    proto_buffers = (WSAPROTOCOL_INFO *)malloc(proto_buffers_len);
	    if (!proto_buffers) {
		WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
		return INVALID_SOCKET;
	    }

	    protocols_available =
		WSAEnumProtocols(NULL, proto_buffers, &proto_buffers_len);
	    if (protocols_available != SOCKET_ERROR) {
		int i;
		for (i = 0; i < protocols_available; i++) {
		    if ((af != AF_UNSPEC && af != proto_buffers[i].iAddressFamily) ||
			(type != proto_buffers[i].iSocketType) ||
			(protocol != 0 && protocol != proto_buffers[i].iProtocol))
			continue;

		    if ((proto_buffers[i].dwServiceFlags1 & XP1_IFS_HANDLES) == 0)
			continue;

		    out = WSASocket(af, type, protocol, &(proto_buffers[i]), 0,
				    WSA_FLAG_OVERLAPPED);
		    break;
		}
		if (out == INVALID_SOCKET)
		    out = WSASocket(af, type, protocol, NULL, 0, 0);
	    }

	    free(proto_buffers);
	}
    }

    return out;
}

#undef socket

/* License: Artistic or GPL */
int WSAAPI
rb_w32_socket(int af, int type, int protocol)
{
    SOCKET s;
    int fd;

    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	s = open_ifs_socket(af, type, protocol);
	if (s == INVALID_SOCKET) {
	    errno = map_errno(WSAGetLastError());
	    fd = -1;
	}
	else {
	    fd = rb_w32_open_osfhandle(s, O_RDWR|O_BINARY|O_NOINHERIT);
	    if (fd != -1)
		socklist_insert(s, 0);
	    else
		closesocket(s);
	}
    });
    return fd;
}

#undef gethostbyaddr

/* License: Artistic or GPL */
struct hostent * WSAAPI
rb_w32_gethostbyaddr(const char *addr, int len, int type)
{
    struct hostent *r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = gethostbyaddr(addr, len, type);
	if (r == NULL)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef gethostbyname

/* License: Artistic or GPL */
struct hostent * WSAAPI
rb_w32_gethostbyname(const char *name)
{
    struct hostent *r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = gethostbyname(name);
	if (r == NULL)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef gethostname

/* License: Artistic or GPL */
int WSAAPI
rb_w32_gethostname(char *name, int len)
{
    int r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = gethostname(name, len);
	if (r == SOCKET_ERROR)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef getprotobyname

/* License: Artistic or GPL */
struct protoent * WSAAPI
rb_w32_getprotobyname(const char *name)
{
    struct protoent *r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = getprotobyname(name);
	if (r == NULL)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef getprotobynumber

/* License: Artistic or GPL */
struct protoent * WSAAPI
rb_w32_getprotobynumber(int num)
{
    struct protoent *r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = getprotobynumber(num);
	if (r == NULL)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef getservbyname

/* License: Artistic or GPL */
struct servent * WSAAPI
rb_w32_getservbyname(const char *name, const char *proto)
{
    struct servent *r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = getservbyname(name, proto);
	if (r == NULL)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

#undef getservbyport

/* License: Artistic or GPL */
struct servent * WSAAPI
rb_w32_getservbyport(int port, const char *proto)
{
    struct servent *r;
    if (!NtSocketsInitialized) {
	StartSockets();
    }
    RUBY_CRITICAL({
	r = getservbyport(port, proto);
	if (r == NULL)
	    errno = map_errno(WSAGetLastError());
    });
    return r;
}

/* License: Ruby's */
static int
socketpair_internal(int af, int type, int protocol, SOCKET *sv)
{
    SOCKET svr = INVALID_SOCKET, r = INVALID_SOCKET, w = INVALID_SOCKET;
    struct sockaddr_in sock_in4;
#ifdef INET6
    struct sockaddr_in6 sock_in6;
#endif
    struct sockaddr *addr;
    int ret = -1;
    int len;

    if (!NtSocketsInitialized) {
	StartSockets();
    }

    switch (af) {
      case AF_INET:
#if defined PF_INET && PF_INET != AF_INET
      case PF_INET:
#endif
	sock_in4.sin_family = AF_INET;
	sock_in4.sin_port = 0;
	sock_in4.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr = (struct sockaddr *)&sock_in4;
	len = sizeof(sock_in4);
	break;
#ifdef INET6
      case AF_INET6:
	memset(&sock_in6, 0, sizeof(sock_in6));
	sock_in6.sin6_family = AF_INET6;
	sock_in6.sin6_addr = IN6ADDR_LOOPBACK_INIT;
	addr = (struct sockaddr *)&sock_in6;
	len = sizeof(sock_in6);
	break;
#endif
      default:
	errno = EAFNOSUPPORT;
	return -1;
    }
    if (type != SOCK_STREAM) {
	errno = EPROTOTYPE;
	return -1;
    }

    sv[0] = (SOCKET)INVALID_HANDLE_VALUE;
    sv[1] = (SOCKET)INVALID_HANDLE_VALUE;
    RUBY_CRITICAL({
	do {
	    svr = open_ifs_socket(af, type, protocol);
	    if (svr == INVALID_SOCKET)
		break;
	    if (bind(svr, addr, len) < 0)
		break;
	    if (getsockname(svr, addr, &len) < 0)
		break;
	    if (type == SOCK_STREAM)
		listen(svr, 5);

	    w = open_ifs_socket(af, type, protocol);
	    if (w == INVALID_SOCKET)
		break;
	    if (connect(w, addr, len) < 0)
		break;

	    r = accept(svr, addr, &len);
	    if (r == INVALID_SOCKET)
		break;

	    ret = 0;
	} while (0);

	if (ret < 0) {
	    errno = map_errno(WSAGetLastError());
	    if (r != INVALID_SOCKET)
		closesocket(r);
	    if (w != INVALID_SOCKET)
		closesocket(w);
	}
	else {
	    sv[0] = r;
	    sv[1] = w;
	}
	if (svr != INVALID_SOCKET)
	    closesocket(svr);
    });

    return ret;
}

/* License: Ruby's */
int
rb_w32_socketpair(int af, int type, int protocol, int *sv)
{
    SOCKET pair[2];

    if (socketpair_internal(af, type, protocol, pair) < 0)
	return -1;
    sv[0] = rb_w32_open_osfhandle(pair[0], O_RDWR|O_BINARY|O_NOINHERIT);
    if (sv[0] == -1) {
	closesocket(pair[0]);
	closesocket(pair[1]);
	return -1;
    }
    sv[1] = rb_w32_open_osfhandle(pair[1], O_RDWR|O_BINARY|O_NOINHERIT);
    if (sv[1] == -1) {
	rb_w32_close(sv[0]);
	closesocket(pair[1]);
	return -1;
    }
    socklist_insert(pair[0], 0);
    socklist_insert(pair[1], 0);

    return 0;
}

//
// Networking stubs
//

void endhostent(void) {}
void endnetent(void) {}
void endprotoent(void) {}
void endservent(void) {}

struct netent *getnetent (void) {return (struct netent *) NULL;}

struct netent *getnetbyaddr(long net, int type) {return (struct netent *)NULL;}

struct netent *getnetbyname(const char *name) {return (struct netent *)NULL;}

struct protoent *getprotoent (void) {return (struct protoent *) NULL;}

struct servent *getservent (void) {return (struct servent *) NULL;}

void sethostent (int stayopen) {}

void setnetent (int stayopen) {}

void setprotoent (int stayopen) {}

void setservent (int stayopen) {}

/* License: Ruby's */
static int
setfl(SOCKET sock, int arg)
{
    int ret;
    int flag = 0;
    u_long ioctlArg;

    socklist_lookup(sock, &flag);
    if (arg & O_NONBLOCK) {
	flag |= O_NONBLOCK;
	ioctlArg = 1;
    }
    else {
	flag &= ~O_NONBLOCK;
	ioctlArg = 0;
    }
    RUBY_CRITICAL({
	ret = ioctlsocket(sock, FIONBIO, &ioctlArg);
	if (ret == 0)
	    socklist_insert(sock, flag);
	else
	    errno = map_errno(WSAGetLastError());
    });

    return ret;
}

/* License: Ruby's */
static int
dupfd(HANDLE hDup, char flags, int minfd)
{
    int save_errno;
    int ret;
    int fds[32];
    int filled = 0;

    do {
	ret = _open_osfhandle((intptr_t)hDup, flags | FOPEN);
	if (ret == -1) {
	    goto close_fds_and_return;
	}
	if (ret >= minfd) {
	    goto close_fds_and_return;
	}
	fds[filled++] = ret;
    } while (filled < (int)numberof(fds));

    ret = dupfd(hDup, flags, minfd);

  close_fds_and_return:
    save_errno = errno;
    while (filled > 0) {
	int fd = fds[--filled];
	_osfhnd(fd) = (intptr_t)INVALID_HANDLE_VALUE;
	close(fd);
    }
    errno = save_errno;

    return ret;
}

/* License: Ruby's */
int
fcntl(int fd, int cmd, ...)
{
    va_list va;
    int arg;

    if (cmd == F_SETFL) {
	SOCKET sock = TO_SOCKET(fd);
	if (!is_socket(sock)) {
	    errno = EBADF;
	    return -1;
	}

	va_start(va, cmd);
	arg = va_arg(va, int);
	va_end(va);
	return setfl(sock, arg);
    }
    else if (cmd == F_DUPFD) {
	int ret;
	HANDLE hDup;
	if (!(DuplicateHandle(GetCurrentProcess(), (HANDLE)_get_osfhandle(fd),
			      GetCurrentProcess(), &hDup, 0L,
			      !(_osfile(fd) & FNOINHERIT),
			      DUPLICATE_SAME_ACCESS))) {
	    errno = map_errno(GetLastError());
	    return -1;
	}

	va_start(va, cmd);
	arg = va_arg(va, int);
	va_end(va);

	if ((ret = dupfd(hDup, _osfile(fd), arg)) == -1)
	    CloseHandle(hDup);
	return ret;
    }
    else {
	errno = EINVAL;
	return -1;
    }
}

#ifndef WNOHANG
#define WNOHANG -1
#endif

/* License: Ruby's */
static rb_pid_t
poll_child_status(struct ChildRecord *child, int *stat_loc)
{
    DWORD exitcode;
    DWORD err;

    if (!GetExitCodeProcess(child->hProcess, &exitcode)) {
	/* If an error occured, return immediatly. */
    error_exit:
	err = GetLastError();
	if (err == ERROR_INVALID_PARAMETER)
	    errno = ECHILD;
	else {
	    if (GetLastError() == ERROR_INVALID_HANDLE)
		errno = EINVAL;
	    else
		errno = map_errno(GetLastError());
	}
	CloseChildHandle(child);
	return -1;
    }
    if (exitcode != STILL_ACTIVE) {
        rb_pid_t pid;
	/* If already died, wait process's real termination. */
        if (rb_w32_wait_events_blocking(&child->hProcess, 1, INFINITE) != WAIT_OBJECT_0) {
	    goto error_exit;
        }
	pid = child->pid;
	CloseChildHandle(child);
	if (stat_loc) *stat_loc = exitcode << 8;
	return pid;
    }
    return 0;
}

/* License: Artistic or GPL */
rb_pid_t
waitpid(rb_pid_t pid, int *stat_loc, int options)
{
    DWORD timeout;

    /* Artistic or GPL part start */
    if (options == WNOHANG) {
	timeout = 0;
    }
    else {
	timeout = INFINITE;
    }
    /* Artistic or GPL part end */

    if (pid == -1) {
	int count = 0;
	int ret;
	HANDLE events[MAXCHILDNUM];

	FOREACH_CHILD(child) {
	    if (!child->pid || child->pid < 0) continue;
	    if ((pid = poll_child_status(child, stat_loc))) return pid;
	    events[count++] = child->hProcess;
	} END_FOREACH_CHILD;
	if (!count) {
	    errno = ECHILD;
	    return -1;
	}

	ret = rb_w32_wait_events_blocking(events, count, timeout);
	if (ret == WAIT_TIMEOUT) return 0;
	if ((ret -= WAIT_OBJECT_0) == count) {
	    return -1;
	}
	if (ret > count) {
	    errno = map_errno(GetLastError());
	    return -1;
	}

	return poll_child_status(FindChildSlotByHandle(events[ret]), stat_loc);
    }
    else {
	struct ChildRecord* child = FindChildSlot(pid);
	if (!child) {
	    errno = ECHILD;
	    return -1;
	}

	while (!(pid = poll_child_status(child, stat_loc))) {
	    /* wait... */
	    if (rb_w32_wait_events_blocking(&child->hProcess, 1, timeout) != WAIT_OBJECT_0) {
		/* still active */
		pid = 0;
		break;
	    }
	}
    }

    return pid;
}

#include <sys/timeb.h>

/* License: Ruby's */
static int
filetime_to_timeval(const FILETIME* ft, struct timeval *tv)
{
    ULARGE_INTEGER tmp;
    unsigned LONG_LONG lt;

    tmp.LowPart = ft->dwLowDateTime;
    tmp.HighPart = ft->dwHighDateTime;
    lt = tmp.QuadPart;

    /* lt is now 100-nanosec intervals since 1601/01/01 00:00:00 UTC,
       convert it into UNIX time (since 1970/01/01 00:00:00 UTC).
       the first leap second is at 1972/06/30, so we doesn't need to think
       about it. */
    lt /= 10;	/* to usec */
    lt -= (LONG_LONG)((1970-1601)*365.2425) * 24 * 60 * 60 * 1000 * 1000;

    tv->tv_sec = (long)(lt / (1000 * 1000));
    tv->tv_usec = (long)(lt % (1000 * 1000));

    return tv->tv_sec > 0 ? 0 : -1;
}

/* License: Ruby's */
int _cdecl
gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);
    filetime_to_timeval(&ft, tv);

    return 0;
}

/* License: Ruby's */
char *
rb_w32_getcwd(char *buffer, int size)
{
    char *p = buffer;
    int len;

    len = GetCurrentDirectory(0, NULL);
    if (!len) {
	errno = map_errno(GetLastError());
	return NULL;
    }

    if (p) {
	if (size < len) {
	    errno = ERANGE;
	    return NULL;
	}
    }
    else {
	p = malloc(len);
	size = len;
	if (!p) {
	    errno = ENOMEM;
	    return NULL;
	}
    }

    if (!GetCurrentDirectory(size, p)) {
	errno = map_errno(GetLastError());
	if (!buffer)
	    free(p);
        return NULL;
    }

    translate_char(p, '\\', '/');

    return p;
}

/* License: Artistic or GPL */
int
chown(const char *path, int owner, int group)
{
    return 0;
}

/* License: Artistic or GPL */
int
rb_w32_uchown(const char *path, int owner, int group)
{
    return 0;
}

/* License: Ruby's */
int
kill(int pid, int sig)
{
    int ret = 0;
    DWORD err;

    if (pid < 0 || pid == 0 && sig != SIGINT) {
	errno = EINVAL;
	return -1;
    }

    (void)IfWin95(pid = -pid, 0);
    if ((unsigned int)pid == GetCurrentProcessId() &&
	(sig != 0 && sig != SIGKILL)) {
	if ((ret = raise(sig)) != 0) {
	    /* MSVCRT doesn't set errno... */
	    errno = EINVAL;
	}
	return ret;
    }

    switch (sig) {
      case 0:
	RUBY_CRITICAL({
	    HANDLE hProc =
		OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
	    if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_INVALID_PARAMETER) {
		    errno = ESRCH;
		}
		else {
		    errno = EPERM;
		}
		ret = -1;
	    }
	    else {
		CloseHandle(hProc);
	    }
	});
	break;

      case SIGINT:
	RUBY_CRITICAL({
	    DWORD ctrlEvent = CTRL_C_EVENT;
	    if (pid != 0) {
	        /* CTRL+C signal cannot be generated for process groups.
		 * Instead, we use CTRL+BREAK signal. */
	        ctrlEvent = CTRL_BREAK_EVENT;
	    }
	    if (!GenerateConsoleCtrlEvent(ctrlEvent, (DWORD)pid)) {
		if ((err = GetLastError()) == 0)
		    errno = EPERM;
		else
		    errno = map_errno(GetLastError());
		ret = -1;
	    }
	});
	break;

      case SIGKILL:
	RUBY_CRITICAL({
	    HANDLE hProc;
	    struct ChildRecord* child = FindChildSlot(pid);
	    if (child) {
		hProc = child->hProcess;
	    }
	    else {
		hProc = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
	    }
	    if (hProc == NULL || hProc == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_INVALID_PARAMETER) {
		    errno = ESRCH;
		}
		else {
		    errno = EPERM;
		}
		ret = -1;
	    }
	    else {
		DWORD status;
		if (!GetExitCodeProcess(hProc, &status)) {
		    errno = map_errno(GetLastError());
		    ret = -1;
		}
		else if (status == STILL_ACTIVE) {
		    if (!TerminateProcess(hProc, 0)) {
			errno = EPERM;
			ret = -1;
		    }
		}
		else {
		    errno = ESRCH;
		    ret = -1;
		}
		if (!child) {
		    CloseHandle(hProc);
		}
	    }
	});
	break;

      default:
	errno = EINVAL;
	ret = -1;
	break;
    }

    return ret;
}

/* License: Ruby's */
static int
wlink(const WCHAR *from, const WCHAR *to)
{
    typedef BOOL (WINAPI link_func)(LPCWSTR, LPCWSTR, LPSECURITY_ATTRIBUTES);
    static link_func *pCreateHardLinkW = NULL;
    static int myerrno = 0;

    if (!pCreateHardLinkW && !myerrno) {
	pCreateHardLinkW = (link_func *)get_proc_address("kernel32", "CreateHardLinkW", NULL);
	if (!pCreateHardLinkW)
	    myerrno = ENOSYS;
    }
    if (!pCreateHardLinkW) {
	errno = myerrno;
	return -1;
    }

    if (!pCreateHardLinkW(to, from, NULL)) {
	errno = map_errno(GetLastError());
	return -1;
    }

    return 0;
}

/* License: Ruby's */
int
rb_w32_ulink(const char *from, const char *to)
{
    WCHAR *wfrom;
    WCHAR *wto;
    int ret;

    if (!(wfrom = utf8_to_wstr(from, NULL)))
	return -1;
    if (!(wto = utf8_to_wstr(to, NULL))) {
	free(wfrom);
	return -1;
    }
    ret = wlink(wfrom, wto);
    free(wto);
    free(wfrom);
    return ret;
}

/* License: Ruby's */
int
link(const char *from, const char *to)
{
    WCHAR *wfrom;
    WCHAR *wto;
    int ret;

    if (!(wfrom = filecp_to_wstr(from, NULL)))
	return -1;
    if (!(wto = filecp_to_wstr(to, NULL))) {
	free(wfrom);
	return -1;
    }
    ret = wlink(wfrom, wto);
    free(wto);
    free(wfrom);
    return ret;
}

/* License: Ruby's */
int
wait(int *status)
{
    return waitpid(-1, status, 0);
}

/* License: Ruby's */
char *
rb_w32_ugetenv(const char *name)
{
    WCHAR *wenvarea, *wenv;
    int len = strlen(name);
    char *env;
    int wlen;

    if (len == 0) return NULL;

    if (uenvarea) {
	free(uenvarea);
	uenvarea = NULL;
    }
    if (envarea) {
	FreeEnvironmentStrings(envarea);
	envarea = NULL;
    }
    wenvarea = GetEnvironmentStringsW();
    if (!wenvarea) {
	map_errno(GetLastError());
	return NULL;
    }
    for (wenv = wenvarea, wlen = 1; *wenv; wenv += lstrlenW(wenv) + 1)
	wlen += lstrlenW(wenv) + 1;
    uenvarea = wstr_to_mbstr(CP_UTF8, wenvarea, wlen, NULL);
    FreeEnvironmentStringsW(wenvarea);
    if (!uenvarea)
	return NULL;

    for (env = uenvarea; *env; env += strlen(env) + 1)
	if (strncasecmp(env, name, len) == 0 && *(env + len) == '=')
	    return env + len + 1;

    return NULL;
}

/* License: Ruby's */
char *
rb_w32_getenv(const char *name)
{
    int len = strlen(name);
    char *env;

    if (len == 0) return NULL;
    if (uenvarea) {
	free(uenvarea);
	uenvarea = NULL;
    }
    if (envarea) {
	FreeEnvironmentStrings(envarea);
	envarea = NULL;
    }
    envarea = GetEnvironmentStrings();
    if (!envarea) {
	map_errno(GetLastError());
	return NULL;
    }

    for (env = envarea; *env; env += strlen(env) + 1)
	if (strncasecmp(env, name, len) == 0 && *(env + len) == '=')
	    return env + len + 1;

    return NULL;
}

/* License: Artistic or GPL */
static int
wrename(const WCHAR *oldpath, const WCHAR *newpath)
{
    int res = 0;
    int oldatts;
    int newatts;

    oldatts = GetFileAttributesW(oldpath);
    newatts = GetFileAttributesW(newpath);

    if (oldatts == -1) {
	errno = map_errno(GetLastError());
	return -1;
    }

    RUBY_CRITICAL({
	if (newatts != -1 && newatts & FILE_ATTRIBUTE_READONLY)
	    SetFileAttributesW(newpath, newatts & ~ FILE_ATTRIBUTE_READONLY);

	if (!MoveFileW(oldpath, newpath))
	    res = -1;

	if (res) {
	    switch (GetLastError()) {
	      case ERROR_ALREADY_EXISTS:
	      case ERROR_FILE_EXISTS:
		if (IsWinNT()) {
		    if (MoveFileExW(oldpath, newpath, MOVEFILE_REPLACE_EXISTING))
			res = 0;
		}
		else {
		    for (;;) {
			if (!DeleteFileW(newpath) && GetLastError() != ERROR_FILE_NOT_FOUND)
			    break;
			else if (MoveFileW(oldpath, newpath)) {
			    res = 0;
			    break;
			}
		    }
		}
	    }
	}

	if (res)
	    errno = map_errno(GetLastError());
	else
	    SetFileAttributesW(newpath, oldatts);
    });

    return res;
}

/* License: Ruby's */
int rb_w32_urename(const char *from, const char *to)
{
    WCHAR *wfrom;
    WCHAR *wto;
    int ret = -1;

    if (!(wfrom = utf8_to_wstr(from, NULL)))
	return -1;
    if (!(wto = utf8_to_wstr(to, NULL))) {
	free(wfrom);
	return -1;
    }
    ret = wrename(wfrom, wto);
    free(wto);
    free(wfrom);
    return ret;
}

/* License: Ruby's */
int rb_w32_rename(const char *from, const char *to)
{
    WCHAR *wfrom;
    WCHAR *wto;
    int ret = -1;

    if (!(wfrom = filecp_to_wstr(from, NULL)))
	return -1;
    if (!(wto = filecp_to_wstr(to, NULL))) {
	free(wfrom);
	return -1;
    }
    ret = wrename(wfrom, wto);
    free(wto);
    free(wfrom);
    return ret;
}

/* License: Ruby's */
static int
isUNCRoot(const WCHAR *path)
{
    if (path[0] == L'\\' && path[1] == L'\\') {
	const WCHAR *p;
	for (p = path + 2; *p; p++) {
	    if (*p == L'\\')
		break;
	}
	if (p[0] && p[1]) {
	    for (p++; *p; p++) {
		if (*p == L'\\')
		    break;
	    }
	    if (!p[0] || !p[1] || (p[1] == L'.' && !p[2]))
		return 1;
	}
    }
    return 0;
}

#define COPY_STAT(src, dest, size_cast) do {	\
	(dest).st_dev 	= (src).st_dev;		\
	(dest).st_ino 	= (src).st_ino;		\
	(dest).st_mode  = (src).st_mode;	\
	(dest).st_nlink = (src).st_nlink;	\
	(dest).st_uid   = (src).st_uid;		\
	(dest).st_gid   = (src).st_gid;		\
	(dest).st_rdev 	= (src).st_rdev;	\
	(dest).st_size 	= size_cast(src).st_size; \
	(dest).st_atime = (src).st_atime;	\
	(dest).st_mtime = (src).st_mtime;	\
	(dest).st_ctime = (src).st_ctime;	\
    } while (0)

#ifdef __BORLANDC__
#undef fstat
/* License: Ruby's */
int
rb_w32_fstat(int fd, struct stat *st)
{
    BY_HANDLE_FILE_INFORMATION info;
    int ret = fstat(fd, st);

    if (ret) return ret;
    st->st_mode &= ~(S_IWGRP | S_IWOTH);
    if (GetFileInformationByHandle((HANDLE)_get_osfhandle(fd), &info) &&
	!(info.dwFileAttributes & FILE_ATTRIBUTE_READONLY)) {
	st->st_mode |= S_IWUSR;
    }
    return ret;
}

/* License: Ruby's */
int
rb_w32_fstati64(int fd, struct stati64 *st)
{
    BY_HANDLE_FILE_INFORMATION info;
    struct stat tmp;
    int ret = fstat(fd, &tmp);

    if (ret) return ret;
    tmp.st_mode &= ~(S_IWGRP | S_IWOTH);
    COPY_STAT(tmp, *st, +);
    if (GetFileInformationByHandle((HANDLE)_get_osfhandle(fd), &info)) {
	if (!(info.dwFileAttributes & FILE_ATTRIBUTE_READONLY)) {
	    st->st_mode |= S_IWUSR;
	}
	st->st_size = ((__int64)info.nFileSizeHigh << 32) | info.nFileSizeLow;
    }
    return ret;
}
#endif

/* License: Ruby's */
static time_t
filetime_to_unixtime(const FILETIME *ft)
{
    struct timeval tv;

    if (filetime_to_timeval(ft, &tv) == (time_t)-1)
	return 0;
    else
	return tv.tv_sec;
}

/* License: Ruby's */
static unsigned
fileattr_to_unixmode(DWORD attr, const WCHAR *path)
{
    unsigned mode = 0;

    if (attr & FILE_ATTRIBUTE_READONLY) {
	mode |= S_IREAD;
    }
    else {
	mode |= S_IREAD | S_IWRITE | S_IWUSR;
    }

    if (attr & FILE_ATTRIBUTE_DIRECTORY) {
	mode |= S_IFDIR | S_IEXEC;
    }
    else {
	mode |= S_IFREG;
    }

    if (path && (mode & S_IFREG)) {
	const WCHAR *end = path + lstrlenW(path);
	while (path < end) {
	    end = CharPrevW(path, end);
	    if (*end == L'.') {
		if ((_wcsicmp(end, L".bat") == 0) ||
		    (_wcsicmp(end, L".cmd") == 0) ||
		    (_wcsicmp(end, L".com") == 0) ||
		    (_wcsicmp(end, L".exe") == 0)) {
		    mode |= S_IEXEC;
		}
		break;
	    }
	}
    }

    mode |= (mode & 0700) >> 3;
    mode |= (mode & 0700) >> 6;

    return mode;
}

/* License: Ruby's */
static int
check_valid_dir(const WCHAR *path)
{
    WIN32_FIND_DATAW fd;
    HANDLE fh;
    WCHAR full[MAX_PATH];
    WCHAR *dmy;

    /* if the specified path is the root of a drive and the drive is empty, */
    /* FindFirstFile() returns INVALID_HANDLE_VALUE. */
    if (!GetFullPathNameW(path, sizeof(full) / sizeof(WCHAR), full, &dmy)) {
	errno = map_errno(GetLastError());
	return -1;
    }
    if (full[1] == L':' && !full[3] && GetDriveTypeW(full) != DRIVE_NO_ROOT_DIR)
	return 0;

    fh = open_dir_handle(path, &fd);
    if (fh == INVALID_HANDLE_VALUE)
	return -1;
    FindClose(fh);
    return 0;
}

/* License: Ruby's */
static int
winnt_stat(const WCHAR *path, struct stati64 *st)
{
    HANDLE h;
    WIN32_FIND_DATAW wfd;

    memset(st, 0, sizeof(*st));
    st->st_nlink = 1;

    if (wcspbrk(path, L"?*")) {
	errno = ENOENT;
	return -1;
    }
    h = FindFirstFileW(path, &wfd);
    if (h != INVALID_HANDLE_VALUE) {
	FindClose(h);
	st->st_mode  = fileattr_to_unixmode(wfd.dwFileAttributes, path);
	st->st_atime = filetime_to_unixtime(&wfd.ftLastAccessTime);
	st->st_mtime = filetime_to_unixtime(&wfd.ftLastWriteTime);
	st->st_ctime = filetime_to_unixtime(&wfd.ftCreationTime);
	st->st_size = ((__int64)wfd.nFileSizeHigh << 32) | wfd.nFileSizeLow;
    }
    else {
	// If runtime stat(2) is called for network shares, it fails on WinNT.
	// Because GetDriveType returns 1 for network shares. (Win98 returns 4)
	DWORD attr = GetFileAttributesW(path);
	if (attr == (DWORD)-1L) {
	    errno = map_errno(GetLastError());
	    return -1;
	}
	if (attr & FILE_ATTRIBUTE_DIRECTORY) {
	    if (check_valid_dir(path)) return -1;
	}
	st->st_mode  = fileattr_to_unixmode(attr, path);
    }

    st->st_dev = st->st_rdev = (iswalpha(path[0]) && path[1] == L':') ?
	towupper(path[0]) - L'A' : _getdrive() - 1;

    return 0;
}

#ifdef WIN95
/* License: Ruby's */
static int
win95_stat(const WCHAR *path, struct stati64 *st)
{
    int ret = _wstati64(path, st);
    if (ret) return ret;
    if (st->st_mode & S_IFDIR) {
	return check_valid_dir(path);
    }
    return 0;
}
#else
#define win95_stat(path, st) -1
#endif

/* License: Ruby's */
int
rb_w32_stat(const char *path, struct stat *st)
{
    struct stati64 tmp;

    if (rb_w32_stati64(path, &tmp)) return -1;
    COPY_STAT(tmp, *st, (_off_t));
    return 0;
}

/* License: Ruby's */
static int
wstati64(const WCHAR *path, struct stati64 *st)
{
    const WCHAR *p;
    WCHAR *buf1, *s, *end;
    int len, size;
    int ret;
    VALUE v;

    if (!path || !st) {
	errno = EFAULT;
	return -1;
    }
    size = lstrlenW(path) + 2;
    buf1 = ALLOCV_N(WCHAR, v, size);
    for (p = path, s = buf1; *p; p++, s++) {
	if (*p == L'/')
	    *s = L'\\';
	else
	    *s = *p;
    }
    *s = '\0';
    len = s - buf1;
    if (!len || L'\"' == *(--s)) {
	errno = ENOENT;
	return -1;
    }
    end = buf1 + len - 1;

    if (isUNCRoot(buf1)) {
	if (*end == L'.')
	    *end = L'\0';
	else if (*end != L'\\')
	    lstrcatW(buf1, L"\\");
    }
    else if (*end == L'\\' || (buf1 + 1 == end && *end == L':'))
	lstrcatW(buf1, L".");

    ret = IsWinNT() ? winnt_stat(buf1, st) : win95_stat(buf1, st);
    if (ret == 0) {
	st->st_mode &= ~(S_IWGRP | S_IWOTH);
    }
    if (v)
	ALLOCV_END(v);

    return ret;
}

/* License: Ruby's */
int
rb_w32_ustati64(const char *path, struct stati64 *st)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = utf8_to_wstr(path, NULL)))
	return -1;
    ret = wstati64(wpath, st);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_stati64(const char *path, struct stati64 *st)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = filecp_to_wstr(path, NULL)))
	return -1;
    ret = wstati64(wpath, st);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_access(const char *path, int mode)
{
    struct stati64 stat;
    if (rb_w32_stati64(path, &stat) != 0)
	return -1;
    mode <<= 6;
    if ((stat.st_mode & mode) != mode) {
	errno = EACCES;
	return -1;
    }
    return 0;
}

/* License: Ruby's */
int
rb_w32_uaccess(const char *path, int mode)
{
    struct stati64 stat;
    if (rb_w32_ustati64(path, &stat) != 0)
	return -1;
    mode <<= 6;
    if ((stat.st_mode & mode) != mode) {
	errno = EACCES;
	return -1;
    }
    return 0;
}

/* License: Ruby's */
static int
rb_chsize(HANDLE h, off_t size)
{
    long upos, lpos, usize, lsize;
    int ret = -1;
    DWORD e;

    if ((lpos = SetFilePointer(h, 0, (upos = 0, &upos), SEEK_CUR)) == -1L &&
	(e = GetLastError())) {
	errno = map_errno(e);
	return -1;
    }
    usize = (long)(size >> 32);
    lsize = (long)size;
    if (SetFilePointer(h, lsize, &usize, SEEK_SET) == (DWORD)-1L &&
	(e = GetLastError())) {
	errno = map_errno(e);
    }
    else if (!SetEndOfFile(h)) {
	errno = map_errno(GetLastError());
    }
    else {
	ret = 0;
    }
    SetFilePointer(h, lpos, &upos, SEEK_SET);
    return ret;
}

/* License: Ruby's */
int
rb_w32_truncate(const char *path, off_t length)
{
    HANDLE h;
    int ret;
#ifdef WIN95
    if (IsWin95()) {
	int fd = open(path, O_WRONLY), e = 0;
	if (fd == -1) return -1;
	ret = chsize(fd, (unsigned long)length);
	if (ret == -1) e = errno;
	close(fd);
	if (ret == -1) errno = e;
	return ret;
    }
#endif
    h = CreateFile(path, GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    if (h == INVALID_HANDLE_VALUE) {
	errno = map_errno(GetLastError());
	return -1;
    }
    ret = rb_chsize(h, length);
    CloseHandle(h);
    return ret;
}

/* License: Ruby's */
int
rb_w32_ftruncate(int fd, off_t length)
{
    HANDLE h;

#ifdef WIN95
    if (IsWin95()) {
	return chsize(fd, (unsigned long)length);
    }
#endif
    h = (HANDLE)_get_osfhandle(fd);
    if (h == (HANDLE)-1) return -1;
    return rb_chsize(h, length);
}

#ifdef __BORLANDC__
/* License: Ruby's */
off_t
_filelengthi64(int fd)
{
    DWORD u, l;
    int e;

    l = GetFileSize((HANDLE)_get_osfhandle(fd), &u);
    if (l == (DWORD)-1L && (e = GetLastError())) {
	errno = map_errno(e);
	return (off_t)-1;
    }
    return ((off_t)u << 32) | l;
}

/* License: Ruby's */
off_t
_lseeki64(int fd, off_t offset, int whence)
{
    long u, l;
    int e;
    HANDLE h = (HANDLE)_get_osfhandle(fd);

    if (!h) {
	errno = EBADF;
	return -1;
    }
    u = (long)(offset >> 32);
    if ((l = SetFilePointer(h, (long)offset, &u, whence)) == -1L &&
	(e = GetLastError())) {
	errno = map_errno(e);
	return -1;
    }
    return ((off_t)u << 32) | l;
}
#endif

/* License: Ruby's */
int
fseeko(FILE *stream, off_t offset, int whence)
{
    off_t pos;
    switch (whence) {
      case SEEK_CUR:
	if (fgetpos(stream, (fpos_t *)&pos))
	    return -1;
	pos += offset;
	break;
      case SEEK_END:
	if ((pos = _filelengthi64(fileno(stream))) == (off_t)-1)
	    return -1;
	pos += offset;
	break;
      default:
	pos = offset;
	break;
    }
    return fsetpos(stream, (fpos_t *)&pos);
}

/* License: Ruby's */
off_t
rb_w32_ftello(FILE *stream)
{
    off_t pos;
    if (fgetpos(stream, (fpos_t *)&pos)) return (off_t)-1;
    return pos;
}

/* License: Ruby's */
static long
filetime_to_clock(FILETIME *ft)
{
    __int64 qw = ft->dwHighDateTime;
    qw <<= 32;
    qw |= ft->dwLowDateTime;
    qw /= 10000;  /* File time ticks at 0.1uS, clock at 1mS */
    return (long) qw;
}

/* License: Ruby's */
int
rb_w32_times(struct tms *tmbuf)
{
    FILETIME create, exit, kernel, user;

    if (GetProcessTimes(GetCurrentProcess(),&create, &exit, &kernel, &user)) {
	tmbuf->tms_utime = filetime_to_clock(&user);
	tmbuf->tms_stime = filetime_to_clock(&kernel);
	tmbuf->tms_cutime = 0;
	tmbuf->tms_cstime = 0;
    }
    else {
	tmbuf->tms_utime = clock();
	tmbuf->tms_stime = 0;
	tmbuf->tms_cutime = 0;
	tmbuf->tms_cstime = 0;
    }
    return 0;
}

#define yield_once() Sleep(0)
#define yield_until(condition) do yield_once(); while (!(condition))

/* License: Ruby's */
static void
catch_interrupt(void)
{
    yield_once();
    RUBY_CRITICAL(rb_w32_wait_events(NULL, 0, 0));
}

#if defined __BORLANDC__
#undef read
/* License: Ruby's */
int
read(int fd, void *buf, size_t size)
{
    int ret = _read(fd, buf, size);
    if ((ret < 0) && (errno == EPIPE)) {
	errno = 0;
	ret = 0;
    }
    catch_interrupt();
    return ret;
}
#endif


#define FILE_COUNT _cnt
#define FILE_READPTR _ptr

#undef fgetc
/* License: Ruby's */
int
rb_w32_getc(FILE* stream)
{
    int c;
    if (enough_to_get(stream->FILE_COUNT)) {
	c = (unsigned char)*stream->FILE_READPTR++;
    }
    else {
	c = _filbuf(stream);
#if defined __BORLANDC__
        if ((c == EOF) && (errno == EPIPE)) {
	    clearerr(stream);
        }
#endif
	catch_interrupt();
    }
    return c;
}

#undef fputc
/* License: Ruby's */
int
rb_w32_putc(int c, FILE* stream)
{
    if (enough_to_put(stream->FILE_COUNT)) {
	c = (unsigned char)(*stream->FILE_READPTR++ = (char)c);
    }
    else {
	c = _flsbuf(c, stream);
	catch_interrupt();
    }
    return c;
}

/* License: Ruby's */
struct asynchronous_arg_t {
    /* output field */
    void* stackaddr;
    int errnum;

    /* input field */
    uintptr_t (*func)(uintptr_t self, int argc, uintptr_t* argv);
    uintptr_t self;
    int argc;
    uintptr_t* argv;
};

/* License: Ruby's */
static DWORD WINAPI
call_asynchronous(PVOID argp)
{
    DWORD ret;
    struct asynchronous_arg_t *arg = argp;
    arg->stackaddr = &argp;
    ret = (DWORD)arg->func(arg->self, arg->argc, arg->argv);
    arg->errnum = errno;
    return ret;
}

/* License: Ruby's */
uintptr_t
rb_w32_asynchronize(asynchronous_func_t func, uintptr_t self,
		    int argc, uintptr_t* argv, uintptr_t intrval)
{
    DWORD val;
    BOOL interrupted = FALSE;
    HANDLE thr;

    RUBY_CRITICAL({
	struct asynchronous_arg_t arg;

	arg.stackaddr = NULL;
	arg.errnum = 0;
	arg.func = func;
	arg.self = self;
	arg.argc = argc;
	arg.argv = argv;

	thr = CreateThread(NULL, 0, call_asynchronous, &arg, 0, &val);

	if (thr) {
	    yield_until(arg.stackaddr);

	    if (rb_w32_wait_events_blocking(&thr, 1, INFINITE) != WAIT_OBJECT_0) {
		interrupted = TRUE;

		if (TerminateThread(thr, intrval)) {
		    yield_once();
		}
	    }

	    GetExitCodeThread(thr, &val);
	    CloseHandle(thr);

	    if (interrupted) {
		/* must release stack of killed thread, why doesn't Windows? */
		MEMORY_BASIC_INFORMATION m;

		memset(&m, 0, sizeof(m));
		if (!VirtualQuery(arg.stackaddr, &m, sizeof(m))) {
		    Debug(fprintf(stderr, "couldn't get stack base:%p:%d\n",
				  arg.stackaddr, GetLastError()));
		}
		else if (!VirtualFree(m.AllocationBase, 0, MEM_RELEASE)) {
		    Debug(fprintf(stderr, "couldn't release stack:%p:%d\n",
				  m.AllocationBase, GetLastError()));
		}
		errno = EINTR;
	    }
	    else {
		errno = arg.errnum;
	    }
	}
    });

    if (!thr) {
	rb_fatal("failed to launch waiter thread:%ld", GetLastError());
    }

    return val;
}

/* License: Ruby's */
char **
rb_w32_get_environ(void)
{
    WCHAR *envtop, *env;
    char **myenvtop, **myenv;
    int num;

    /*
     * We avoid values started with `='. If you want to deal those values,
     * change this function, and some functions in hash.c which recognize
     * `=' as delimiter or rb_w32_getenv() and ruby_setenv().
     * CygWin deals these values by changing first `=' to '!'. But we don't
     * use such trick and follow cmd.exe's way that just doesn't show these
     * values.
     *
     * This function returns UTF-8 strings.
     */
    envtop = GetEnvironmentStringsW();
    for (env = envtop, num = 0; *env; env += lstrlenW(env) + 1)
	if (*env != '=') num++;

    myenvtop = (char **)malloc(sizeof(char *) * (num + 1));
    for (env = envtop, myenv = myenvtop; *env; env += lstrlenW(env) + 1) {
	if (*env != '=') {
	    if (!(*myenv = wstr_to_utf8(env, NULL))) {
		break;
	    }
	    myenv++;
	}
    }
    *myenv = NULL;
    FreeEnvironmentStringsW(envtop);

    return myenvtop;
}

/* License: Ruby's */
void
rb_w32_free_environ(char **env)
{
    char **t = env;

    while (*t) free(*t++);
    free(env);
}

/* License: Ruby's */
rb_pid_t
rb_w32_getpid(void)
{
    rb_pid_t pid;

    pid = GetCurrentProcessId();

    (void)IfWin95(pid = -pid, 0);

    return pid;
}


/* License: Ruby's */
rb_pid_t
rb_w32_getppid(void)
{
    typedef long (WINAPI query_func)(HANDLE, int, void *, ULONG, ULONG *);
    static query_func *pNtQueryInformationProcess = NULL;
    rb_pid_t ppid = 0;

    if (!IsWin95() && rb_w32_osver() >= 5) {
	if (!pNtQueryInformationProcess)
	    pNtQueryInformationProcess = (query_func *)get_proc_address("ntdll.dll", "NtQueryInformationProcess", NULL);
	if (pNtQueryInformationProcess) {
	    struct {
		long ExitStatus;
		void* PebBaseAddress;
		uintptr_t AffinityMask;
		uintptr_t BasePriority;
		uintptr_t UniqueProcessId;
		uintptr_t ParentProcessId;
	    } pbi;
	    ULONG len;
	    long ret = pNtQueryInformationProcess(GetCurrentProcess(), 0, &pbi, sizeof(pbi), &len);
	    if (!ret) {
		ppid = pbi.ParentProcessId;
	    }
	}
    }

    return ppid;
}

/* License: Ruby's */
int
rb_w32_uopen(const char *file, int oflag, ...)
{
    WCHAR *wfile;
    int ret;
    int pmode;

    va_list arg;
    va_start(arg, oflag);
    pmode = va_arg(arg, int);
    va_end(arg);

    if (!(wfile = utf8_to_wstr(file, NULL)))
	return -1;
    ret = rb_w32_wopen(wfile, oflag, pmode);
    free(wfile);
    return ret;
}

/* License: Ruby's */
int
rb_w32_open(const char *file, int oflag, ...)
{
    WCHAR *wfile;
    int ret;
    int pmode;

    va_list arg;
    va_start(arg, oflag);
    pmode = va_arg(arg, int);
    va_end(arg);

    if ((oflag & O_TEXT) || !(oflag & O_BINARY))
	return _open(file, oflag, pmode);

    if (!(wfile = filecp_to_wstr(file, NULL)))
	return -1;
    ret = rb_w32_wopen(wfile, oflag, pmode);
    free(wfile);
    return ret;
}

int
rb_w32_wopen(const WCHAR *file, int oflag, ...)
{
    char flags = 0;
    int fd;
    DWORD access;
    DWORD create;
    DWORD attr = FILE_ATTRIBUTE_NORMAL;
    SECURITY_ATTRIBUTES sec;
    HANDLE h;

    if ((oflag & O_TEXT) || !(oflag & O_BINARY)) {
	va_list arg;
	int pmode;
	va_start(arg, oflag);
	pmode = va_arg(arg, int);
	va_end(arg);
	return _wopen(file, oflag, pmode);
    }

    sec.nLength = sizeof(sec);
    sec.lpSecurityDescriptor = NULL;
    if (oflag & O_NOINHERIT) {
	sec.bInheritHandle = FALSE;
	flags |= FNOINHERIT;
    }
    else {
	sec.bInheritHandle = TRUE;
    }
    oflag &= ~O_NOINHERIT;

    /* always open with binary mode */
    oflag &= ~(O_BINARY | O_TEXT);

    switch (oflag & (O_RDWR | O_RDONLY | O_WRONLY)) {
      case O_RDWR:
	access = GENERIC_READ | GENERIC_WRITE;
	break;
      case O_RDONLY:
	access = GENERIC_READ;
	break;
      case O_WRONLY:
	access = GENERIC_WRITE;
	break;
      default:
	errno = EINVAL;
	return -1;
    }
    oflag &= ~(O_RDWR | O_RDONLY | O_WRONLY);

    switch (oflag & (O_CREAT | O_EXCL | O_TRUNC)) {
      case O_CREAT:
	create = OPEN_ALWAYS;
	break;
      case 0:
      case O_EXCL:
	create = OPEN_EXISTING;
	break;
      case O_CREAT | O_EXCL:
      case O_CREAT | O_EXCL | O_TRUNC:
	create = CREATE_NEW;
	break;
      case O_TRUNC:
      case O_TRUNC | O_EXCL:
	create = TRUNCATE_EXISTING;
	break;
      case O_CREAT | O_TRUNC:
	create = CREATE_ALWAYS;
	break;
      default:
	errno = EINVAL;
	return -1;
    }
    if (oflag & O_CREAT) {
	va_list arg;
	int pmode;
	va_start(arg, oflag);
	pmode = va_arg(arg, int);
	va_end(arg);
	/* TODO: we need to check umask here, but it's not exported... */
	if (!(pmode & S_IWRITE))
	    attr = FILE_ATTRIBUTE_READONLY;
    }
    oflag &= ~(O_CREAT | O_EXCL | O_TRUNC);

    if (oflag & O_TEMPORARY) {
	attr |= FILE_FLAG_DELETE_ON_CLOSE;
	access |= DELETE;
    }
    oflag &= ~O_TEMPORARY;

    if (oflag & _O_SHORT_LIVED)
	attr |= FILE_ATTRIBUTE_TEMPORARY;
    oflag &= ~_O_SHORT_LIVED;

    switch (oflag & (O_SEQUENTIAL | O_RANDOM)) {
      case 0:
	break;
      case O_SEQUENTIAL:
	attr |= FILE_FLAG_SEQUENTIAL_SCAN;
	break;
      case O_RANDOM:
	attr |= FILE_FLAG_RANDOM_ACCESS;
	break;
      default:
	errno = EINVAL;
	return -1;
    }
    oflag &= ~(O_SEQUENTIAL | O_RANDOM);

    if (oflag & ~O_APPEND) {
	errno = EINVAL;
	return -1;
    }

    /* allocate a C Runtime file handle */
    RUBY_CRITICAL({
	h = CreateFile("NUL", 0, 0, NULL, OPEN_ALWAYS, 0, NULL);
	fd = _open_osfhandle((intptr_t)h, 0);
	CloseHandle(h);
    });
    if (fd == -1) {
	errno = EMFILE;
	return -1;
    }
    RUBY_CRITICAL({
	MTHREAD_ONLY(EnterCriticalSection(&(_pioinfo(fd)->lock)));
	_set_osfhnd(fd, (intptr_t)INVALID_HANDLE_VALUE);
	_set_osflags(fd, 0);

	h = CreateFileW(file, access, FILE_SHARE_READ | FILE_SHARE_WRITE, &sec,
			create, attr, NULL);
	if (h == INVALID_HANDLE_VALUE) {
	    errno = map_errno(GetLastError());
	    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	    fd = -1;
	    goto quit;
	}

	switch (GetFileType(h)) {
	  case FILE_TYPE_CHAR:
	    flags |= FDEV;
	    break;
	  case FILE_TYPE_PIPE:
	    flags |= FPIPE;
	    break;
	  case FILE_TYPE_UNKNOWN:
	    errno = map_errno(GetLastError());
	    CloseHandle(h);
	    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	    fd = -1;
	    goto quit;
	}
	if (!(flags & (FDEV | FPIPE)) && (oflag & O_APPEND))
	    flags |= FAPPEND;

	_set_osfhnd(fd, (intptr_t)h);
	_osfile(fd) = flags | FOPEN;

	MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
      quit:
	;
    });

    return fd;
}

/* License: Ruby's */
int
rb_w32_fclose(FILE *fp)
{
    int fd = fileno(fp);
    SOCKET sock = TO_SOCKET(fd);
    int save_errno = errno;

    if (fflush(fp)) return -1;
    if (!is_socket(sock)) {
	UnlockFile((HANDLE)sock, 0, 0, LK_LEN, LK_LEN);
	return fclose(fp);
    }
    _set_osfhnd(fd, (SOCKET)INVALID_HANDLE_VALUE);
    fclose(fp);
    errno = save_errno;
    if (closesocket(sock) == SOCKET_ERROR) {
	errno = map_errno(WSAGetLastError());
	return -1;
    }
    return 0;
}

/* License: Ruby's */
int
rb_w32_pipe(int fds[2])
{
    static DWORD serial = 0;
    char name[] = "\\\\.\\pipe\\ruby0000000000000000-0000000000000000";
    char *p;
    SECURITY_ATTRIBUTES sec;
    HANDLE hRead, hWrite, h;
    int fdRead, fdWrite;
    int ret;

    /* if doesn't have CancelIo, use default pipe function */
    if (!cancel_io)
	return _pipe(fds, 65536L, _O_NOINHERIT);

    p = strchr(name, '0');
    snprintf(p, strlen(p) + 1, "%"PRI_PIDT_PREFIX"x-%lx", rb_w32_getpid(), serial++);

    sec.nLength = sizeof(sec);
    sec.lpSecurityDescriptor = NULL;
    sec.bInheritHandle = FALSE;

    RUBY_CRITICAL({
	hRead = CreateNamedPipe(name, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
				0, 2, 65536, 65536, 0, &sec);
    });
    if (hRead == INVALID_HANDLE_VALUE) {
	DWORD err = GetLastError();
	if (err == ERROR_PIPE_BUSY)
	    errno = EMFILE;
	else
	    errno = map_errno(GetLastError());
	return -1;
    }

    RUBY_CRITICAL({
	hWrite = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, &sec,
			    OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    });
    if (hWrite == INVALID_HANDLE_VALUE) {
	errno = map_errno(GetLastError());
	CloseHandle(hRead);
	return -1;
    }

    RUBY_CRITICAL(do {
	ret = 0;
	h = CreateFile("NUL", 0, 0, NULL, OPEN_ALWAYS, 0, NULL);
	fdRead = _open_osfhandle((intptr_t)h, 0);
	CloseHandle(h);
	if (fdRead == -1) {
	    errno = EMFILE;
	    CloseHandle(hWrite);
	    CloseHandle(hRead);
	    ret = -1;
	    break;
	}

	MTHREAD_ONLY(EnterCriticalSection(&(_pioinfo(fdRead)->lock)));
	_set_osfhnd(fdRead, (intptr_t)hRead);
	_set_osflags(fdRead, FOPEN | FPIPE | FNOINHERIT);
	MTHREAD_ONLY(LeaveCriticalSection(&(_pioinfo(fdRead)->lock)));
    } while (0));
    if (ret)
	return ret;

    RUBY_CRITICAL(do {
	h = CreateFile("NUL", 0, 0, NULL, OPEN_ALWAYS, 0, NULL);
	fdWrite = _open_osfhandle((intptr_t)h, 0);
	CloseHandle(h);
	if (fdWrite == -1) {
	    errno = EMFILE;
	    CloseHandle(hWrite);
	    ret = -1;
	    break;
	}
	MTHREAD_ONLY(EnterCriticalSection(&(_pioinfo(fdWrite)->lock)));
	_set_osfhnd(fdWrite, (intptr_t)hWrite);
	_set_osflags(fdWrite, FOPEN | FPIPE | FNOINHERIT);
	MTHREAD_ONLY(LeaveCriticalSection(&(_pioinfo(fdWrite)->lock)));
    } while (0));
    if (ret) {
	rb_w32_close(fdRead);
	return ret;
    }

    fds[0] = fdRead;
    fds[1] = fdWrite;

    return 0;
}

/* License: Ruby's */
int
rb_w32_close(int fd)
{
    SOCKET sock = TO_SOCKET(fd);
    int save_errno = errno;

    if (!is_socket(sock)) {
	UnlockFile((HANDLE)sock, 0, 0, LK_LEN, LK_LEN);
	return _close(fd);
    }
    _set_osfhnd(fd, (SOCKET)INVALID_HANDLE_VALUE);
    socklist_delete(&sock, NULL);
    _close(fd);
    errno = save_errno;
    if (closesocket(sock) == SOCKET_ERROR) {
	errno = map_errno(WSAGetLastError());
	return -1;
    }
    return 0;
}

#undef read
/* License: Ruby's */
ssize_t
rb_w32_read(int fd, void *buf, size_t size)
{
    SOCKET sock = TO_SOCKET(fd);
    DWORD read;
    DWORD wait;
    DWORD err;
    size_t len;
    size_t ret;
    OVERLAPPED ol, *pol = NULL;
    BOOL isconsole;
    BOOL islineinput = FALSE;
    int start = 0;

    if (is_socket(sock))
	return rb_w32_recv(fd, buf, size, 0);

    // validate fd by using _get_osfhandle() because we cannot access _nhandle
    if (_get_osfhandle(fd) == -1) {
	return -1;
    }

    if (_osfile(fd) & FTEXT) {
	return _read(fd, buf, size);
    }

    MTHREAD_ONLY(EnterCriticalSection(&(_pioinfo(fd)->lock)));

    if (!size || _osfile(fd) & FEOFLAG) {
	_set_osflags(fd, _osfile(fd) & ~FEOFLAG);
	MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	return 0;
    }

    ret = 0;
    isconsole = is_console(_osfhnd(fd));
    if (isconsole) {
	DWORD mode;
	GetConsoleMode((HANDLE)_osfhnd(fd),&mode);
	islineinput = (mode & ENABLE_LINE_INPUT) != 0;
    }
  retry:
    /* get rid of console reading bug */
    if (isconsole) {
	if (start)
	    len = 1;
	else {
	    len = 0;
	    start = 1;
	}
    }
    else
	len = size;
    size -= len;

    /* if have cancel_io, use Overlapped I/O */
    if (cancel_io) {
	memset(&ol, 0, sizeof(ol));
	if (!(_osfile(fd) & (FDEV | FPIPE))) {
	    LONG high = 0;
	    DWORD low = SetFilePointer((HANDLE)_osfhnd(fd), 0, &high,
				       FILE_CURRENT);
#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif
	    if (low == INVALID_SET_FILE_POINTER) {
		errno = map_errno(GetLastError());
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		return -1;
	    }
	    ol.Offset = low;
	    ol.OffsetHigh = high;
	}
	ol.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (!ol.hEvent) {
	    errno = map_errno(GetLastError());
	    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	    return -1;
	}

	pol = &ol;
    }

    if (!ReadFile((HANDLE)_osfhnd(fd), buf, len, &read, pol)) {
	err = GetLastError();
	if (err != ERROR_IO_PENDING) {
	    if (pol) CloseHandle(ol.hEvent);
	    if (err == ERROR_ACCESS_DENIED)
		errno = EBADF;
	    else if (err == ERROR_BROKEN_PIPE || err == ERROR_HANDLE_EOF) {
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		return 0;
	    }
	    else
		errno = map_errno(err);

	    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	    return -1;
	}

	if (pol) {
	    wait = rb_w32_wait_events_blocking(&ol.hEvent, 1, INFINITE);
	    if (wait != WAIT_OBJECT_0) {
		if (wait == WAIT_OBJECT_0 + 1)
		    errno = EINTR;
		else
		    errno = map_errno(GetLastError());
		CloseHandle(ol.hEvent);
		cancel_io((HANDLE)_osfhnd(fd));
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		return -1;
	    }

	    if (!GetOverlappedResult((HANDLE)_osfhnd(fd), &ol, &read, TRUE) &&
		(err = GetLastError()) != ERROR_HANDLE_EOF) {
		int ret = 0;
		if (err != ERROR_BROKEN_PIPE) {
		    errno = map_errno(err);
		    ret = -1;
		}
		CloseHandle(ol.hEvent);
		cancel_io((HANDLE)_osfhnd(fd));
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		return ret;
	    }
	}
    }
    else {
	err = GetLastError();
	errno = map_errno(err);
    }

    if (pol) {
	CloseHandle(ol.hEvent);

	if (!(_osfile(fd) & (FDEV | FPIPE))) {
	    LONG high = ol.OffsetHigh;
	    DWORD low = ol.Offset + read;
	    if (low < ol.Offset)
		++high;
	    SetFilePointer((HANDLE)_osfhnd(fd), low, &high, FILE_BEGIN);
	}
    }

    ret += read;
    if (read >= len) {
	buf = (char *)buf + read;
	if (err != ERROR_OPERATION_ABORTED &&
	    !(isconsole && len == 1 && (!islineinput || *((char *)buf - 1) == '\n')) && size > 0)
	    goto retry;
    }
    if (read == 0)
	_set_osflags(fd, _osfile(fd) | FEOFLAG);


    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));

    return ret;
}

#undef write
/* License: Ruby's */
ssize_t
rb_w32_write(int fd, const void *buf, size_t size)
{
    SOCKET sock = TO_SOCKET(fd);
    DWORD written;
    DWORD wait;
    DWORD err;
    size_t len;
    size_t ret;
    OVERLAPPED ol, *pol = NULL;

    if (is_socket(sock))
	return rb_w32_send(fd, buf, size, 0);

    // validate fd by using _get_osfhandle() because we cannot access _nhandle
    if (_get_osfhandle(fd) == -1) {
	return -1;
    }

    if ((_osfile(fd) & FTEXT) &&
        (!(_osfile(fd) & FPIPE) || fd == fileno(stdout) || fd == fileno(stderr))) {
	return _write(fd, buf, size);
    }

    MTHREAD_ONLY(EnterCriticalSection(&(_pioinfo(fd)->lock)));

    if (!size || _osfile(fd) & FEOFLAG) {
	MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	return 0;
    }

    ret = 0;
  retry:
    /* get rid of console writing bug */
    len = (_osfile(fd) & FDEV) ? min(32 * 1024, size) : size;
    size -= len;

    /* if have cancel_io, use Overlapped I/O */
    if (cancel_io) {
	memset(&ol, 0, sizeof(ol));
	if (!(_osfile(fd) & (FDEV | FPIPE))) {
	    LONG high = 0;
	    DWORD method = _osfile(fd) & FAPPEND ? FILE_END : FILE_CURRENT;
	    DWORD low = SetFilePointer((HANDLE)_osfhnd(fd), 0, &high, method);
#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif
	    if (low == INVALID_SET_FILE_POINTER) {
		errno = map_errno(GetLastError());
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		return -1;
	    }
	    ol.Offset = low;
	    ol.OffsetHigh = high;
	}
	ol.hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (!ol.hEvent) {
	    errno = map_errno(GetLastError());
	    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	    return -1;
	}

	pol = &ol;
    }

    if (!WriteFile((HANDLE)_osfhnd(fd), buf, len, &written, pol)) {
	err = GetLastError();
	if (err != ERROR_IO_PENDING) {
	    if (pol) CloseHandle(ol.hEvent);
	    if (err == ERROR_ACCESS_DENIED)
		errno = EBADF;
	    else
		errno = map_errno(err);

	    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
	    return -1;
	}

	if (pol) {
	    wait = rb_w32_wait_events_blocking(&ol.hEvent, 1, INFINITE);
	    if (wait != WAIT_OBJECT_0) {
		if (wait == WAIT_OBJECT_0 + 1)
		    errno = EINTR;
		else
		    errno = map_errno(GetLastError());
		CloseHandle(ol.hEvent);
		cancel_io((HANDLE)_osfhnd(fd));
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		return -1;
	    }

	    if (!GetOverlappedResult((HANDLE)_osfhnd(fd), &ol, &written,
				     TRUE)) {
		errno = map_errno(err);
		CloseHandle(ol.hEvent);
		cancel_io((HANDLE)_osfhnd(fd));
		MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));
		return -1;
	    }
	}
    }

    if (pol) {
	CloseHandle(ol.hEvent);

	if (!(_osfile(fd) & (FDEV | FPIPE))) {
	    LONG high = ol.OffsetHigh;
	    DWORD low = ol.Offset + written;
	    if (low < ol.Offset)
		++high;
	    SetFilePointer((HANDLE)_osfhnd(fd), low, &high, FILE_BEGIN);
	}
    }

    ret += written;
    if (written == len) {
	buf = (const char *)buf + len;
	if (size > 0)
	    goto retry;
    }

    MTHREAD_ONLY(LeaveCriticalSection(&_pioinfo(fd)->lock));

    return ret;
}

/* License: Ruby's */
long
rb_w32_write_console(uintptr_t strarg, int fd)
{
    static int disable;
    HANDLE handle;
    DWORD dwMode, reslen;
    VALUE str = strarg;

    if (disable) return -1L;
    handle = (HANDLE)_osfhnd(fd);
    if (!GetConsoleMode(handle, &dwMode) ||
	!rb_econv_has_convpath_p(rb_enc_name(rb_enc_get(str)), "UTF-16LE"))
	return -1L;

    str = rb_str_encode(str, rb_enc_from_encoding(rb_enc_find("UTF-16LE")),
			ECONV_INVALID_REPLACE|ECONV_UNDEF_REPLACE, Qnil);
    if (!WriteConsoleW(handle, (LPWSTR)RSTRING_PTR(str), RSTRING_LEN(str)/2,
		       &reslen, NULL)) {
	if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
	    disable = TRUE;
	return -1L;
    }
    return (long)reslen;
}

/* License: Ruby's */
static int
unixtime_to_filetime(time_t time, FILETIME *ft)
{
    struct tm *tm;
    SYSTEMTIME st;
    FILETIME lt;

    tm = localtime(&time);
    if (!tm) {
	return -1;
    }
    st.wYear = tm->tm_year + 1900;
    st.wMonth = tm->tm_mon + 1;
    st.wDayOfWeek = tm->tm_wday;
    st.wDay = tm->tm_mday;
    st.wHour = tm->tm_hour;
    st.wMinute = tm->tm_min;
    st.wSecond = tm->tm_sec;
    st.wMilliseconds = 0;
    if (!SystemTimeToFileTime(&st, &lt) ||
	!LocalFileTimeToFileTime(&lt, ft)) {
	errno = map_errno(GetLastError());
	return -1;
    }
    return 0;
}

/* License: Ruby's */
static int
wutime(const WCHAR *path, const struct utimbuf *times)
{
    HANDLE hFile;
    FILETIME atime, mtime;
    struct stati64 stat;
    int ret = 0;

    if (wstati64(path, &stat)) {
	return -1;
    }

    if (times) {
	if (unixtime_to_filetime(times->actime, &atime)) {
	    return -1;
	}
	if (unixtime_to_filetime(times->modtime, &mtime)) {
	    return -1;
	}
    }
    else {
	GetSystemTimeAsFileTime(&atime);
	mtime = atime;
    }

    RUBY_CRITICAL({
	const DWORD attr = GetFileAttributesW(path);
	if (attr != (DWORD)-1 && (attr & FILE_ATTRIBUTE_READONLY))
	    SetFileAttributesW(path, attr & ~FILE_ATTRIBUTE_READONLY);
	hFile = CreateFileW(path, GENERIC_WRITE, 0, 0, OPEN_EXISTING,
			    IsWin95() ? 0 : FILE_FLAG_BACKUP_SEMANTICS, 0);
	if (hFile == INVALID_HANDLE_VALUE) {
	    errno = map_errno(GetLastError());
	    ret = -1;
	}
	else {
	    if (!SetFileTime(hFile, NULL, &atime, &mtime)) {
		errno = map_errno(GetLastError());
		ret = -1;
	    }
	    CloseHandle(hFile);
	}
	if (attr != (DWORD)-1 && (attr & FILE_ATTRIBUTE_READONLY))
	    SetFileAttributesW(path, attr);
    });

    return ret;
}

/* License: Ruby's */
int
rb_w32_uutime(const char *path, const struct utimbuf *times)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = utf8_to_wstr(path, NULL)))
	return -1;
    ret = wutime(wpath, times);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_utime(const char *path, const struct utimbuf *times)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = filecp_to_wstr(path, NULL)))
	return -1;
    ret = wutime(wpath, times);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_uchdir(const char *path)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = utf8_to_wstr(path, NULL)))
	return -1;
    ret = _wchdir(wpath);
    free(wpath);
    return ret;
}

/* License: Ruby's */
static int
wmkdir(const WCHAR *wpath, int mode)
{
    int ret = -1;

    RUBY_CRITICAL(do {
	if (CreateDirectoryW(wpath, NULL) == FALSE) {
	    errno = map_errno(GetLastError());
	    break;
	}
	if (_wchmod(wpath, mode) == -1) {
	    RemoveDirectoryW(wpath);
	    break;
	}
	ret = 0;
    } while (0));
    return ret;
}

/* License: Ruby's */
int
rb_w32_umkdir(const char *path, int mode)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = utf8_to_wstr(path, NULL)))
	return -1;
    ret = wmkdir(wpath, mode);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_mkdir(const char *path, int mode)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = filecp_to_wstr(path, NULL)))
	return -1;
    ret = wmkdir(wpath, mode);
    free(wpath);
    return ret;
}

/* License: Ruby's */
static int
wrmdir(const WCHAR *wpath)
{
    int ret = 0;
    RUBY_CRITICAL({
	const DWORD attr = GetFileAttributesW(wpath);
	if (attr != (DWORD)-1 && (attr & FILE_ATTRIBUTE_READONLY)) {
	    SetFileAttributesW(wpath, attr & ~FILE_ATTRIBUTE_READONLY);
	}
	if (RemoveDirectoryW(wpath) == FALSE) {
	    errno = map_errno(GetLastError());
	    ret = -1;
	    if (attr != (DWORD)-1 && (attr & FILE_ATTRIBUTE_READONLY)) {
		SetFileAttributesW(wpath, attr);
	    }
	}
    });
    return ret;
}

/* License: Ruby's */
int
rb_w32_rmdir(const char *path)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = filecp_to_wstr(path, NULL)))
	return -1;
    ret = wrmdir(wpath);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_urmdir(const char *path)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = utf8_to_wstr(path, NULL)))
	return -1;
    ret = wrmdir(wpath);
    free(wpath);
    return ret;
}

/* License: Ruby's */
static int
wunlink(const WCHAR *path)
{
    int ret = 0;
    RUBY_CRITICAL({
	const DWORD attr = GetFileAttributesW(path);
	if (attr != (DWORD)-1 && (attr & FILE_ATTRIBUTE_READONLY)) {
	    SetFileAttributesW(path, attr & ~FILE_ATTRIBUTE_READONLY);
	}
	if (!DeleteFileW(path)) {
	    errno = map_errno(GetLastError());
	    ret = -1;
	    if (attr != (DWORD)-1 && (attr & FILE_ATTRIBUTE_READONLY)) {
		SetFileAttributesW(path, attr);
	    }
	}
    });
    return ret;
}

/* License: Ruby's */
int
rb_w32_uunlink(const char *path)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = utf8_to_wstr(path, NULL)))
	return -1;
    ret = wunlink(wpath);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_unlink(const char *path)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = filecp_to_wstr(path, NULL)))
	return -1;
    ret = wunlink(wpath);
    free(wpath);
    return ret;
}

/* License: Ruby's */
int
rb_w32_uchmod(const char *path, int mode)
{
    WCHAR *wpath;
    int ret;

    if (!(wpath = utf8_to_wstr(path, NULL)))
	return -1;
    ret = _wchmod(wpath, mode);
    free(wpath);
    return ret;
}

#if !defined(__BORLANDC__)
/* License: Ruby's */
int
rb_w32_isatty(int fd)
{
    DWORD mode;

    // validate fd by using _get_osfhandle() because we cannot access _nhandle
    if (_get_osfhandle(fd) == -1) {
	return 0;
    }
    if (!GetConsoleMode((HANDLE)_osfhnd(fd), &mode)) {
	errno = ENOTTY;
	return 0;
    }
    return 1;
}
#endif

//
// Fix bcc32's stdio bug
//

#ifdef __BORLANDC__
/* License: Ruby's */
static int
too_many_files(void)
{
    FILE *f;
    for (f = _streams; f < _streams + _nfile; f++) {
	if (f->fd < 0) return 0;
    }
    return 1;
}

#undef fopen
/* License: Ruby's */
FILE *
rb_w32_fopen(const char *path, const char *mode)
{
    FILE *f = (errno = 0, fopen(path, mode));
    if (f == NULL && errno == 0) {
	if (too_many_files())
	    errno = EMFILE;
    }
    return f;
}

/* License: Ruby's */
FILE *
rb_w32_fdopen(int handle, const char *type)
{
    FILE *f = (errno = 0, _fdopen(handle, (char *)type));
    if (f == NULL && errno == 0) {
	if (handle < 0)
	    errno = EBADF;
	else if (too_many_files())
	    errno = EMFILE;
    }
    return f;
}

/* License: Ruby's */
FILE *
rb_w32_fsopen(const char *path, const char *mode, int shflags)
{
    FILE *f = (errno = 0, _fsopen(path, mode, shflags));
    if (f == NULL && errno == 0) {
	if (too_many_files())
	    errno = EMFILE;
    }
    return f;
}
#endif

#if defined(_MSC_VER) && RT_VER <= 60
extern long _ftol(double);
/* License: Ruby's */
long
_ftol2(double d)
{
    return _ftol(d);
}

/* License: Ruby's */
long
_ftol2_sse(double d)
{
    return _ftol(d);
}
#endif

#ifndef signbit
/* License: Ruby's */
int
signbit(double x)
{
    int *ip = (int *)(&x + 1) - 1;
    return *ip < 0;
}
#endif

/* License: Ruby's */
char * WSAAPI
rb_w32_inet_ntop(int af, const void *addr, char *numaddr, size_t numaddr_len)
{
    typedef char *(WSAAPI inet_ntop_t)(int, void *, char *, size_t);
    inet_ntop_t *pInetNtop;
    pInetNtop = (inet_ntop_t *)get_proc_address("ws2_32", "inet_ntop", NULL);
    if (pInetNtop) {
	return pInetNtop(af, (void *)addr, numaddr, numaddr_len);
    }
    else {
	struct in_addr in;
	memcpy(&in.s_addr, addr, sizeof(in.s_addr));
	snprintf(numaddr, numaddr_len, "%s", inet_ntoa(in));
    }
    return numaddr;
}

char
rb_w32_fd_is_text(int fd) {
    return _osfile(fd) & FTEXT;
}
