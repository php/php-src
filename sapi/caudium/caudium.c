/* 
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
#ifdef HAVE_CAUDIUM

#include "php_ini.h"
#include "php_globals.h"
#include "SAPI.h"
#include "php_main.h" 
#include "ext/standard/info.h"

#include "php_version.h"

/* Pike Include Files 
 *
 * conflicts with pike avoided by only using long names. Requires a new
 * Pike 0.7 since it was implemented for this interface only.
 *
 */
#define NO_PIKE_SHORTHAND

/* Ok, we are now using Pike level threads to handle PHP4 since
 * the nice th_farm threads aren't working on Linux with glibc 2.2
 * (why this is I don't know).
 */
#define USE_PIKE_LEVEL_THREADS

#include <fdlib.h>
#include <program.h>
#include <pike_types.h>
#include <interpret.h>
#include <module_support.h>
#include <array.h>
#include <backend.h>
#include <stralloc.h>
#include <mapping.h>
#include <object.h>
#include <threads.h>
#include <builtin_functions.h>
#include <operators.h>
#include <version.h>

#if (PIKE_MAJOR_VERSION == 7 && PIKE_MINOR_VERSION == 1 && PIKE_BUILD_VERSION >= 12) || PIKE_MAJOR_VERSION > 7 || (PIKE_MAJOR_VERSION == 7 && PIKE_MINOR_VERSION > 1)
# include "pike_error.h"
#else
# include "error.h"
# ifndef Pike_error
#  define Pike_error error
# endif
#endif

/* Pike 7.x and newer */
#define MY_MAPPING_LOOP(md, COUNT, KEY) \
  for(COUNT=0;COUNT < md->data->hashsize; COUNT++ ) \
	for(KEY=md->data->hash[COUNT];KEY;KEY=KEY->next)

#ifndef ZTS
/* Need thread safety */
#error You need to compile PHP with threads.
#endif

#ifndef PIKE_THREADS
#error The PHP4 module requires that your Pike has thread support.
#endif

#undef HIDE_GLOBAL_VARIABLES
#undef REVEAL_GLOBAL_VARIABLES
#define HIDE_GLOBAL_VARIABLES()
#define REVEAL_GLOBAL_VARIABLES()

/* php_caudium_request is per-request object storage */

typedef struct
{
  struct mapping *request_data;
  struct object *my_fd_obj;
  struct svalue done_cb;
  struct pike_string *filename;
  int my_fd;
  int written;
  TSRMLS_D;
} php_caudium_request;


void pike_module_init(void);
void pike_module_exit(void);
static void free_struct(TSRMLS_D);
void f_php_caudium_request_handler(INT32 args);

/* Defines to get to the data supplied when the script is started. */

/* Per thread storage area id... */
static int caudium_globals_id;

#define GET_THIS() php_caudium_request *_request = ts_resource(caudium_globals_id)
#define THIS _request
#define PTHIS ((php_caudium_request *)(Pike_fp->current_storage))
/* File descriptor integer. Used to write directly to the FD without 
 * passing Pike
 */
#define MY_FD    (THIS->my_fd)

/* FD object. Really a PHPScript object from Pike which implements a couple
 * of functions to handle headers, writing and buffering.
 */
#define MY_FD_OBJ        ((struct object *)(THIS->my_fd_obj))

/* Mapping with data supplied from the calling Caudium module. Contains
 * a mapping with headers, an FD object etc.
 */
#define REQUEST_DATA ((struct mapping *)(THIS->request_data))

extern int fd_from_object(struct object *o);
static unsigned char caudium_php_initialized;

#ifndef mt_lock_interpreter
#define mt_lock_interpreter()     mt_lock(&interpreter_lock);
#define mt_unlock_interpreter()   mt_unlock(&interpreter_lock);
#endif


/* This allows calling of pike functions from the PHP callbacks,
 * which requires the Pike interpreter to be locked.
 */
#define THREAD_SAFE_RUN(COMMAND, what)  do {\
  struct thread_state *state;\
  if((state = thread_state_for_id(th_self()))!=NULL) {\
    if(!state->swapped) {\
      COMMAND;\
    } else {\
      mt_lock_interpreter();\
      SWAP_IN_THREAD(state);\
      COMMAND;\
      SWAP_OUT_THREAD(state);\
      mt_unlock_interpreter();\
    }\
  }\
} while(0)



