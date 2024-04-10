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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "namespace_compat.h"

/* https://dom.spec.whatwg.org/#dom-htmlcollection-nameditem-key */
PHP_METHOD(DOM_HTMLCollection, namedItem)
{
	zend_string *key;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(key)
	ZEND_PARSE_PARAMETERS_END();

	/* 1. If key is the empty string, return null. */
	if (ZSTR_LEN(key) == 0) {
		RETURN_NULL();
	}

	dom_object *intern = Z_DOMOBJ_P(ZEND_THIS);
	dom_nnodemap_object *objmap = intern->ptr;

	/* 2. Return the first element in the collection for which at least one of the following is true: */
	xmlNodePtr basep = dom_object_get_node(objmap->baseobj);
	if (basep != NULL) {
		int cur = 0;
		int next = cur;
		xmlNodePtr candidate = basep->children;
		while (candidate != NULL) {
			candidate = dom_get_elements_by_tag_name_ns_raw(basep, candidate, objmap->ns, objmap->local, objmap->local_lower, &cur, next);
			if (candidate == NULL) {
				break;
			}

			xmlAttrPtr attr;

			/* it has an ID which is key; */
			if ((attr = xmlHasNsProp(candidate, BAD_CAST "id", NULL)) != NULL && dom_compare_value(attr, BAD_CAST ZSTR_VAL(key))) {
				DOM_RET_OBJ(candidate, objmap->baseobj);
				return;
			}
			/* it is in the HTML namespace and has a name attribute whose value is key; */
			else if (php_dom_ns_is_fast(candidate, php_dom_ns_is_html_magic_token)) {
				if ((attr = xmlHasNsProp(candidate, BAD_CAST "name", NULL)) != NULL && dom_compare_value(attr, BAD_CAST ZSTR_VAL(key))) {
					DOM_RET_OBJ(candidate, objmap->baseobj);
					return;
				}
			}

			next = cur + 1;
		}
	}
}

#endif
