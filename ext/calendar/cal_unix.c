/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shane Caraveo             <shane@caraveo.com>               | 
   |          Colin Viebrock            <colin@easydns.com>               |
   |          Hartmut Holzgraefe        <hholzgra@php.net>                |
   +----------------------------------------------------------------------+
 */
/* $Id: */

#include "php.h"
#include "php_calendar.h"
#include "sdncal.h"
#include <time.h>

/* {{{ proto int unixtojd([int timestamp])
   Convert UNIX timestamp to Julian Day */
PHP_FUNCTION(unixtojd)
{
  zval *timestamp;
  long jdate; 
  time_t t;
  struct tm *ta, tmbuf;
  int myargc=ZEND_NUM_ARGS();
	
  if ((myargc > 1) || (zend_get_parameters(ht, myargc, &timestamp) != SUCCESS)) {
    WRONG_PARAM_COUNT;
  }

  if(myargc==1) {
    convert_to_long(timestamp);
    t = Z_LVAL_P(timestamp);
  } else {
    t = time(NULL);
  }

  if(t < 0) {
	RETURN_FALSE;
  }

  ta = php_localtime_r(&t, &tmbuf);
  if (!ta) {
	  RETURN_FALSE;
  }

  jdate = GregorianToSdn(ta->tm_year+1900, ta->tm_mon+1, ta->tm_mday);
  
  RETURN_LONG(jdate);
}
/* }}} */

/* {{{ proto int jdtounix(int jday)
   Convert Julian Day to UNIX timestamp */
PHP_FUNCTION(jdtounix)
{
  zval *jday;
  long uday;

  if ((ZEND_NUM_ARGS()!= 1) || (zend_get_parameters(ht, 1, &jday) != SUCCESS)) {
    WRONG_PARAM_COUNT;
  }
  
  convert_to_long(jday);

  uday = Z_LVAL_P(jday) - 2440588 /* J.D. of 1.1.1970 */;
  
  if(uday<0)     RETURN_FALSE; /* before beginning of unix epoch */ 
  if(uday>24755) RETURN_FALSE; /* behind end of unix epoch */

  RETURN_LONG(uday*24*3600);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