/* Low level header lookup. Basically looks for the named header in the mapping
 * headers in the supplied options mapping.
 */
 
INLINE static struct svalue *lookup_header(char *headername)
{
  struct svalue *headers, *value;
  struct pike_string *sind;
  GET_THIS();
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
 * php_caudium_low_ub_write() writes data to the client connection. Might be
 * rewritten to do more direct IO to save CPU and the need to lock the 
 * interpreter for better threading.
 */

INLINE static int
php_caudium_low_ub_write(const char *str, uint str_length TSRMLS_DC) {
  int sent_bytes = 0;
  struct pike_string *to_write = NULL;
  GET_THIS();
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
    PG(connection_status) = PHP_CONNECTION_ABORTED;
    zend_bailout();
  }
  return sent_bytes;
}

/*
 * php_caudium_sapi_ub_write() calls php_caudium_low_ub_write in a Pike thread
 * safe manner or writes directly to the output FD if RXML post-parsing is
 * disabled. 
 */

static int
php_caudium_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
  GET_THIS();
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
	  THIS->written += sent_bytes;
	  return sent_bytes;
	 case EINTR: 
	 case EWOULDBLOCK:
	  continue;
	}
      } else {
	sent_bytes += written;
      }
    }
    THIS->written += sent_bytes;
  } else {
    THREAD_SAFE_RUN(sent_bytes = php_caudium_low_ub_write(str, str_length TSRMLS_CC),
		    "write");
  }
  return sent_bytes;
}

/* php_caudium_set_header() sets a header in the header mapping. Called in a
 * thread safe manner from php_caudium_sapi_header_handler.
 */
INLINE static void
php_caudium_set_header(char *header_name, char *value, char *p)
{
  struct svalue hsval;
  struct pike_string *hval, *ind, *hind;
  struct mapping *headermap;
  struct svalue *s_headermap, *soldval;
  int vallen;
  GET_THIS();
  //  hval = make_shared_string(value);
  ind = make_shared_string(" _headers");
  hind = make_shared_binary_string(header_name,
				   (int)(p - header_name));

  s_headermap = low_mapping_string_lookup(REQUEST_DATA, ind);
  if(!s_headermap || s_headermap->type != PIKE_T_MAPPING)
  {
    struct svalue mappie;                                           
    mappie.type = PIKE_T_MAPPING;
    headermap = allocate_mapping(1);
    mappie.u.mapping = headermap;
    mapping_string_insert(REQUEST_DATA, ind, &mappie);
    free_mapping(headermap);
    hval = make_shared_string(value);
  } else {
    headermap = s_headermap->u.mapping;
    soldval = low_mapping_string_lookup(headermap, hind);
    vallen = strlen(value);
    if(soldval != NULL && 
       soldval->type == PIKE_T_STRING &&
       soldval->u.string->size_shift == 0) {
      /* Existing, valid header. Prepend.*/
      hval = begin_shared_string(soldval->u.string->len + 1 + vallen);
      MEMCPY(hval->str, soldval->u.string->str, soldval->u.string->len);
      STR0(hval)[soldval->u.string->len] = '\0';
      MEMCPY(hval->str+soldval->u.string->len+1, value, vallen);
      hval = end_shared_string(hval);
    } else { 
      hval = make_shared_string(value);
    }
  }
  hsval.type = PIKE_T_STRING;
  hsval.u.string = hval;

  mapping_string_insert(headermap, hind, &hsval);

  free_string(hval);
  free_string(ind);
  free_string(hind);
}

/*
 * php_caudium_sapi_header_handler() sets a HTTP reply header to be 
 * sent to the client.
 */
static int
php_caudium_sapi_header_handler(sapi_header_struct *sapi_header,
			      sapi_headers_struct *sapi_headers TSRMLS_DC)
{
  char *header_name, *header_content, *p;
  header_name = sapi_header->header;
  header_content = p = strchr(header_name, ':');
  
  if(p) {
  do {
    header_content++;
  } while(*header_content == ' ');
    THREAD_SAFE_RUN(php_caudium_set_header(header_name, header_content, p), "header handler");
  }
  sapi_free_header(sapi_header);
  return 0;
}

