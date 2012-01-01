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
   | Author: David Hedbor <neotron@php.net>                               |
   | Based on aolserver SAPI by Sascha Schumann <sascha@schumann.cx>      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#ifdef HAVE_ROXEN

#include "php_ini.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_main.h" 
#include "ext/standard/info.h"

#include "php_version.h"

#ifndef ZTS
/* Only valid if thread safety is enabled. */
#undef ROXEN_USE_ZTS
#endif


/* Pike Include Files 
 *
 * conflicts with pike avoided by only using long names. Requires a new
 * Pike 0.7 since it was implemented for this interface only.
 *
 */
#define NO_PIKE_SHORTHAND

#include <fdlib.h>
#include <program.h>
#include <pike_types.h>
#include <interpret.h>
#include <module_support.h>
#include <error.h>
#include <array.h>
#include <backend.h>
#include <stralloc.h>
#include <mapping.h>
#include <object.h>
#include <threads.h>
#include <builtin_functions.h>
#include <operators.h>

#undef HIDE_GLOBAL_VARIABLES
#undef REVEAL_GLOBAL_VARIABLES
#define HIDE_GLOBAL_VARIABLES()
#define REVEAL_GLOBAL_VARIABLES()

/* php_roxen_request is per-request object storage */

typedef struct
{
  struct mapping *request_data;
  struct object *my_fd_obj;
  int my_fd;
  char *filename;
} php_roxen_request;


/* Defines to get to the data supplied when the script is started. */

#ifdef ROXEN_USE_ZTS

/* ZTS does work now, but it seems like it's faster using the "serialization"
 * method I previously used. Thus it's not used unless ROXEN_USE_ZTS is defined.
 */

/* Per thread storage area id... */
static int roxen_globals_id;

# define GET_THIS() php_roxen_request *_request = ts_resource(roxen_globals_id)
# define THIS _request
#else
static php_roxen_request *current_request = NULL;

# define GET_THIS() current_request = ((php_roxen_request *)Pike_fp->current_storage)
# define THIS current_request
#endif

/* File descriptor integer. Used to write directly to the FD without 
 * passing Pike
 */
#define MY_FD    (THIS->my_fd)

/* FD object. Really a PHPScript object from Pike which implements a couple
 * of functions to handle headers, writing and buffering.
 */
#define MY_FD_OBJ        ((struct object *)(THIS->my_fd_obj))

/* Mapping with data supplied from the calling Roxen module. Contains
 * a mapping with headers, an FD object etc.
 */
#define REQUEST_DATA ((struct mapping *)(THIS->request_data))


#if defined(_REENTRANT) && !defined(ROXEN_USE_ZTS)
/* Lock used to serialize the PHP execution. If ROXEN_USE_ZTS is defined, we
 * are using the PHP thread safe mechanism instead.
 */
static PIKE_MUTEX_T roxen_php_execution_lock;
# define PHP_INIT_LOCK()	mt_init(&roxen_php_execution_lock)
# define PHP_LOCK(X)    THREADS_ALLOW();mt_lock(&roxen_php_execution_lock);THREADS_DISALLOW()
# define PHP_UNLOCK(X)	mt_unlock(&roxen_php_execution_lock);
# define PHP_DESTROY()	mt_destroy(&roxen_php_execution_lock)
#else /* !_REENTRANT */
# define PHP_INIT_LOCK()	
# define PHP_LOCK(X)
# define PHP_UNLOCK(X)
# define PHP_DESTROY()	
#endif /* _REENTRANT */

extern int fd_from_object(struct object *o);
static unsigned char roxen_php_initialized;

/* This allows calling of pike functions from the PHP callbacks,
 * which requires the Pike interpreter to be locked.
 */
#define THREAD_SAFE_RUN(COMMAND, what)  do {\
  struct thread_state *state;\
 if((state = thread_state_for_id(th_self()))!=NULL) {\
    if(!state->swapped) {\
      COMMAND;\
    } else {\
      mt_lock(&interpreter_lock);\
      SWAP_IN_THREAD(state);\
      COMMAND;\
      SWAP_OUT_THREAD(state);\
      mt_unlock(&interpreter_lock);\
    }\
  }\
} while(0)

struct program *php_program;


/* To avoid executing a PHP script from a PHP callback, which would
 * create a deadlock, a global thread id is used. If the thread calling the
 * php-script is the same as the current thread, it fails. 
 */
static int current_thread = -1;


/* Low level header lookup. Basically looks for the named header in the mapping
 * headers in the supplied options mapping.
 */
 
