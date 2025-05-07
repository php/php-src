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
   | Authors: Bogdan Ungureanu <bogdanungureanu21@gmail.com>              |
   +----------------------------------------------------------------------+
*/

#ifndef LISTFORMATTER_DATA_H
#define LISTFORMATTER_DATA_H

#include <php.h>

#include <unicode/ulistformatter.h>

#include "intl_error.h"

typedef struct {
    // error handling
    intl_error      error;

    // formatter handling
    UListFormatter*  ulistfmt;
} listformatter_data;

void listformatter_data_init( listformatter_data* lf_data );
void listformatter_data_free( listformatter_data* lf_data );

#endif // LISTFORMATTER_DATA_H 