/*
 * php_caudium_sapi_send_headers() flushes the headers to the client.
 * Called before real content is sent by PHP.
 */

INLINE static int
php_caudium_low_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
  struct pike_string *ind;
  struct svalue *s_headermap;
  GET_THIS();
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
php_caudium_sapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
  int res = 0;
  THREAD_SAFE_RUN(res = php_caudium_low_send_headers(sapi_headers TSRMLS_CC), "send headers");
  return res;
}

/*
 * php_caudium_sapi_read_post() reads a specified number of bytes from
 * the client. Used for POST/PUT requests.
 */

INLINE static int php_caudium_low_read_post(char *buf, uint count_bytes)
{
  uint total_read = 0;
  GET_THIS();
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
php_caudium_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
  uint total_read = 0;
  THREAD_SAFE_RUN(total_read = php_caudium_low_read_post(buf, count_bytes), "read post");
  return total_read;
}

/* 
 * php_caudium_sapi_read_cookies() returns the Cookie header from
 * the HTTP request header
 */
	
static char *
php_caudium_sapi_read_cookies(TSRMLS_D)
{
  char *cookies;
  cookies = lookup_string_header("HTTP_COOKIE", NULL);
  return cookies;
}

static void php_info_caudium(ZEND_MODULE_INFO_FUNC_ARGS)
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

static zend_module_entry php_caudium_module = {
  STANDARD_MODULE_HEADER,
  "Caudium",
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  php_info_caudium,
  NULL,
  STANDARD_MODULE_PROPERTIES
};


INLINE static void low_sapi_caudium_register_variables(zval *track_vars_array TSRMLS_DC)   
{
  int i;
  struct keypair *k;
  struct svalue *headers;
  struct pike_string *sind;
  struct svalue *ind;
  struct svalue *val;
  GET_THIS();
  php_register_variable("PHP_SELF", SG(request_info).request_uri,
			track_vars_array TSRMLS_CC);
  php_register_variable("GATEWAY_INTERFACE", "CGI/1.1",
			track_vars_array TSRMLS_CC);
  php_register_variable("REQUEST_METHOD",
			(char *) SG(request_info).request_method,
			track_vars_array TSRMLS_CC);
  php_register_variable("REQUEST_URI", SG(request_info).request_uri,
			track_vars_array TSRMLS_CC);
  php_register_variable("PATH_TRANSLATED", SG(request_info).path_translated,
			track_vars_array TSRMLS_CC);

  sind = make_shared_string("env");
  headers = low_mapping_string_lookup(REQUEST_DATA, sind);
  free_string(sind);
  if(headers && headers->type == PIKE_T_MAPPING) {
    MY_MAPPING_LOOP(headers->u.mapping, i, k) {
      ind = &k->ind;
      val = &k->val;
      if(ind && ind->type == PIKE_T_STRING &&
	 val && val->type == PIKE_T_STRING) {
	php_register_variable(ind->u.string->str, val->u.string->str,
			      track_vars_array TSRMLS_CC );
      }
    }
  }
}

static void sapi_caudium_register_variables(zval *track_vars_array TSRMLS_DC)
{
  THREAD_SAFE_RUN(low_sapi_caudium_register_variables(track_vars_array TSRMLS_CC), "register_variables");
}


static int php_caudium_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, &php_caudium_module, 1)==FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}


/* this structure is static (as in "it does not change") */
static sapi_module_struct caudium_sapi_module = {
  "caudium",
  "Caudium",
  php_caudium_startup,			/* startup */
  php_module_shutdown_wrapper,		/* shutdown */
  NULL,					/* activate */
  NULL,					/* deactivate */
  php_caudium_sapi_ub_write,		/* unbuffered write */
  NULL,					/* flush */
  NULL,					/* get uid */
  NULL,					/* getenv */
  php_error,				/* error handler */
  php_caudium_sapi_header_handler,	/* header handler */
  php_caudium_sapi_send_headers,	/* send headers handler */
  NULL,					/* send header handler */
  php_caudium_sapi_read_post,		/* read POST data */
  php_caudium_sapi_read_cookies,	/* read cookies */
  sapi_caudium_register_variables,	/* register server variables */
  NULL,					/* Log message */
  NULL,					/* Block interruptions */
  NULL,					/* Unblock interruptions */

  STANDARD_SAPI_MODULE_PROPERTIES
};

