/* 
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999 The PHP Group                         |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_0.txt.                                  |
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
#include "main.h"

#include "php_version.h"


#ifdef ZTS
/* This is true at the moment. It works with a Roxen with threads. As a matter
 * of fact, that is preferred. However the scripts are executed once at a time.
 * Once PHP is threadsafe it should be easy enough to fix.
 */
#error Roxen PHP module isnt thread safe at the moment.
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

#ifdef _REENTRANT
/* Lock used to serialize the PHP execution */
static PIKE_MUTEX_T roxen_php_execution_lock;
#define PHP_INIT_LOCK()	mt_init(&roxen_php_execution_lock)
#define PHP_LOCK()	fprintf(stderr, "*** php lock (thr_id=%d, glob=%d).\n", th_self(), current_thread);THREADS_ALLOW();mt_lock(&roxen_php_execution_lock);fprintf(stderr, "*** php locked.\n");THREADS_DISALLOW()
#define PHP_UNLOCK()	mt_unlock(&roxen_php_execution_lock);fprintf(stderr, "*** php unlocked (thr_id=%d, glob=%d).\n", th_self(), current_thread);
#define PHP_DESTROY()	mt_destroy(&roxen_php_execution_lock)
#else /* !_REENTRANT */
#define PHP_INIT_LOCK()	
#define PHP_LOCK()	
#define PHP_UNLOCK()	
#define PHP_DESTROY()	
#endif /* _REENTRANT */

extern int fd_from_object(struct object *o);
static unsigned char roxen_php_initialized;

/* Defines to get to the data supplied when the script is started. */
#define REALTHIS ((php_roxen_request *)fp->current_storage)
#define GET_THIS() current_request = REALTHIS
#define THIS current_request
#define MY_FD ((struct object *)(THIS->my_fd))
#define REQUEST_DATA ((struct mapping *)(THIS->request_data))

/* This allows calling of pike functions from the PHP callbacks,
 * which requires the Pike interpretor to be locked.
 */
#define THREAD_SAFE_RUN(COMMAND, what)  do {\
  struct thread_state *state;\
  fprintf(stderr,"threads: %d  disabled: %d  id: %d\n",num_threads, threads_disabled, th_self());\
 if((state = thread_state_for_id(th_self()))!=NULL) {\
    if(!state->swapped) {\
       fprintf(stderr, "MT lock (%s).\n", what);\
      COMMAND;\
    } else {\
       fprintf(stderr, "MT nonlock (%s).\n", what); \
      mt_lock(&interpreter_lock);\
      SWAP_IN_THREAD(state);\
      fprintf(stderr, "MT locked.\n", what); \
      COMMAND;\
      fprintf(stderr, "MT locked done.\n", what); \
      SWAP_OUT_THREAD(state);\
      mt_unlock(&interpreter_lock);\
      fprintf(stderr, "MT unlocked.\n", what); \
    }\
  }\
} while(0)

/* Toggle debug printouts, for now... */
//#define MUCH_DEBUG
#ifndef MUCH_DEBUG
void no_fprintf(){}
#define fprintf no_fprintf
#endif

struct program *php_program;

/* roxen_globals_struct is per-request object storage */
typedef struct {
  struct mapping *request_data;
  struct object *my_fd;
  char *filename;
} php_roxen_request;

static php_roxen_request *current_request = NULL;

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
  struct svalue *head;
  THREAD_SAFE_RUN(head = lookup_header(headername), "header lookup");
  if(!head || head->type != PIKE_T_STRING) {
    fprintf(stderr, "Header lookup for %s: default (%s)\n", headername,
	    default_value);
    return default_value;
  }
  fprintf(stderr, "Header lookup for %s: %s(%d)\n", headername,
      head->u.string->str, head->u.string->len);
  return head->u.string->str;
}

/* Lookup a header in the mapping and return the value as if it's an integer
 * and otherwise return the default.
 */
