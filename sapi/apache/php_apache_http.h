#define NO_REGEX_EXTRA_H

#ifdef WIN32
#include <stddef.h>
#endif

#ifdef NETWARE
#include <netinet/in.h>
#endif

#include "zend.h"
#include "php_regex.h"
#include "php_compat.h"

#if HAVE_OPENSSL_EXT
/* zlib typedefs free_func which causes problems if the SSL includes happen
 * after zlib.h is included */
# include <openssl/ssl.h>
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

#include "mod_php4.h"