/*
 * php_caudium_module_main() is called by the per-request handler and
 * "executes" the script
 */

static void php_caudium_module_main(php_caudium_request *ureq)
{
  int res;
  zend_file_handle file_handle = {0};
#ifndef USE_PIKE_LEVEL_THREADS
  struct thread_state *state;
  extern struct program *thread_id_prog;
#endif
  TSRMLS_FETCH();
  GET_THIS();
  THIS->filename = ureq->filename;
  THIS->done_cb = ureq->done_cb;
  THIS->my_fd_obj = ureq->my_fd_obj;
  THIS->my_fd = ureq->my_fd;
  THIS->request_data = ureq->request_data;
  free(ureq);

#ifndef USE_PIKE_LEVEL_THREADS
  mt_lock_interpreter();
  init_interpreter();
#if PIKE_MAJOR_VERSION == 7 && PIKE_MINOR_VERSION < 1
  thread_id = low_clone(thread_id_prog);
  state = OBJ2THREAD(thread_id);
  Pike_stack_top=((char *)&state)+ (thread_stack_size-16384) * STACK_DIRECTION;
  recoveries = NULL;
  call_c_initializers(thread_id);
  OBJ2THREAD(thread_id)->id=th_self();
  num_threads++;
  thread_table_insert(thread_id);
  state->status=THREAD_RUNNING;
#else
  Pike_interpreter.thread_id = low_clone(thread_id_prog);
  state = OBJ2THREAD(Pike_interpreter.thread_id);
  Pike_interpreter.stack_top=((char *)&state)+ (thread_stack_size-16384) * STACK_DIRECTION;
  Pike_interpreter.recoveries = NULL;
  call_c_initializers(Pike_interpreter.thread_id);
  state->id=th_self();
  //  SWAP_OUT_THREAD(OBJ2THREAD(Pike_interpreter.thread_id));
  num_threads++;
  thread_table_insert(Pike_interpreter.thread_id);
  state->status=THREAD_RUNNING;
#endif
  state->swapped = 0;
#endif 
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
  if (!strcmp(SG(request_info).request_method, "HEAD")) {
    SG(request_info).headers_only = 1;
  } else {
    SG(request_info).headers_only = 0;
  }

  /* Let PHP4 handle the deconding of the AUTH */
  php_handle_auth_data(lookup_string_header("HTTP_AUTHORIZATION", NULL), TSRMLS_C);
   /* Swap out this thread and release the interpreter lock to allow
   * Pike threads to run. We wait since the above would otherwise require
   * a lot of unlock/lock.
   */
#ifndef USE_PIKE_LEVEL_THREADS
  SWAP_OUT_THREAD(state);
  mt_unlock_interpreter();
#else
  THREADS_ALLOW();
#endif

#ifdef VIRTUAL_DIR
  /* Change virtual directory, if the feature is enabled, which is
   * (almost) a requirement for PHP in Caudium. Might want to fail if it
   * isn't. Not a problem though, since it's on by default when using ZTS
   * which we require.
   */
  VCWD_CHDIR_FILE(THIS->filename->str);
#endif
  
  file_handle.type = ZEND_HANDLE_FILENAME;
  file_handle.filename = THIS->filename->str;
  file_handle.opened_path = NULL;
  file_handle.free_filename = 0;

  THIS->written = 0;
  res = php_request_startup(TSRMLS_C);

  if(res == FAILURE) {
    THREAD_SAFE_RUN({
      apply_svalue(&THIS->done_cb, 0);
      pop_stack();
      free_struct(TSRMLS_C);
    }, "Negative run response");
  } else {
    php_execute_script(&file_handle TSRMLS_CC);
    php_request_shutdown(NULL);
    THREAD_SAFE_RUN({
      push_int(THIS->written);
      apply_svalue(&THIS->done_cb, 1);
      pop_stack();
      free_struct(TSRMLS_C);
    }, "positive run response");
  }

#ifndef USE_PIKE_LEVEL_THREADS
  mt_lock_interpreter();
  SWAP_IN_THREAD(state);
#if PIKE_MAJOR_VERSION == 7 && PIKE_MINOR_VERSION < 1
  state->status=THREAD_EXITED;
  co_signal(& state->status_change);
  thread_table_delete(thread_id);
  free_object(thread_id);
  thread_id=NULL;
#else
  state->status=THREAD_EXITED;
  co_signal(& state->status_change);
  thread_table_delete(Pike_interpreter.thread_id);
  free_object(Pike_interpreter.thread_id);
  Pike_interpreter.thread_id=NULL;
#endif
  cleanup_interpret();
  num_threads--;
  mt_unlock_interpreter();
#else
  THREADS_DISALLOW();
#endif
}