static INLINE struct svalue *lookup_header(char *headername)
{
  struct svalue *headers, *value;
  struct pike_string *sind;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif
  sind = make_shared_string("env");
  headers = low_mapping_string_lookup(REQUEST_DATA, sind);
  free_string(sind);
  if(!headers || headers->type != PIKE_T_MAPPING) return NULL;
  sind = make_shared_string(headername);
  value = low_mapping_string_lookup(headers->u.mapping, sind);
  free_string(sind);
  if(!value) return NULL;
  return value;
}

/* Lookup a header in the mapping and return the value as a string, or
 * return the default if it's missing
 */
INLINE static char *lookup_string_header(char *headername, char *default_value)
{
  struct svalue *head = NULL;
  THREAD_SAFE_RUN(head = lookup_header(headername), "header lookup");
  if(!head || head->type != PIKE_T_STRING)
    return default_value;
  return head->u.string->str;
}

/* Lookup a header in the mapping and return the value as if it's an integer
 * and otherwise return the default.
 */
INLINE static int lookup_integer_header(char *headername, int default_value)
{
  struct svalue *head = NULL;
  THREAD_SAFE_RUN(head = lookup_header(headername), "header lookup");
  if(!head || head->type != PIKE_T_INT)
    return default_value;
  return head->u.integer;
}

/*
 * php_roxen_low_ub_write() writes data to the client connection. Might be
 * rewritten to do more direct IO to save CPU and the need to lock the *
 * interpreter for better threading.
 */

static int
php_roxen_low_ub_write(const char *str, uint str_length TSRMLS_DC) {
  int sent_bytes = 0;
  struct pike_string *to_write = NULL;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif

  if(!MY_FD_OBJ->prog) {
    PG(connection_status) = PHP_CONNECTION_ABORTED;
    zend_bailout();
    return -1;
  }
  to_write = make_shared_binary_string(str, str_length);
  push_string(to_write);
  safe_apply(MY_FD_OBJ, "write", 1);
  if(Pike_sp[-1].type == PIKE_T_INT)
    sent_bytes = Pike_sp[-1].u.integer;
  pop_stack();
  if(sent_bytes != str_length) {
    /* This means the connection is closed. Dead. Gone. *sniff*  */
    php_handle_aborted_connection();
  }
  return sent_bytes;
}

/*
 * php_roxen_sapi_ub_write() calls php_roxen_low_ub_write in a Pike thread
 * safe manner.
 */

static int
php_roxen_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif

  int sent_bytes = 0, fd = MY_FD;
  if(fd)
  {
    for(sent_bytes=0;sent_bytes < str_length;)
    {
      int written;
      written = fd_write(fd, str + sent_bytes, str_length - sent_bytes);
      if(written < 0)
      {
	switch(errno)
	{
	 default:
	  /* This means the connection is closed. Dead. Gone. *sniff*  */
	  PG(connection_status) = PHP_CONNECTION_ABORTED;
	  zend_bailout();
	  return sent_bytes;
	 case EINTR: 
	 case EWOULDBLOCK:
	  continue;
	}

      } else {
	sent_bytes += written;
      }
    }
  } else {
    THREAD_SAFE_RUN(sent_bytes = php_roxen_low_ub_write(str, str_length TSRMLS_CC),
		    "write");
  }
  return sent_bytes;
}

/* php_roxen_set_header() sets a header in the header mapping. Called in a
 * thread safe manner from php_roxen_sapi_header_handler.
 */
static void php_roxen_set_header(char *header_name, char *value, char *p)
{
  struct svalue hsval;
  struct pike_string *hval, *ind, *hind;
  struct mapping *headermap;
  struct svalue *s_headermap;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif
  hval = make_shared_string(value);
  ind = make_shared_string(" _headers");
  hind = make_shared_binary_string(header_name,
				   (int)(p - header_name));

  s_headermap = low_mapping_string_lookup(REQUEST_DATA, ind);
  if(!s_headermap)
  {
    struct svalue mappie;                                           
    mappie.type = PIKE_T_MAPPING;
    headermap = allocate_mapping(1);
    mappie.u.mapping = headermap;
    mapping_string_insert(REQUEST_DATA, ind, &mappie);
    free_mapping(headermap);
  } else
    headermap = s_headermap->u.mapping;

  hsval.type = PIKE_T_STRING;
  hsval.u.string = hval;
  mapping_string_insert(headermap, hind, &hsval);

  free_string(hval);
  free_string(ind);
  free_string(hind);
}

/*
 * php_roxen_sapi_header_handler() sets a HTTP reply header to be 
 * sent to the client.
 */
