/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "spoofchecker_class.h"
#include "spoofchecker.h"

#include <unicode/uspoof.h>


/* {{{ spoofchecker_register_constants
 * Register constants
 */
void spoofchecker_register_constants(INIT_FUNC_ARGS)
{
	if (!Spoofchecker_ce_ptr)
	{
		zend_error(E_ERROR, "Spoofchecker class not defined");
		return;
	}

	#define SPOOFCHECKER_EXPOSE_CLASS_CONST(x) zend_declare_class_constant_long(Spoofchecker_ce_ptr, ZEND_STRS( #x ) - 1, USPOOF_##x);

	SPOOFCHECKER_EXPOSE_CLASS_CONST(SINGLE_SCRIPT_CONFUSABLE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(MIXED_SCRIPT_CONFUSABLE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(WHOLE_SCRIPT_CONFUSABLE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(ANY_CASE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(SINGLE_SCRIPT)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(INVISIBLE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(CHAR_LIMIT)

#if U_ICU_VERSION_MAJOR_NUM >= 58
	SPOOFCHECKER_EXPOSE_CLASS_CONST(ASCII)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(HIGHLY_RESTRICTIVE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(MODERATELY_RESTRICTIVE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(MINIMALLY_RESTRICTIVE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(UNRESTRICTIVE)
	SPOOFCHECKER_EXPOSE_CLASS_CONST(SINGLE_SCRIPT_RESTRICTIVE)
#endif

	#undef SPOOFCHECKER_EXPOSE_CLASS_CONST
}
/* }}} */
