/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Scott MacVicar <scottmac@php.net>                           |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php_intl.h"
#include "spoofchecker_class.h"
#include "intl_data.h"

/* {{{ Spoofchecker object constructor. */
PHP_METHOD(Spoofchecker, __construct)
{
#if U_ICU_VERSION_MAJOR_NUM < 58
	int checks;
#endif
	SPOOFCHECKER_METHOD_INIT_VARS;

	ZEND_PARSE_PARAMETERS_NONE();

	SPOOFCHECKER_METHOD_FETCH_OBJECT_NO_CHECK;

	co->uspoof = uspoof_open(SPOOFCHECKER_ERROR_CODE_P(co));
	// TODO Throw exception directly
	bool old_use_exception = INTL_G(use_exceptions);
	int old_error_level = INTL_G(error_level);
	INTL_G(use_exceptions) = true;
	INTL_G(error_level) = 0;
	INTL_METHOD_CHECK_STATUS(co, "unable to open ICU Spoof Checker");
	INTL_G(use_exceptions) = old_use_exception;
	INTL_G(error_level) = old_error_level;

#if U_ICU_VERSION_MAJOR_NUM >= 58
	/* TODO save it into the object for further suspiction check comparison. */
	/* ICU 58 removes WSC and MSC handling. However there are restriction
	 levels as defined in
	 http://www.unicode.org/reports/tr39/tr39-15.html#Restriction_Level_Detection
	 and the default is high restrictive. In further, we might want to utilize
	 uspoof_check2 APIs when it became stable, to use extended check result APIs.
	 Subsequent changes in the unicode security algos are to be watched.*/
	uspoof_setRestrictionLevel(co->uspoof, SPOOFCHECKER_DEFAULT_RESTRICTION_LEVEL);
	co->uspoofres = uspoof_openCheckResult(SPOOFCHECKER_ERROR_CODE_P(co));
#else
	/* Single-script enforcement is on by default. This fails for languages
	 like Japanese that legally use multiple scripts within a single word,
	 so we turn it off.
	*/
	checks = uspoof_getChecks(co->uspoof, SPOOFCHECKER_ERROR_CODE_P(co));
	uspoof_setChecks(co->uspoof, checks & ~USPOOF_SINGLE_SCRIPT, SPOOFCHECKER_ERROR_CODE_P(co));
#endif
}
/* }}} */
