/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@php.net>                                  |
   *         Pierre Joye <pierre@php.net>                                 |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "winutil.h"
#include "codepage.h"
#include <bcrypt.h>
#include <lmcons.h>


PHP_WINUTIL_API char *php_win32_error_to_msg(HRESULT error)
{/*{{{*/
	wchar_t *bufw = NULL, *pw;
	char *buf;

	DWORD ret = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),	(LPWSTR)&bufw, 0, NULL
	);

	if (!ret || !bufw) {
		return "";
	}

	/* strip trailing line breaks and periods */
	for (pw = bufw + wcslen(bufw) - 1; pw >= bufw && (*pw == L'\r' || *pw == L'\n' || *pw == L'.'); pw--);
	pw[1] = L'\0';

	buf = php_win32_cp_conv_w_to_any(bufw, ret, PHP_WIN32_CP_IGNORE_LEN_P);

	LocalFree(bufw);

	return (buf ? buf : "");
}/*}}}*/

PHP_WINUTIL_API void php_win32_error_msg_free(char *msg)
{/*{{{*/
	if (msg && msg[0]) {
		free(msg);
	}
}/*}}}*/

int php_win32_check_trailing_space(const char * path, const size_t path_len)
{/*{{{*/
	if (path_len > MAXPATHLEN - 1) {
		return 1;
	}
	if (path) {
		if (path[0] == ' ' || path[path_len - 1] == ' ') {
			return 0;
		} else {
			return 1;
		}
	} else {
		return 0;
	}
}/*}}}*/

static BCRYPT_ALG_HANDLE bcrypt_algo;
static BOOL has_bcrypt_algo = 0;

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#ifdef PHP_EXPORTS
BOOL php_win32_shutdown_random_bytes(void)
{/*{{{*/
	BOOL ret = TRUE;

	if (has_bcrypt_algo) {
		ret = NT_SUCCESS(BCryptCloseAlgorithmProvider(bcrypt_algo, 0));
		has_bcrypt_algo = 0;
	}

	return ret;
}/*}}}*/

BOOL php_win32_init_random_bytes(void)
{/*{{{*/
	if (has_bcrypt_algo) {
		return TRUE;
	}

	has_bcrypt_algo = NT_SUCCESS(BCryptOpenAlgorithmProvider(&bcrypt_algo, BCRYPT_RNG_ALGORITHM, NULL, 0));

	return has_bcrypt_algo;
}/*}}}*/
#endif

PHP_WINUTIL_API int php_win32_get_random_bytes(unsigned char *buf, size_t size)
{  /* {{{ */

	BOOL ret;

#if 0
	/* Currently we fail on startup, with CNG API it shows no regressions so far and is secure.
		Should switch on and try to reinit, if it fails too often on startup. This means also
		bringing locks back. */
	if (has_bcrypt_algo == 0) {
		return FAILURE;
	}
#endif

	/* No sense to loop here, the limit is huge enough. */
	ret = NT_SUCCESS(BCryptGenRandom(bcrypt_algo, buf, (ULONG)size, 0));

	return ret ? SUCCESS : FAILURE;
}
/* }}} */


/*
* This functions based on the code from the UNIXem project under
* the BSD like license. Modified for PHP by ab@php.net
*
* Home:    http://synesis.com.au/software/
*
* Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
*/

