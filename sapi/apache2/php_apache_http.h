# if HAVE_AP_CONFIG_H
#include "ap_config_auto.h"
#ifdef RHAPSODY
#undef HAVE_SNPRINTF
#endif
#include "ap_config.h"
#ifdef RHAPSODY
#undef HAVE_SNPRINTF   
#define HAVE_SNPRINTF 1
#endif 
# endif
# if HAVE_OLD_COMPAT_H
#include "compat.h"
# endif
# if HAVE_AP_COMPAT_H 
#include "ap_compat.h"
# endif
#include "httpd.h"
#include "http_main.h"    
#include "http_core.h"    
#include "http_request.h" 
#include "http_protocol.h"
#include "http_config.h"
#include "http_log.h"