static int
php_roxen_sapi_header_handler(sapi_header_struct *sapi_header,
			      sapi_headers_struct *sapi_headers TSRMLS_DC)
{
  char *header_name, *header_content, *p;
  header_name = sapi_header->header;
  header_content = p = strchr(header_name, ':');
  
  if(p) {
  do {
    header_content++;
  } while(*header_content == ' ');
    THREAD_SAFE_RUN(php_roxen_set_header(header_name, header_content, p), "header handler");
  }
  sapi_free_header(sapi_header);
  return 0;
}

/*
 * php_roxen_sapi_send_headers() flushes the headers to the client.
 * Called before real content is sent by PHP.
 */

static int
php_roxen_low_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
  struct pike_string *ind;
  struct svalue *s_headermap;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif

  if(!MY_FD_OBJ->prog) {
    PG(connection_status) = PHP_CONNECTION_ABORTED;
    zend_bailout();
    return SAPI_HEADER_SEND_FAILED;
  }
  ind = make_shared_string(" _headers");  
  s_headermap = low_mapping_string_lookup(REQUEST_DATA, ind);
  free_string(ind);
  
  push_int(SG(sapi_headers).http_response_code);
  if(s_headermap && s_headermap->type == PIKE_T_MAPPING)
    ref_push_mapping(s_headermap->u.mapping);
  else
    push_int(0);
  safe_apply(MY_FD_OBJ, "send_headers", 2);
  pop_stack();
  
  return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int
php_roxen_sapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
  int res = 0;
  THREAD_SAFE_RUN(res = php_roxen_low_send_headers(sapi_headers TSRMLS_CC), "send headers");
  return res;
}

/*
 * php_roxen_sapi_read_post() reads a specified number of bytes from
 * the client. Used for POST/PUT requests.
 */

INLINE static int php_roxen_low_read_post(char *buf, uint count_bytes)
{
  uint total_read = 0;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif
  TSRMLS_FETCH();
  
  if(!MY_FD_OBJ->prog)
  {
    PG(connection_status) = PHP_CONNECTION_ABORTED;
    zend_bailout();
    return -1;
  }
  push_int(count_bytes);
  safe_apply(MY_FD_OBJ, "read_post", 1);
  if(Pike_sp[-1].type == PIKE_T_STRING) {
    MEMCPY(buf, Pike_sp[-1].u.string->str,
           (total_read = Pike_sp[-1].u.string->len));
    buf[total_read] = '\0';
  } else
    total_read = 0;
  pop_stack();
  return total_read;
}

static int
php_roxen_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
  uint total_read = 0;
  THREAD_SAFE_RUN(total_read = php_roxen_low_read_post(buf, count_bytes), "read post");
  return total_read;
}

/* 
 * php_roxen_sapi_read_cookies() returns the Cookie header from
 * the HTTP request header
 */
	
static char *
php_roxen_sapi_read_cookies(TSRMLS_D)
{
  char *cookies;
  cookies = lookup_string_header("HTTP_COOKIE", NULL);
  return cookies;
}

static void php_info_roxen(ZEND_MODULE_INFO_FUNC_ARGS)
{
  /*  char buf[512]; */
  php_info_print_table_start();
  php_info_print_table_row(2, "SAPI module version", "$Id$");
  /*  php_info_print_table_row(2, "Build date", Ns_InfoBuildDate());
      php_info_print_table_row(2, "Config file path", Ns_InfoConfigFile());
      php_info_print_table_row(2, "Error Log path", Ns_InfoErrorLog());
      php_info_print_table_row(2, "Installation path", Ns_InfoHomePath());
      php_info_print_table_row(2, "Hostname of server", Ns_InfoHostname());
      php_info_print_table_row(2, "Source code label", Ns_InfoLabel());
      php_info_print_table_row(2, "Server platform", Ns_InfoPlatform());
      snprintf(buf, 511, "%s/%s", Ns_InfoServerName(), Ns_InfoServerVersion());
      php_info_print_table_row(2, "Server version", buf);
      snprintf(buf, 511, "%d day(s), %02d:%02d:%02d", 
      uptime / 86400,
      (uptime / 3600) % 24,
      (uptime / 60) % 60,
      uptime % 60);
      php_info_print_table_row(2, "Server uptime", buf);
  */
  php_info_print_table_end();
}

static zend_module_entry php_roxen_module = {
  STANDARD_MODULE_HEADER,
  "Roxen",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  php_info_roxen,
  NULL,
  STANDARD_MODULE_PROPERTIES
};

