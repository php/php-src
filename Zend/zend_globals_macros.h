/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_GLOBALS_MACROS_H
#define ZEND_GLOBALS_MACROS_H

#include "zend_portability.h"

typedef struct _zend_compiler_globals zend_compiler_globals;
typedef struct _zend_executor_globals zend_executor_globals;
typedef struct _zend_php_scanner_globals zend_php_scanner_globals;
typedef struct _zend_ini_scanner_globals zend_ini_scanner_globals;

#ifdef ZEND_WIN32
# define ZEND_TLS_API
# ifdef LIBZEND_EXPORTS
#  define ZEND_TLS_DIRECT 1
# endif
#else
# define ZEND_TLS_API ZEND_API
# define ZEND_TLS_DIRECT 1
#endif

BEGIN_EXTERN_C()

/* Compiler */
#ifdef ZTS
# ifdef ZEND_TLS_DIRECT
extern ZEND_TLS_API TSRM_TLS TSRM_TLS_MODEL_ATTR zend_compiler_globals compiler_globals_tls;
#  define CG(v) (compiler_globals_tls.v)
# else
#  define CG(v) ZEND_TSRMG(compiler_globals_id, zend_compiler_globals *, v)
# endif
#else
# define CG(v) (compiler_globals.v)
extern ZEND_API struct _zend_compiler_globals compiler_globals;
#endif
ZEND_API int zendparse(void);


/* Executor */
#ifdef ZTS
# ifdef ZEND_TLS_DIRECT
extern ZEND_TLS_API TSRM_TLS TSRM_TLS_MODEL_ATTR zend_executor_globals executor_globals_tls;
#  define EG(v) (executor_globals_tls.v)
# else
#  define EG(v) ZEND_TSRMG(executor_globals_id, zend_executor_globals *, v)
# endif
#else
# define EG(v) (executor_globals.v)
extern ZEND_API zend_executor_globals executor_globals;
#endif

/* Language Scanner */
#ifdef ZTS
# define LANG_SCNG(v) ZEND_TSRMG_FAST(language_scanner_globals_offset, zend_php_scanner_globals *, v)
extern ZEND_API ts_rsrc_id language_scanner_globals_id;
extern ZEND_API size_t language_scanner_globals_offset;
#else
# define LANG_SCNG(v) (language_scanner_globals.v)
extern ZEND_API zend_php_scanner_globals language_scanner_globals;
#endif


/* INI Scanner */
#ifdef ZTS
# define INI_SCNG(v) ZEND_TSRMG_FAST(ini_scanner_globals_offset, zend_ini_scanner_globals *, v)
extern ZEND_API ts_rsrc_id ini_scanner_globals_id;
extern ZEND_API size_t ini_scanner_globals_offset;
#else
# define INI_SCNG(v) (ini_scanner_globals.v)
extern ZEND_API zend_ini_scanner_globals ini_scanner_globals;
#endif

END_EXTERN_C()

#endif /* ZEND_GLOBALS_MACROS_H */