PHP_WINUTIL_API int php_win32_code_to_errno(unsigned long w32Err)
{/*{{{*/
    size_t  i;

    struct code_to_errno_map
    {
        unsigned long   w32Err;
        int             eerrno;
    };

    static const struct code_to_errno_map errmap[] =
    {
        /*   1 */       {   ERROR_INVALID_FUNCTION          ,   EINVAL          }
        /*   2 */   ,   {   ERROR_FILE_NOT_FOUND            ,   ENOENT          }
        /*   3 */   ,   {   ERROR_PATH_NOT_FOUND            ,   ENOENT          }
        /*   4 */   ,   {   ERROR_TOO_MANY_OPEN_FILES       ,   EMFILE          }
        /*   5 */   ,   {   ERROR_ACCESS_DENIED             ,   EACCES          }
        /*   6 */   ,   {   ERROR_INVALID_HANDLE            ,   EBADF           }
        /*   7 */   ,   {   ERROR_ARENA_TRASHED             ,   ENOMEM          }
        /*   8 */   ,   {   ERROR_NOT_ENOUGH_MEMORY         ,   ENOMEM          }
        /*   9 */   ,   {   ERROR_INVALID_BLOCK             ,   ENOMEM          }
        /*  10 */   ,   {   ERROR_BAD_ENVIRONMENT           ,   E2BIG           }
        /*  11 */   ,   {   ERROR_BAD_FORMAT                ,   ENOEXEC         }
        /*  12 */   ,   {   ERROR_INVALID_ACCESS            ,   EINVAL          }
        /*  13 */   ,   {   ERROR_INVALID_DATA              ,   EINVAL          }
        /*  14 */   ,   {   ERROR_OUTOFMEMORY               ,   ENOMEM          }
        /*  15 */   ,   {   ERROR_INVALID_DRIVE             ,   ENOENT          }
        /*  16 */   ,   {   ERROR_CURRENT_DIRECTORY         ,   ECURDIR         }
        /*  17 */   ,   {   ERROR_NOT_SAME_DEVICE           ,   EXDEV           }
        /*  18 */   ,   {   ERROR_NO_MORE_FILES             ,   ENOENT          }
        /*  19 */   ,   {   ERROR_WRITE_PROTECT             ,   EROFS           }
        /*  20 */   ,   {   ERROR_BAD_UNIT                  ,   ENXIO           }
        /*  21 */   ,   {   ERROR_NOT_READY                 ,   EBUSY           }
        /*  22 */   ,   {   ERROR_BAD_COMMAND               ,   EIO             }
        /*  23 */   ,   {   ERROR_CRC                       ,   EIO             }
        /*  24 */   ,   {   ERROR_BAD_LENGTH                ,   EIO             }
        /*  25 */   ,   {   ERROR_SEEK                      ,   EIO             }
        /*  26 */   ,   {   ERROR_NOT_DOS_DISK              ,   EIO             }
        /*  27 */   ,   {   ERROR_SECTOR_NOT_FOUND          ,   ENXIO           }
        /*  28 */   ,   {   ERROR_OUT_OF_PAPER              ,   EBUSY           }
        /*  29 */   ,   {   ERROR_WRITE_FAULT               ,   EIO             }
        /*  30 */   ,   {   ERROR_READ_FAULT                ,   EIO             }
        /*  31 */   ,   {   ERROR_GEN_FAILURE               ,   EIO             }
        /*  32 */   ,   {   ERROR_SHARING_VIOLATION         ,   EAGAIN          }
        /*  33 */   ,   {   ERROR_LOCK_VIOLATION            ,   EACCES          }
        /*  34 */   ,   {   ERROR_WRONG_DISK                ,   ENXIO           }
        /*  35 */   ,   {   35                              ,   ENFILE          }
        /*  36 */   ,   {   ERROR_SHARING_BUFFER_EXCEEDED   ,   ENFILE          }
        /*  37 */   ,   {   ERROR_HANDLE_EOF                ,   EINVAL          }
        /*  38 */   ,   {   ERROR_HANDLE_DISK_FULL          ,   ENOSPC          }
#if 0
        /*  39 */   ,   {   0                               ,   0               }
        /*  40 */   ,   {   0                               ,   0               }
        /*  41 */   ,   {   0                               ,   0               }
        /*  42 */   ,   {   0                               ,   0               }
        /*  43 */   ,   {   0                               ,   0               }
        /*  44 */   ,   {   0                               ,   0               }
        /*  45 */   ,   {   0                               ,   0               }
        /*  46 */   ,   {   0                               ,   0               }
        /*  47 */   ,   {   0                               ,   0               }
        /*  48 */   ,   {   0                               ,   0               }
        /*  49 */   ,   {   0                               ,   0               }
#endif
        /*  50 */   ,   {   ERROR_NOT_SUPPORTED             ,   ENOSYS          }
#if 0
        /*  51 */   ,   {   0                               ,   0               }
        /*  52 */   ,   {   0                               ,   0               }
#endif
        /*  53 */   ,   {   ERROR_BAD_NETPATH               ,   ENOENT          }
#if 0
        /*  54 */   ,   {   0                               ,   0               }
        /*  55 */   ,   {   0                               ,   0               }
        /*  56 */   ,   {   0                               ,   0               }
        /*  57 */   ,   {   0                               ,   0               }
        /*  58 */   ,   {   0                               ,   0               }
        /*  59 */   ,   {   0                               ,   0               }
        /*  60 */   ,   {   0                               ,   0               }
        /*  61 */   ,   {   0                               ,   0               }
        /*  62 */   ,   {   0                               ,   0               }
        /*  63 */   ,   {   0                               ,   0               }
        /*  64 */   ,   {   0                               ,   0               }
#endif
        /*  65 */   ,   {   ERROR_NETWORK_ACCESS_DENIED     ,   EACCES          }
#if 0
        /*  66 */   ,   {   0                               ,   0               }
#endif
        /*  67 */   ,   {   ERROR_BAD_NET_NAME              ,   ENOENT          }
#if 0
        /*  68 */   ,   {   0                               ,   0               }
        /*  69 */   ,   {   0                               ,   0               }
        /*  70 */   ,   {   0                               ,   0               }
        /*  71 */   ,   {   0                               ,   0               }
        /*  72 */   ,   {   0                               ,   0               }
        /*  73 */   ,   {   0                               ,   0               }
        /*  74 */   ,   {   0                               ,   0               }
        /*  75 */   ,   {   0                               ,   0               }
        /*  76 */   ,   {   0                               ,   0               }
        /*  77 */   ,   {   0                               ,   0               }
        /*  78 */   ,   {   0                               ,   0               }
        /*  79 */   ,   {   0                               ,   0               }
#endif
        /*  80 */   ,   {   ERROR_FILE_EXISTS               ,   EEXIST          }
#if 0
        /*  81 */   ,   {   0                               ,   0               }
#endif
        /*  82 */   ,   {   ERROR_CANNOT_MAKE               ,   EACCES          }
        /*  83 */   ,   {   ERROR_FAIL_I24                  ,   EACCES          }
#if 0
        /*  84 */   ,   {   0                               ,   0               }
        /*  85 */   ,   {   0                               ,   0               }
        /*  86 */   ,   {   0                               ,   0               }
#endif
        /*  87 */   ,   {   ERROR_INVALID_PARAMETER         ,   EINVAL          }
#if 0
        /*  88 */   ,   {   0                               ,   0               }
#endif
        /*  89 */   ,   {   ERROR_NO_PROC_SLOTS             ,   EAGAIN          }
#if 0
        /*  90 */   ,   {   0                               ,   0               }
        /*  91 */   ,   {   0                               ,   0               }
        /*  92 */   ,   {   0                               ,   0               }
        /*  93 */   ,   {   0                               ,   0               }
        /*  94 */   ,   {   0                               ,   0               }
        /*  95 */   ,   {   0                               ,   0               }
        /*  96 */   ,   {   0                               ,   0               }
        /*  97 */   ,   {   0                               ,   0               }
        /*  98 */   ,   {   0                               ,   0               }
        /*  99 */   ,   {   0                               ,   0               }
        /* 100 */   ,   {   0                               ,   0               }
        /* 101 */   ,   {   0                               ,   0               }
        /* 102 */   ,   {   0                               ,   0               }
        /* 103 */   ,   {   0                               ,   0               }
        /* 104 */   ,   {   0                               ,   0               }
        /* 105 */   ,   {   0                               ,   0               }
        /* 106 */   ,   {   0                               ,   0               }
        /* 107 */   ,   {   0                               ,   0               }
#endif
        /* 108 */   ,   {   ERROR_DRIVE_LOCKED              ,   EACCES          }
        /* 109 */   ,   {   ERROR_BROKEN_PIPE               ,   EPIPE           }
#if 0
        /* 110 */   ,   {   0                               ,   0               }
#endif
        /* 111 */   ,   {   ERROR_BUFFER_OVERFLOW           ,   ENAMETOOLONG    }
        /* 112 */   ,   {   ERROR_DISK_FULL                 ,   ENOSPC          }
#if 0
        /* 113 */   ,   {   0                               ,   0               }
#endif
        /* 114 */   ,   {   ERROR_INVALID_TARGET_HANDLE     ,   EBADF           }
#if 0
        /* 115 */   ,   {   0                               ,   0               }
        /* 116 */   ,   {   0                               ,   0               }
        /* 117 */   ,   {   0                               ,   0               }
        /* 118 */   ,   {   0                               ,   0               }
        /* 119 */   ,   {   0                               ,   0               }
        /* 120 */   ,   {   0                               ,   0               }
        /* 121 */   ,   {   0                               ,   0               }
#endif
        /* 122 */   ,   {   ERROR_INSUFFICIENT_BUFFER       ,   ERANGE          }
        /* 123 */   ,   {   ERROR_INVALID_NAME              ,   ENOENT          }
        /* 124 */   ,   {   ERROR_INVALID_HANDLE            ,   EINVAL          }
#if 0
        /* 125 */   ,   {   0                               ,   0               }
#endif
        /* 126 */   ,   {   ERROR_MOD_NOT_FOUND             ,   ENOENT          }
        /* 127 */   ,   {   ERROR_PROC_NOT_FOUND            ,   ENOENT          }
        /* 128 */   ,   {   ERROR_WAIT_NO_CHILDREN          ,   ECHILD          }
        /* 129 */   ,   {   ERROR_CHILD_NOT_COMPLETE        ,   ECHILD          }
        /* 130 */   ,   {   ERROR_DIRECT_ACCESS_HANDLE      ,   EBADF           }
        /* 131 */   ,   {   ERROR_NEGATIVE_SEEK             ,   EINVAL          }
        /* 132 */   ,   {   ERROR_SEEK_ON_DEVICE            ,   EACCES          }
#if 0
        /* 133 */   ,   {   0                               ,   0               }
        /* 134 */   ,   {   0                               ,   0               }
        /* 135 */   ,   {   0                               ,   0               }
        /* 136 */   ,   {   0                               ,   0               }
        /* 137 */   ,   {   0                               ,   0               }
        /* 138 */   ,   {   0                               ,   0               }
        /* 139 */   ,   {   0                               ,   0               }
        /* 140 */   ,   {   0                               ,   0               }
        /* 141 */   ,   {   0                               ,   0               }
        /* 142 */   ,   {   0                               ,   0               }
        /* 143 */   ,   {   0                               ,   0               }
        /* 144 */   ,   {   0                               ,   0               }
#endif
        /* 145 */   ,   {   ERROR_DIR_NOT_EMPTY             ,   ENOTEMPTY       }
#if 0
        /* 146 */   ,   {   0                               ,   0               }
        /* 147 */   ,   {   0                               ,   0               }
        /* 148 */   ,   {   0                               ,   0               }
        /* 149 */   ,   {   0                               ,   0               }
        /* 150 */   ,   {   0                               ,   0               }
        /* 151 */   ,   {   0                               ,   0               }
        /* 152 */   ,   {   0                               ,   0               }
        /* 153 */   ,   {   0                               ,   0               }
        /* 154 */   ,   {   0                               ,   0               }
        /* 155 */   ,   {   0                               ,   0               }
        /* 156 */   ,   {   0                               ,   0               }
        /* 157 */   ,   {   0                               ,   0               }
#endif
        /* 158 */   ,   {   ERROR_NOT_LOCKED                ,   EACCES          }
#if 0
        /* 159 */   ,   {   0                               ,   0               }
        /* 160 */   ,   {   0                               ,   0               }
#endif
        /* 161 */   ,   {   ERROR_BAD_PATHNAME              ,   ENOENT          }
#if 0
        /* 162 */   ,   {   0                               ,   0               }
        /* 163 */   ,   {   0                               ,   0               }
#endif
        /* 164 */   ,   {   ERROR_MAX_THRDS_REACHED         ,   EAGAIN          }
#if 0
        /* 165 */   ,   {   0                               ,   0               }
        /* 166 */   ,   {   0                               ,   0               }
#endif
        /* 167 */   ,   {   ERROR_LOCK_FAILED               ,   EACCES          }
#if 0
        /* 168 */   ,   {   0                               ,   0               }
        /* 169 */   ,   {   0                               ,   0               }
        /* 170 */   ,   {   0                               ,   0               }
        /* 171 */   ,   {   0                               ,   0               }
        /* 172 */   ,   {   0                               ,   0               }
        /* 173 */   ,   {   0                               ,   0               }
        /* 174 */   ,   {   0                               ,   0               }
        /* 175 */   ,   {   0                               ,   0               }
        /* 176 */   ,   {   0                               ,   0               }
        /* 177 */   ,   {   0                               ,   0               }
        /* 178 */   ,   {   0                               ,   0               }
        /* 179 */   ,   {   0                               ,   0               }
        /* 180 */   ,   {   0                               ,   0               }
        /* 181 */   ,   {   0                               ,   0               }
        /* 182 */   ,   {   0                               ,   0               }
#endif
        /* 183 */   ,   {   ERROR_ALREADY_EXISTS            ,   EEXIST          }
#if 0
        /* 184 */   ,   {   0                               ,   0               }
        /* 185 */   ,   {   0                               ,   0               }
        /* 186 */   ,   {   0                               ,   0               }
        /* 187 */   ,   {   0                               ,   0               }
        /* 188 */   ,   {   0                               ,   0               }
        /* 189 */   ,   {   0                               ,   0               }
        /* 190 */   ,   {   0                               ,   0               }
        /* 191 */   ,   {   0                               ,   0               }
        /* 192 */   ,   {   0                               ,   0               }
        /* 193 */   ,   {   0                               ,   0               }
        /* 194 */   ,   {   0                               ,   0               }
        /* 195 */   ,   {   0                               ,   0               }
        /* 196 */   ,   {   0                               ,   0               }
        /* 197 */   ,   {   0                               ,   0               }
        /* 198 */   ,   {   0                               ,   0               }
        /* 199 */   ,   {   0                               ,   0               }
#endif

        /* 206 */   ,   {   ERROR_FILENAME_EXCED_RANGE      ,   ENAMETOOLONG    }

        /* 215 */   ,   {   ERROR_NESTING_NOT_ALLOWED       ,   EAGAIN          }
		/* 258 */   ,   { WAIT_TIMEOUT, ETIME}

        /* 267 */   ,   {   ERROR_DIRECTORY                 ,   ENOTDIR         }
		/* 336 */   ,   {   ERROR_DIRECTORY_NOT_SUPPORTED   ,   EISDIR          }

        /* 996 */   ,   {   ERROR_IO_INCOMPLETE             ,   EAGAIN          }
        /* 997 */   ,   {   ERROR_IO_PENDING                ,   EAGAIN          }

        /* 1004 */   ,  {   ERROR_INVALID_FLAGS             ,   EINVAL          }
        /* 1113 */   ,  {   ERROR_NO_UNICODE_TRANSLATION    ,   EINVAL          }
        /* 1168 */   ,  {   ERROR_NOT_FOUND                 ,   ENOENT          }
        /* 1224 */   ,  {   ERROR_USER_MAPPED_FILE          ,   EACCES          }
        /* 1314 */   ,  {   ERROR_PRIVILEGE_NOT_HELD        ,   EACCES          }
        /* 1816 */  ,   {   ERROR_NOT_ENOUGH_QUOTA          ,   ENOMEM          }
					,   {   ERROR_ABANDONED_WAIT_0          ,   EIO }
		/* 1464 */	,	{	ERROR_SYMLINK_NOT_SUPPORTED		,	EINVAL			}
		/* 4390 */	,	{	ERROR_NOT_A_REPARSE_POINT		,	EINVAL			}
    };

    for(i = 0; i < sizeof(errmap)/sizeof(struct code_to_errno_map); ++i)
    {
        if(w32Err == errmap[i].w32Err)
        {
            return errmap[i].eerrno;
        }
    }

    assert(!"Unrecognised value");

    return EINVAL;
}/*}}}*/