INLINE static int lookup_integer_header(char *headername, int default_value)
{
  struct svalue *head;
  THREAD_SAFE_RUN(head = lookup_header(headername), "header lookup");
  if(!head || head->type != PIKE_T_INT) {
    fprintf(stderr, "Header lookup for %s: default (%d)\n", headername,
	    default_value);
    return default_value;
  }
  fprintf(stderr, "Header lookup for %s: %d \n", headername,
	  head->u.integer); 
  return head->u.integer;
}

/*
 * php_roxen_low_ub_write() writes data to the client connection. Might be
 * rewritten to do more direct IO to save CPU and the need to lock the *
 * interpretor for better threading.
 */

static int
php_roxen_low_ub_write(const char *str, uint str_length) {
  int sent_bytes = 0;
  struct pike_string *to_write;
  to_write = make_shared_binary_string(str, str_length);
  push_string(to_write);
  safe_apply(MY_FD, "write", 1);
  if(sp[-1].type == PIKE_T_INT)
    sent_bytes = sp[-1].u.integer;
  //free_string(to_write);
  pop_stack();
  if(sent_bytes != str_length) {
    /* This means the connection is closed. Dead. Gone. *sniff*  */
    PG(connection_status) = PHP_CONNECTION_ABORTED;
    zend_bailout();
  }
  fprintf(stderr, "low_write done.\n");
}

/*
 * php_roxen_sapi_ub_write() calls php_roxen_low_ub_write in a Pike thread
 * safe manner.
 */

static int
php_roxen_sapi_ub_write(const char *str, uint str_length)
{
  int sent_bytes = 0;
  THREAD_SAFE_RUN(sent_bytes = php_roxen_low_ub_write(str, str_length), "write");
  fprintf(stderr, "write done.\n");
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

  fprintf(stderr, "Setting header %s to %s\n", hind->str, value);
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
			      sapi_headers_struct *sapi_headers SLS_DC)
{
  char *header_name, *header_content;
  char *p;
  struct pike_string *hind;
  header_name = sapi_header->header;
  header_content = p = strchr(header_name, ':');
  
  if(!p) return 0;
  do {
    header_content++;
  } while(*header_content == ' ');
  THREAD_SAFE_RUN(php_roxen_set_header(header_name, header_content, p), "header handler");
  efree(sapi_header->header);
  return 1;
}

/*
 * php_roxen_sapi_send_headers() flushes the headers to the client.
 * Called before real content is sent by PHP.
 */

static int
php_roxen_low_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
  struct pike_string *ind;
  struct svalue *s_headermap;
  ind = make_shared_string(" _headers");  
  s_headermap = low_mapping_string_lookup(REQUEST_DATA, ind);
  free_string(ind);
  fprintf(stderr, "Send Headers (%d)...\n", SG(sapi_headers).http_response_code);

  push_int(SG(sapi_headers).http_response_code);
  if(s_headermap && s_headermap->type == PIKE_T_MAPPING)
    ref_push_mapping(s_headermap->u.mapping);
  else
    push_int(0);
  fprintf(stderr, "Send Headers 1 (%d)...\n", SG(sapi_headers).http_response_code);
  safe_apply(MY_FD, "send_headers", 2);
  fprintf(stderr, "Send Headers 1 (%d)...\n", SG(sapi_headers).http_response_code);
  pop_stack();
  fprintf(stderr, "Send Headers 2 (%d)...\n", SG(sapi_headers).http_response_code);
  
	/*  
      if(SG(sapi_headers).send_default_content_type) {
      Ns_ConnSetRequiredHeaders(NSG(conn), "text/html", 0);
      }
      Ns_ConnFlushHeaders(NSG(conn), SG(sapi_headers).http_response_code);
  */
  return SAPI_HEADER_SENT_SUCCESSFULLY;
}

static int
php_roxen_sapi_send_headers(sapi_headers_struct *sapi_headers SLS_DC)
{
  THREAD_SAFE_RUN(php_roxen_low_send_headers(sapi_headers SLS_CC), "send headers");
  return SAPI_HEADER_SENT_SUCCESSFULLY;
}

/*
 * php_roxen_sapi_read_post() reads a specified number of bytes from
 * the client. Used for POST/PUT requests.
 */

