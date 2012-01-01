/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
  |          Stig Sæther Bakken <ssb@php.net>                            |
  |          David Sklar <sklar@student.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#define NO_REGEX_EXTRA_H

#ifdef WIN32
#include <stddef.h>
#endif

#ifdef NETWARE
#include <netinet/in.h>
#endif

#include "zend.h"
#include "ext/ereg/php_regex.h"
#include "php_compat.h"

#ifdef HAVE_OPENSSL_EXT
/* zlib typedefs free_func which causes problems if the SSL includes happen
 * after zlib.h is included */
# include <openssl/ssl.h>
#endif

#ifdef regex_t
#undef regex_t
#endif

#include "httpd.h"
#include "http_config.h"

#if MODULE_MAGIC_NUMBER > 19980712
# include "ap_compat.h"
#else
# if MODULE_MAGIC_NUMBER > 19980324
#  include "compat.h"
# endif
#endif

#include "http_core.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_log.h"   
#include "util_script.h"

#include "php_variables.h"
#include "php_main.h"     
#include "php_ini.h"
#include "ext/standard/php_standard.h"

#include "mod_php5.h"
