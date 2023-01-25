/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"

ZEND_API ZEND_COLD ZEND_NORETURN void _zend_bailout(const char *filename,
                                                    uint32_t lineno) /* {{{ */
{
  // Not implemented yet, as WebAssembly has no stack switching
  // feature implementation at this point.
	zend_throw_exception(NULL, "Exception bailout is not supported yet on this platform (wasm32-wasi)", 0);
}
/* }}} */
