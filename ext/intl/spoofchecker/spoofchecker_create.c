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

#include "php_intl.h"
#include "spoofchecker_class.h"
#include "spoofchecker_create.h"
#include "intl_data.h"

/* {{{ proto Spoofchecker Spoofchecker::__construct()
 * Spoofchecker object constructor.
 */
PHP_METHOD(Spoofchecker, __construct)
{
	int checks;
	SPOOFCHECKER_METHOD_INIT_VARS;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	SPOOFCHECKER_METHOD_FETCH_OBJECT_NO_CHECK;

	co->uspoof = uspoof_open(SPOOFCHECKER_ERROR_CODE_P(co));
	INTL_CTOR_CHECK_STATUS(co, "spoofchecker: unable to open ICU Spoof Checker");

	/* Single-script enforcement is on by default. This fails for languages
	 like Japanese that legally use multiple scripts within a single word,
	 so we turn it off.
	*/
	checks = uspoof_getChecks(co->uspoof, SPOOFCHECKER_ERROR_CODE_P(co));
	uspoof_setChecks(co->uspoof, checks & ~USPOOF_SINGLE_SCRIPT, SPOOFCHECKER_ERROR_CODE_P(co));
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