/*
 * The php_caudium_request_handler() is called per request and handles
 * everything for one request.
 */

void f_php_caudium_request_handler(INT32 args)
{
  struct object *my_fd_obj;
  struct mapping *request_data;
  struct svalue *done_callback;
  struct pike_string *script;
  struct svalue *raw_fd;
  struct pike_string *ind;
  php_caudium_request *_request;
  THIS = malloc(sizeof(php_caudium_request));
  if(THIS == NULL)
    Pike_error("Out of memory.");

  get_all_args("PHP4.Interpreter->run", args, "%S%m%O%*", &script,
	       &request_data, &my_fd_obj, &done_callback);
  if(done_callback->type != PIKE_T_FUNCTION) 
    Pike_error("PHP4.Interpreter->run: Bad argument 4, expected function.\n");
  add_ref(request_data);
  add_ref(my_fd_obj);
  add_ref(script);

  THIS->request_data = request_data;
  THIS->my_fd_obj = my_fd_obj;
  THIS->filename = script;
  assign_svalue_no_free(&THIS->done_cb, done_callback);

  ind = make_shared_binary_string("my_fd", 5);
  raw_fd = low_mapping_string_lookup(THIS->request_data, ind);
  if(raw_fd && raw_fd->type == PIKE_T_OBJECT)
  {
    int fd = fd_from_object(raw_fd->u.object);
    if(fd == -1)
      THIS->my_fd = 0; /* Don't send directly to this FD... */
    else
      THIS->my_fd = fd;
  } else
    THIS->my_fd = 0;
#ifdef USE_PIKE_LEVEL_THREADS
  php_caudium_module_main(THIS);
#else
  th_farm((void (*)(void *))php_caudium_module_main, THIS);
#endif
  pop_n_elems(args);
}

static void free_struct(TSRMLS_D)
{
  GET_THIS();
  if(THIS->request_data) free_mapping(THIS->request_data);
  if(THIS->my_fd_obj)    free_object(THIS->my_fd_obj);
  free_svalue(&THIS->done_cb);
  if(THIS->filename)     free_string(THIS->filename);
  MEMSET(THIS, 0, sizeof(php_caudium_request));
}


/*
 * pike_module_init() is called by Pike once at startup
 *
 * This functions allocates basic structures
 */

void pike_module_init( void )
{
  if (!caudium_php_initialized) {
    caudium_php_initialized = 1;
    tsrm_startup(1, 1, 0, NULL);
    ts_allocate_id(&caudium_globals_id, sizeof(php_caudium_request), NULL, NULL);
    sapi_startup(&caudium_sapi_module);
    sapi_module.startup(&caudium_sapi_module);
  }
  start_new_program(); /* Text */
  pike_add_function("run", f_php_caudium_request_handler,
		    "function(string, mapping, object, function:void)", 0);
  end_class("Interpreter", 0);
}

/*
 * pike_module_exit() performs the last steps before the
 * server exists. Shutdowns basic services and frees memory
 */

void pike_module_exit(void)
{
  caudium_php_initialized = 0;
  sapi_module.shutdown(&caudium_sapi_module);
  tsrm_shutdown();
}
#endif