INLINE static int php_roxen_low_read_post(char *buf, uint count_bytes)
{
  uint max_read;
  uint total_read = 0;
  fprintf(stderr, "read post (%d bytes max)\n", count_bytes);

  push_int(count_bytes);
  safe_apply(MY_FD, "read_post", 1);
  if(sp[-1].type == T_STRING) {
    MEMCPY(buf, sp[-1].u.string->str, total_read = sp[-1].u.string->len);
    buf[total_read] = '\0';
  } else
    total_read = -1;
  pop_stack();
  return total_read;
}

static int
php_roxen_sapi_read_post(char *buf, uint count_bytes SLS_DC)
{
  uint total_read;
  THREAD_SAFE_RUN(total_read = php_roxen_low_read_post(buf, count_bytes), "read post");
  return total_read;
}

/* 
 * php_roxen_sapi_read_cookies() returns the Cookie header from
 * the HTTP request header
 */
	
static char *
php_roxen_sapi_read_cookies(SLS_D)
{
  int i;
  char *cookies;
  cookies = lookup_string_header("HTTP_COOKIE", NULL);
  return cookies;
}

static void php_info_roxen(ZEND_MODULE_INFO_FUNC_ARGS)
{
  char buf[512];
  //  int uptime = Ns_InfoUptime();
	
  PUTS("<table border=5 width=600>\n");
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
  PUTS("</table>");
}

static zend_module_entry php_roxen_module = {
  "Roxen",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  php_info_roxen,
  STANDARD_MODULE_PROPERTIES
};

static int php_roxen_startup(sapi_module_struct *sapi_module)
{
  if(php_module_startup(sapi_module) == FAILURE
     || zend_register_module(&php_roxen_module) == FAILURE) {
    return FAILURE;
  } else {
    return SUCCESS;
  }
}

/* this structure is static (as in "it does not change") */

void pike_module_exit(void);

static sapi_module_struct sapi_module = {
  "PHP Language",

  php_module_startup,						/* startup */
  pike_module_exit,			/* shutdown */

  php_roxen_sapi_ub_write,					/* unbuffered write */

  php_error,								/* error handler */

  php_roxen_sapi_header_handler,				/* header handler */
  php_roxen_sapi_send_headers,				/* send headers handler */
  NULL,									/* send header handler */

  php_roxen_sapi_read_post,					/* read POST data */
  php_roxen_sapi_read_cookies,				/* read Cookies */

  STANDARD_SAPI_MODULE_PROPERTIES
};

/*
 * php_roxen_hash_environment() populates the php script environment
 * with a number of variables. HTTP_* variables are created for
 * the HTTP header data, so that a script can access these.
 */
#define ADD_STRING(name)										\
	MAKE_STD_ZVAL(pval);										\
	pval->type = IS_STRING;										\
	pval->value.str.len = strlen(buf);							\
	pval->value.str.val = estrndup(buf, pval->value.str.len);	\
	zend_hash_update(&EG(symbol_table), name, sizeof(name), 	\
			&pval, sizeof(zval *), NULL)

static void
php_roxen_hash_environment(CLS_D ELS_DC PLS_DC SLS_DC)
{
  int i;
  char buf[512];
  zval *pval;
  struct svalue *headers;
  struct pike_string *sind;
  struct array *indices;
  struct svalue *ind, *val;
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
	char *p;
	char c;
	int buf_len;
	buf_len = MIN(511, ind->u.string->len);
	strncpy(buf, ind->u.string->str, buf_len);
	buf[buf_len] = '\0'; /* Terminate correctly */
	MAKE_STD_ZVAL(pval);
	pval->type = IS_STRING;
	pval->value.str.len = val->u.string->len;
	pval->value.str.val = estrndup(val->u.string->str, pval->value.str.len);
	/*	fprintf(stderr, "Header: %s(%d)=%s\n", buf, buf_len, val->u.string->str);*/
	
	zend_hash_update(&EG(symbol_table), buf, buf_len + 1, &pval, sizeof(zval *), NULL);
      }
    }
    free_array(indices);
  }
  
  //  MAKE_STD_ZVAL(pval);
  //  pval->type = IS_LONG;
  //  pval->value.lval = Ns_InfoBootTime();
  //  zend_hash_update(&EG(symbol_table), "SERVER_BOOTTIME", sizeof("SERVER_BOOTTIME"), &pval, sizeof(zval *), NULL);
  
  //  fprintf(stderr, "Set up header environment.\n");
}

