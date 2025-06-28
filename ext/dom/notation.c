/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "dom_properties.h"

/*
* class DOMNotation extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-5431D1B9
* Since:
*/

/* {{{ publicId	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-54F2B4D0
Since:
*/
zend_result dom_notation_public_id_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlEntityPtr, nodep, obj);

	if (nodep->ExternalID) {
		ZVAL_STRING(retval, (char *) (nodep->ExternalID));
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ systemId	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-E8AAB1D0
Since:
*/
zend_result dom_notation_system_id_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlEntityPtr, nodep, obj);

	if (nodep->SystemID) {
		ZVAL_STRING(retval, (char *) (nodep->SystemID));
	} else {
		ZVAL_EMPTY_STRING(retval);
	}

	return SUCCESS;
}

/* }}} */

#endif
