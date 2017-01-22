/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2017 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:                                                              |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "basic_functions.h"
#include "php_math.h"
#include "php_string.h"
#include "base64.h"
#include "php_dir.h"
#include "php_dns.h"
#include "php_mail.h"
#include "md5.h"
#include "sha1.h"
#include "html.h"
#include "exec.h"
#include "file.h"
#include "php_ext_syslog.h"
#include "php_filestat.h"
#include "php_browscap.h"
#include "pack.h"
#include "datetime.h"
#include "microtime.h"
#include "url.h"
#include "pageinfo.h"
#include "cyr_convert.h"
#include "php_link.h"
#include "fsock.h"
#include "php_image.h"
#include "php_iptc.h"
#include "info.h"
#include "uniqid.h"
#include "php_var.h"
#include "quot_print.h"
#include "dl.h"
#include "php_crypt.h"
#include "head.h"
#include "php_lcg.h"
#include "php_metaphone.h"
#include "php_output.h"
#include "php_array.h"
#include "php_assert.h"
#include "php_versioning.h"
#include "php_ftok.h"
#include "php_type.h"
#include "php_password.h"
#include "php_random.h"

#include "php_version.h"
#define PHP_STANDARD_VERSION PHP_VERSION

#define phpext_standard_ptr basic_functions_module_ptr
PHP_MINIT_FUNCTION(standard_filters);
PHP_MSHUTDOWN_FUNCTION(standard_filters);


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