/*
 * php_roxen_module_main() is called by the per-request handler and
 * "executes" the script
 */

static int php_roxen_module_main(SLS_D)
{
  zend_file_handle file_handle;
  file_handle.type = ZEND_HANDLE_FILENAME;
  file_handle.filename = THIS->filename;
  if(php_request_startup(CLS_C ELS_CC PLS_CC SLS_CC) == FAILURE) {
    return 0;
  }
  php_roxen_hash_environment(CLS_C ELS_CC PLS_CC SLS_CC);
  THREADS_ALLOW();
  php_execute_script(&file_handle CLS_CC ELS_CC PLS_CC);
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
  struct object *my_fd;
  struct mapping *request_data;
  struct svalue *done_callback;
  struct pike_string *script;
  int opened_fd = 1, status = 1;
  int f = 0;

  if(current_thread == th_self())
    error("PHP4.Interpetor->run: Tried to run a PHP-script from a PHP "
	  "callback!");
  SLS_FETCH();
  get_all_args("PHP4.Interpretor->run", args, "%S%m%O%*", &script,
	       &request_data, &my_fd, &done_callback);
  REALTHIS->request_data = request_data;
  REALTHIS->my_fd = my_fd;
  REALTHIS->filename = script->str;
  if(done_callback->type != PIKE_T_FUNCTION)
    error("PHP4.Interpretor->run: Bad argument 4, expected function.\n");
  PHP_LOCK();
  current_thread = th_self();
  GET_THIS();
  SG(request_info).query_string = lookup_string_header("QUERY_STRING", 0);;
  SG(server_context) = (void *)1; // avoid server_context == NULL
  /* path_translated is the absolute path to the file */
  SG(request_info).path_translated =
    lookup_string_header("PATH_TRANSLATED", NULL);
  SG(request_info).request_uri = lookup_string_header("DOCUMENT_URI", NULL);
  if(!SG(request_info).request_uri)
    SG(request_info).request_uri = lookup_string_header("SCRIPT_NAME", NULL);
  SG(request_info).request_method = lookup_string_header("REQUEST_METHOD", "GET");
  SG(request_info).content_length = lookup_integer_header("CONTENT_LENGTH", 0);
  SG(request_info).content_type = "text/html";
  SG(request_info).auth_user = NULL; 
  SG(request_info).auth_password = NULL;
  status = php_roxen_module_main(SLS_C);
  current_thread = -1;
  PHP_UNLOCK();
  
  apply_svalue(done_callback, 0);
  pop_stack();
  pop_n_elems(args);
  push_int(status);

}

static void clear_struct(struct object *o)
{
  MEMSET(fp->current_storage, 0, sizeof(php_roxen_request));
}

/*
 * pike_module_init() is called by Pike once at startup
 *
 * This functions allocates basic structures
 */

void pike_module_init()
{
  if (!roxen_php_initialized) {
    sapi_startup(&sapi_module);
    sapi_module.startup(&sapi_module);
    roxen_php_initialized = 1;
    PHP_INIT_LOCK();
  }
  start_new_program(); /* Text */
  ADD_STORAGE(php_roxen_request);
  set_init_callback(clear_struct);
  pike_add_function("run", f_php_roxen_request_handler,
		    "function(string,mapping,object,function:int)", 0);
  add_program_constant("Interpretor", (php_program = end_program()), 0);
  
  /* TSRM is used to allocate a per-thread structure */
  /* read the configuration */
  //php_roxen_config(ctx);

}

/*
 * pike_module_exit() performs the last steps before the
 * server exists. Shutdowns basic services and frees memory
 */

void pike_module_exit(void)
{
  roxen_php_initialized = 0;
  sapi_module.shutdown(&sapi_module);
  if(php_program)  free_program(php_program);
  PHP_DESTROY();
}
#endif
