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
   | Authors: Hans-Peter Oeri (University of St.Gallen) <hp@oeri.ch>      |
   +----------------------------------------------------------------------+
 */

#ifndef RESOURCEBUNDLE_H
#define RESOURCEBUNDLE_H

#include <unicode/ures.h>

#include <zend.h>

#include "resourcebundle/resourcebundle_class.h"

#ifdef __cplusplus
extern "C" {
#endif
void resourcebundle_extract_value( zval *target, ResourceBundle_object *source);
#ifdef __cplusplus
}
#endif

#endif // #ifndef RESOURCEBUNDLE_CLASS_H