static int php_roxen_startup(sapi_module_struct *sapi_module)
{
  if(php_module_startup(sapi_module, &php_roxen_module, 1) == FAILURE) {
    return FAILURE;
  } else {
    return SUCCESS;
  }
}

/* this structure is static (as in "it does not change") */

static sapi_module_struct roxen_sapi_module = {
  "roxen",
  "Roxen",
  php_roxen_startup,			/* startup */
  php_module_shutdown_wrapper,		/* shutdown */
  NULL,					/* activate */
  NULL,					/* deactivate */
  php_roxen_sapi_ub_write,		/* unbuffered write */
  NULL,					/* flush */
  NULL,					/* get uid */
  NULL,					/* getenv */
  php_error,				/* error handler */
  php_roxen_sapi_header_handler,	/* header handler */
  php_roxen_sapi_send_headers,		/* send headers handler */
  NULL,					/* send header handler */
  php_roxen_sapi_read_post,		/* read POST data */
  php_roxen_sapi_read_cookies,		/* read Cookies */
  NULL,					/* register server variables */
  NULL,					/* Log message */
  NULL,					/* Get request time */
  NULL,					/* Child terminate */

  STANDARD_SAPI_MODULE_PROPERTIES
};

/*
 * php_roxen_hash_environment() populates the php script environment
 * with a number of variables. HTTP_* variables are created for
 * the HTTP header data, so that a script can access these.
 */
#define ADD_STRING(name)										\
	MAKE_STD_ZVAL(zvalue);										\
	zvalue->type = IS_STRING;										\
	zvalue->value.str.len = strlen(buf);							\
	zvalue->value.str.val = estrndup(buf, zvalue->value.str.len);	\
	zend_hash_update(&EG(symbol_table), name, sizeof(name), 	\
			&zvalue, sizeof(zval *), NULL)

static void
php_roxen_hash_environment(TSRMLS_D)
{
  int i;
  char buf[512];
  zval *zvalue;
  struct svalue *headers;
  struct pike_string *sind;
  struct array *indices;
  struct svalue *ind, *val;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif
  sind = make_shared_string("env");
  headers = low_mapping_string_lookup(REQUEST_DATA, sind);
  free_string(sind);
  if(headers && headers->type == PIKE_T_MAPPING) {
    indices = mapping_indices(headers->u.mapping);
    for(i = 0; i < indices->size; i++) {
      ind = &indices->item[i];
      val = low_mapping_lookup(headers->u.mapping, ind);
      if(ind && ind->type == PIKE_T_STRING &&
	 val && val->type == PIKE_T_STRING) {
	int buf_len;
	buf_len = MIN(511, ind->u.string->len);
	strncpy(buf, ind->u.string->str, buf_len);
	buf[buf_len] = '\0'; /* Terminate correctly */
	MAKE_STD_ZVAL(zvalue);
	zvalue->type = IS_STRING;
	zvalue->value.str.len = val->u.string->len;
	zvalue->value.str.val = estrndup(val->u.string->str, zvalue->value.str.len);
	
	zend_hash_update(&EG(symbol_table), buf, buf_len + 1, &zvalue, sizeof(zval *), NULL);
      }
    }
    free_array(indices);
  }
  
  /*
    MAKE_STD_ZVAL(zvalue);
    zvalue->type = IS_LONG;
    zvalue->value.lval = Ns_InfoBootTime();
    zend_hash_update(&EG(symbol_table), "SERVER_BOOTTIME", sizeof("SERVER_BOOTTIME"), &zvalue, sizeof(zval *), NULL);
  */
}

/*
 * php_roxen_module_main() is called by the per-request handler and
 * "executes" the script
 */

static int php_roxen_module_main(TSRMLS_D)
{
  int res, len;
  char *dir;
  zend_file_handle file_handle;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif

  file_handle.type = ZEND_HANDLE_FILENAME;
  file_handle.filename = THIS->filename;
  file_handle.free_filename = 0;
  file_handle.opened_path = NULL;

  THREADS_ALLOW();
  res = php_request_startup(TSRMLS_C);
  THREADS_DISALLOW();
  if(res == FAILURE) {
    return 0;
  }
  php_roxen_hash_environment(TSRMLS_C);
  THREADS_ALLOW();
  php_execute_script(&file_handle TSRMLS_CC);
  php_request_shutdown(NULL);
  THREADS_DISALLOW();
  return 1;
}

/*
 * The php_roxen_request_handler() is called per request and handles
 * everything for one request.
 */

