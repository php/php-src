/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "basic_functions.h"
#include "phpmath.h"
#include "php3_string.h"
#include "base64.h"
#include "php3_dir.h"
#include "dns.h"
#include "reg.h"
#include "php3_mail.h"
#include "md5.h"
#include "html.h"
#include "exec.h"
#include "file.h"
#include "php3_syslog.h"
#include "php3_filestat.h"
#include "php3_browscap.h"
#include "pack.h"
#include "datetime.h"
#include "microtime.h"
#include "url.h"
#include "pageinfo.h"
#include "cyr_convert.h"
#include "php3_link.h"
#include "fsock.h"
#include "php_image.h"
#include "php3_iptc.h"
#include "info.h"
#include "uniqid.h"
#include "php3_var.h"
#include "quot_print.h"
#include "type.h"
#include "dl.h"
#include "php3_crypt.h"
#include "head.h"
#include "post.h"

#define phpext_standard_ptr basic_functions_module_ptr

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