PHP_WINUTIL_API char *php_win32_get_username(void)
{/*{{{*/
	wchar_t unamew[UNLEN + 1];
	size_t uname_len;
	char *uname;
	DWORD unsize = UNLEN;

	GetUserNameW(unamew, &unsize);
	uname = php_win32_cp_conv_w_to_any(unamew, unsize - 1, &uname_len);
	if (!uname) {
		return NULL;
	}

	/* Ensure the length doesn't overflow. */
	if (uname_len > UNLEN) {
		uname[uname_len] = '\0';
	}

	return uname;
}/*}}}*/

static zend_always_inline BOOL is_compatible(HMODULE handle, BOOL is_smaller, char *format, char **err)
{/*{{{*/
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER) handle;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((char *) dosHeader + dosHeader->e_lfanew);

	DWORD major = pNTHeader->OptionalHeader.MajorLinkerVersion;
	DWORD minor = pNTHeader->OptionalHeader.MinorLinkerVersion;

#if PHP_LINKER_MAJOR == 14
	/* VS 2015, 2017, 2019 and 2022 are binary compatible, but only forward compatible.
		It should be fine, if we load a module linked with an older one into
		the core linked with the newer one, but not the otherway round.
		Analogously, it should be fine, if a PHP build linked with an older version
		is used with a newer CRT, but not the other way round.
		Otherwise, if the linker major version is not same, it is an error, as
		per the current knowledge.

		This check is to be extended as new VS versions come out. */
	DWORD core_minor = (DWORD)(PHP_LINKER_MINOR/10);
	DWORD comp_minor = (DWORD)(minor/10);
	if (14 == major && (is_smaller ? core_minor < comp_minor : core_minor > comp_minor) || PHP_LINKER_MAJOR != major)
