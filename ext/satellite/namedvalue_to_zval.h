/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id$
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

#ifndef __namedvalue_to_zval_h__
#define __namedvalue_to_zval_h__

#include <zend.h>
#include <orb/orbit.h>

zend_bool orbit_namedvalue_to_zval(
		const CORBA_NamedValue * pSource, zval * pDestination);

zend_bool satellite_any_to_zval(
		const CORBA_any * pSource, zval * pDestination);

#endif /* __namedvalue_to_zval_h__ */
