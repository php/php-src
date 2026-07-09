/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the world-wide-web at the following url:                     |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Liam Hammett                                                 |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_HTML_EXT_H
#define PHP_HTML_EXT_H

#include "php.h"

extern zend_module_entry html_module_entry;
#define phpext_html_ptr &html_module_entry

#define PHP_HTML_VERSION PHP_VERSION

/* Request-scoped userland hooks that let a framework take over how a component
 * tag is turned into an object (RFC §4). `component_factories` replace the `new`
 * for a component (e.g. resolve through a DI container) — the *production* seam.
 * `component_decorators` are the cross-cutting seam: they run on the produced
 * Html\Htmlable of every component, so output transforms / wrapping / logging
 * apply uniformly. Each is an ordered list of html_handler entries (callable +
 * optional per-component scope) tried in registration order, like
 * spl_autoload_register; a factory returns an object to take over the dispatch
 * or null to defer, while each decorator wraps the previous one's result. A
 * scoped entry is skipped in C — before any userland call — for every other
 * component. Both are NULL until first registered and are cleared at request
 * shutdown. */
ZEND_BEGIN_MODULE_GLOBALS(html)
	HashTable *component_factories;
	HashTable *component_decorators;
ZEND_END_MODULE_GLOBALS(html)

ZEND_EXTERN_MODULE_GLOBALS(html)

#define HTML_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(html, v)

#endif /* PHP_HTML_EXT_H */
