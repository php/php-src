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
*/

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
#include "url.h"
#include "pageinfo.h"
#include "fsock.h"
#include "php_image.h"
#include "info.h"
#include "php_var.h"
#include "quot_print.h"
#include "dl.h"
#include "php_crypt.h"
#include "head.h"
#include "php_output.h"
#include "php_array.h"
#include "php_assert.h"
#include "php_versioning.h"
#include "php_password.h"

#include "php_version.h"
#define PHP_STANDARD_VERSION PHP_VERSION

#define phpext_standard_ptr basic_functions_module_ptr
PHP_MINIT_FUNCTION(standard_filters);
PHP_MSHUTDOWN_FUNCTION(standard_filters);
