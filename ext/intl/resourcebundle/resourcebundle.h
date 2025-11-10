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
   | Authors: Hans-Peter Oeri (University of St.Gallen) <hp@oeri.ch>      |
   +----------------------------------------------------------------------+
 */

#ifndef RESOURCEBUNDLE_H
#define RESOURCEBUNDLE_H

#include <unicode/ures.h>

#include <zend.h>

#include "resourcebundle/resourcebundle_class.h"

void resourcebundle_extract_value( zval *target, ResourceBundle_object *source);

#endif // #ifndef RESOURCEBUNDLE_CLASS_H
