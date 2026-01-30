/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend by Perforce and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

#include "zend.h"
#include "zend_API.h"
#include "zend_attributes.h"
#include "zend_builtin_functions.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_closures.h"
#include "zend_generators.h"
#include "zend_weakrefs.h"
#include "zend_enum.h"
#include "zend_fibers.h"

ZEND_API void zend_register_default_classes(void)
{
	zend_register_interfaces();
	zend_register_default_exception();
	zend_register_iterator_wrapper();
	zend_register_closure_ce();
	zend_register_generator_ce();
	zend_register_weakref_ce();
	zend_register_attribute_ce();
	zend_register_enum_ce();
	zend_register_fiber_ce();
}