#else
	if (PHP_LINKER_MAJOR != major)
#endif
	{
		char buf[MAX_PATH];
		if (GetModuleFileName(handle, buf, sizeof(buf)) != 0) {
			spprintf(err, 0, format, buf, major, minor, PHP_LINKER_MAJOR, PHP_LINKER_MINOR);
		} else {
			spprintf(err, 0, "Can't retrieve the module name (error %u)", GetLastError());
		}
		return FALSE;
	}

	return TRUE;
}/*}}}*/

PHP_WINUTIL_API BOOL php_win32_image_compatible(HMODULE handle, char **err)
{/*{{{*/
	return is_compatible(handle, TRUE, "Can't load module '%s' as it's linked with %u.%u, but the core is linked with %d.%d", err);
}/*}}}*/

/* Expect a CRT module handle */
PHP_WINUTIL_API BOOL php_win32_crt_compatible(char **err)
{/*{{{*/
#if PHP_LINKER_MAJOR == 14
	/* Extend for other CRT if needed. */
# if PHP_DEBUG
	const char *crt_name = "vcruntime140d.dll";
# else
	const char *crt_name = "vcruntime140.dll";
# endif
	HMODULE handle = GetModuleHandle(crt_name);
	if (handle == NULL) {
		spprintf(err, 0, "Can't get handle of module %s (error %u)", crt_name, GetLastError());
		return FALSE;
	}
	return is_compatible(handle, FALSE, "'%s' %u.%u is not compatible with this PHP build linked with %d.%d", err);
#endif
	return TRUE;
}/*}}}*/
