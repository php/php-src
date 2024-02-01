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
   | Authors: Ed Batutis <ed@batutis.com>                                 |
   +----------------------------------------------------------------------+
 */

#include "normalizer.h"
#include "normalizer_class.h"
#include "php_intl.h"
#include "normalizer_arginfo.h"
#include "intl_error.h"

#include <unicode/unorm.h>

zend_class_entry *Normalizer_ce_ptr = NULL;

/*
 * 'Normalizer' class registration structures & functions
 */

/* {{{ normalizer_register_Normalizer_class
 * Initialize 'Normalizer' class
 */
void normalizer_register_Normalizer_class( void )
{
	/* Create and register 'Normalizer' class. */
	Normalizer_ce_ptr = register_class_Normalizer();
	Normalizer_ce_ptr->create_object = NULL;
}
/* }}} */