void f_php_roxen_request_handler(INT32 args)
{
  struct object *my_fd_obj;
  struct mapping *request_data;
  struct svalue *done_callback, *raw_fd;
  struct pike_string *script, *ind;
  int status = 1;
#ifdef ROXEN_USE_ZTS
  GET_THIS();
#endif
  TSRMLS_FETCH();

  if(current_thread == th_self())
    php_error(E_WARNING, "PHP5.Interpreter->run: Tried to run a PHP-script from a PHP "
	  "callback!");
  get_all_args("PHP5.Interpreter->run", args, "%S%m%O%*", &script,
	       &request_data, &my_fd_obj, &done_callback);
  if(done_callback->type != PIKE_T_FUNCTION) 
    php_error(E_WARNING, "PHP5.Interpreter->run: Bad argument 4, expected function.\n");
  PHP_LOCK(THIS); /* Need to lock here or reusing the same object might cause
		       * problems in changing stuff in that object */
#ifndef ROXEN_USE_ZTS
  GET_THIS();
#endif
  THIS->request_data = request_data;
  THIS->my_fd_obj = my_fd_obj;
  THIS->filename = script->str;
  current_thread = th_self();
  SG(request_info).query_string = lookup_string_header("QUERY_STRING", 0);
  SG(server_context) = (void *)1; /* avoid server_context == NULL */

  /* path_translated is apparently the absolute path to the file, not
     the translated PATH_INFO
  */
  SG(request_info).path_translated =
    lookup_string_header("SCRIPT_FILENAME", NULL);
  SG(request_info).request_uri = lookup_string_header("DOCUMENT_URI", NULL);
  if(!SG(request_info).request_uri)
    SG(request_info).request_uri = lookup_string_header("SCRIPT_NAME", NULL);
  SG(request_info).request_method = lookup_string_header("REQUEST_METHOD", "GET");
  SG(request_info).content_length = lookup_integer_header("HTTP_CONTENT_LENGTH", 0);
  SG(request_info).content_type = lookup_string_header("HTTP_CONTENT_TYPE", NULL);
  SG(sapi_headers).http_response_code = 200;

  /* FIXME: Check for auth stuff needs to be fixed... */ 
  SG(request_info).auth_user = NULL; 
  SG(request_info).auth_password = NULL;
  
  ind = make_shared_binary_string("my_fd", 5);
  raw_fd = low_mapping_string_lookup(THIS->request_data, ind);
  if(raw_fd && raw_fd->type == PIKE_T_OBJECT)
  {
    int fd = fd_from_object(raw_fd->u.object);
    if(fd == -1)
      php_error(E_WARNING, "PHP5.Interpreter->run: my_fd object not open or not an FD.\n");
    THIS->my_fd = fd;
  } else
    THIS->my_fd = 0;
  
  status = php_roxen_module_main(TSRMLS_C);
  current_thread = -1;
  
  apply_svalue(done_callback, 0);
  pop_stack();
  pop_n_elems(args);
  push_int(status);
  PHP_UNLOCK(THIS);
}


/* Clear the object global struct */
static void clear_struct(struct object *o)
{
  MEMSET(Pike_fp->current_storage, 0, sizeof(php_roxen_request));
}


/*
 * pike_module_init() is called by Pike once at startup
 *
 * This functions allocates basic structures
 */

void pike_module_init( void )
{
  if (!roxen_php_initialized) {
#ifdef ZTS
    tsrm_startup(1, 1, 0, NULL);
#ifdef ROXEN_USE_ZTS
    ts_allocate_id(&roxen_globals_id, sizeof(php_roxen_request), NULL, NULL);
#endif	 
#endif
    sapi_startup(&roxen_sapi_module);
    /*php_roxen_startup(&roxen_sapi_module); removed - should be called from SAPI activation*/
    roxen_php_initialized = 1;
    PHP_INIT_LOCK();
  }
  start_new_program(); /* Text */
  ADD_STORAGE(php_roxen_request);
  set_init_callback(clear_struct);
  pike_add_function("run", f_php_roxen_request_handler,
		    "function(string, mapping, object, function:int)", 0);
  add_program_constant("Interpreter", (php_program = end_program()), 0);
}

/*
 * pike_module_exit() performs the last steps before the
 * server exists. Shutdowns basic services and frees memory
 */

void pike_module_exit(void)
{
  roxen_php_initialized = 0;
  roxen_sapi_module.shutdown(&roxen_sapi_module);
  if(php_program)  free_program(php_program);
#ifdef ZTS
  tsrm_shutdown();
#endif
  PHP_DESTROY();
}
#endif
