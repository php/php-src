 /*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sam Ruby (rubys@us.ibm.com)                                  |
   +----------------------------------------------------------------------+
*/

/* $Id$ */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


/*
 * This module implements Zend OO syntax overloading support for Java
 * components using JNI and reflection.
 */

#include "php.h"
#include "zend_compile.h"
#include "php_ini.h"
#include "php_globals.h"

#if HAVE_JAVAVM_JAVAVM_H
#include <JavaVM/JavaVM.h>
#define JNI_12
#else
#include <jni.h>
#endif


#ifdef PHP_WIN32
#include "win32/winutil.h"
#define DL_ERROR php_win_err()
#endif


#include <stdio.h>

#define IS_EXCEPTION 86

/***************************************************************************/

#ifndef KAFFE
#ifndef JNI_11
#ifndef JNI_12

#ifdef JNI_VERSION_1_2
#define JNI_12
#else
#define JNI_11
#endif

#endif
#endif
#endif

#ifdef PHP_WIN32
#ifdef JNI_12
#define JAVALIB "jvm.dll"
#else
#define JAVALIB "javai.dll"
#endif
#else
#endif

/***************************************************************************/

static int le_jobject = 0;

static char *classpath = 0;
static char *libpath   = 0;
static char *javahome  = 0;
static char *javalib   = 0;

static int iniUpdated  = 0;
static void *dl_handle = 0;

/* {{{ ZEND_BEGIN_MODULE_GLOBALS
 */
ZEND_BEGIN_MODULE_GLOBALS(java)
  JavaVM *jvm;
  JNIEnv *jenv;
  jobject php_reflect;
  jclass  reflect_class;
ZEND_END_MODULE_GLOBALS(java)
/* }}} */

#ifdef ZTS
# define JG(v) TSRMG(java_globals_id, zend_java_globals *, v)
#else
# define JG(v) (java_globals.v)
#endif

ZEND_DECLARE_MODULE_GLOBALS(java)

static zend_class_entry java_class_entry;

static PHP_INI_MH(OnIniUpdate)
{
	if (new_value) *(char**)mh_arg1 = new_value;
	iniUpdated=1;
	return SUCCESS;
}

/* {{{ PHP_INI_BEGIN 
 */
PHP_INI_BEGIN()
  PHP_INI_ENTRY1("java.class.path",   NULL, PHP_INI_ALL, OnIniUpdate, &classpath)
#ifndef JNI_11
  PHP_INI_ENTRY1("java.home",         NULL, PHP_INI_ALL, OnIniUpdate, &javahome)
  PHP_INI_ENTRY1("java.library.path", NULL, PHP_INI_ALL, OnIniUpdate, &libpath)
#endif
#ifdef JAVALIB
  PHP_INI_ENTRY1("java.library", JAVALIB, PHP_INI_ALL, OnIniUpdate, &javalib)
#else
  PHP_INI_ENTRY1("java.library", NULL,    PHP_INI_ALL, OnIniUpdate, &javalib)
#endif
PHP_INI_END()
/* }}} */

/***************************************************************************/

/* {{{ jvm_destroy
 */
/*
 * Destroy a Java Virtual Machine.
 */
void jvm_destroy(TSRMLS_D) 
{
  if (JG(php_reflect)) (*JG(jenv))->DeleteGlobalRef(JG(jenv), JG(php_reflect));
  if (JG(jvm)) {
    (*JG(jvm))->DetachCurrentThread(JG(jvm));
    (*JG(jvm))->DestroyJavaVM(JG(jvm));
    JG(jvm) = 0;
  }
  if (dl_handle) DL_UNLOAD(dl_handle);
  JG(php_reflect) = 0;
  JG(jenv) = 0;
}
/* }}} */

/* {{{ addJVMOption
 */
/*
 * Create a Java Virtual Machine.
 *  - class.path, home, and library.path are read out of the INI file
 *  - appropriate (pre 1.1, JDK 1.1, and JDK 1.2) initialization is performed
 *  - net.php.reflect class file is located
 */

#ifdef JNI_12
static void addJVMOption(JavaVMInitArgs *vm_args, char *name, char *value) 
{
  char *option = (char*) malloc(strlen(name) + strlen(value) + 1);
  strcpy(option, name);
  strcat(option, value);
  vm_args->options[vm_args->nOptions++].optionString = option;
}
#endif
/* }}} */

/* {{{ jvm_create
 */
static int jvm_create(TSRMLS_D) 
{
  int rc;
  jobject local_php_reflect;
  jthrowable error;

  jint (JNICALL *JNI_CreateVM)(const void*, const void*, void*);
#ifndef JNI_12
  jint (JNICALL *JNI_DefaultArgs)(void*);
#endif

#ifdef JNI_11
  JDK1_1InitArgs vm_args;
#else
  JavaVMInitArgs vm_args;
#ifdef JNI_12
  JavaVMOption options[3];
#endif
#endif

  iniUpdated=0;

  if (javalib) {
    dl_handle = DL_LOAD(javalib);

    if (!dl_handle) {
      php_error(E_ERROR, "Unable to load Java Library %s, error: %s", 
        javalib, DL_ERROR);
      return -1;
    }
  }

#ifndef JAVALIB
  if (!dl_handle)
    JNI_CreateVM = &JNI_CreateJavaVM;
  else
#endif

  JNI_CreateVM = (jint (JNICALL *)(const void*, const void*, void*))
    DL_FETCH_SYMBOL(dl_handle, "JNI_CreateJavaVM");

  if (!JNI_CreateVM) {
    php_error(E_ERROR, "Unable to locate CreateJavaVM function");
    return -1;
  }

#ifdef JNI_12

  vm_args.version = JNI_VERSION_1_2;
  vm_args.ignoreUnrecognized = JNI_FALSE;
  vm_args.options = options;
  vm_args.nOptions = 0;

  if (classpath) addJVMOption(&vm_args, "-Djava.class.path=",   classpath);
  if (javahome)  addJVMOption(&vm_args, "-Djava.home=",         javahome);
  if (libpath)   addJVMOption(&vm_args, "-Djava.library.path=", libpath);

#else

#ifndef JAVALIB
  if (!dl_handle)
    JNI_DefaultArgs = &JNI_GetDefaultJavaVMInitArgs;
  else
#endif

  JNI_DefaultArgs = (jint (JNICALL *)(void*))
    DL_FETCH_SYMBOL(dl_handle, "JNI_GetDefaultJavaVMInitArgs");

  if (!JNI_DefaultArgs) {
    php_error(E_ERROR, "Unable to locate GetDefaultJavaVMInitArgs function");
    return -1;
  }

  vm_args.version=0x00010001;
  (*JNI_DefaultArgs)(&vm_args);

  if (!classpath) classpath = "";
  vm_args.classpath = classpath;
#ifdef KAFFE
  vm_args.classhome = javahome;
  vm_args.libraryhome = libpath;
#endif

#endif

  rc = (*JNI_CreateVM)(&JG(jvm), &JG(jenv), &vm_args);

  if (rc) {
    php_error(E_ERROR, "Unable to create Java Virtual Machine");
    return rc;
  }

  JG(reflect_class) = (*JG(jenv))->FindClass(JG(jenv), "net/php/reflect");
  error = (*JG(jenv))->ExceptionOccurred(JG(jenv));
  if (error) {
    jclass errClass;
    jmethodID toString;
    jobject errString;
    const char *errAsUTF;
    jboolean isCopy;
    JNIEnv *jenv = JG(jenv);
    (*jenv)->ExceptionClear(jenv);
    errClass = (*jenv)->GetObjectClass(jenv, error);
    toString = (*jenv)->GetMethodID(jenv, errClass, "toString",
      "()Ljava/lang/String;");
    errString = (*jenv)->CallObjectMethod(jenv, error, toString);
    errAsUTF = (*jenv)->GetStringUTFChars(jenv, errString, &isCopy);
    php_error(E_ERROR, "%s", errAsUTF);
    if (isCopy) (*jenv)->ReleaseStringUTFChars(jenv, error, errAsUTF);
    jvm_destroy(TSRMLS_C);
    return -1;
  }

  local_php_reflect = (*JG(jenv))->AllocObject(JG(jenv), JG(reflect_class));
  JG(php_reflect) = (*JG(jenv))->NewGlobalRef(JG(jenv), local_php_reflect);
  return rc;
}
/* }}} */

/***************************************************************************/

/* {{{ _java_makeObject
 */
static jobject _java_makeObject(pval* arg TSRMLS_DC)
{
  JNIEnv *jenv = JG(jenv);
  jobject result;
  pval **handle;
  int type;
  jmethodID makeArg;
  jclass hashClass;

  switch (Z_TYPE_P(arg)) {
    case IS_STRING:
      result=(*jenv)->NewByteArray(jenv, Z_STRLEN_P(arg));
      (*jenv)->SetByteArrayRegion(jenv, (jbyteArray)result, 0,
        Z_STRLEN_P(arg), Z_STRVAL_P(arg));
      break;

    case IS_OBJECT:
      zend_hash_index_find(Z_OBJPROP_P(arg), 0, (void*)&handle);
      result = zend_list_find(Z_LVAL_PP(handle), &type);
      break;

    case IS_BOOL:
      makeArg = (*jenv)->GetMethodID(jenv, JG(reflect_class), "MakeArg",
        "(Z)Ljava/lang/Object;");
      result = (*jenv)->CallObjectMethod(jenv, JG(php_reflect), makeArg,
        (jboolean)(Z_LVAL_P(arg)));
      break;

    case IS_LONG:
      makeArg = (*jenv)->GetMethodID(jenv, JG(reflect_class), "MakeArg",
        "(J)Ljava/lang/Object;");
      result = (*jenv)->CallObjectMethod(jenv, JG(php_reflect), makeArg,
        (jlong)(Z_LVAL_P(arg)));
      break;

    case IS_DOUBLE:
      makeArg = (*jenv)->GetMethodID(jenv, JG(reflect_class), "MakeArg",
        "(D)Ljava/lang/Object;");
      result = (*jenv)->CallObjectMethod(jenv, JG(php_reflect), makeArg,
        (jdouble)(Z_DVAL_P(arg)));
      break;

    case IS_ARRAY:
      {
      jobject jkey, jval;
      zval **value;
      zval key;
      char *string_key;
      ulong num_key;
      jobject jold;
      jmethodID put, init;

      hashClass = (*jenv)->FindClass(jenv, "java/util/Hashtable");
      init = (*jenv)->GetMethodID(jenv, hashClass, "<init>", "()V");
      result = (*jenv)->NewObject(jenv, hashClass, init);

      put = (*jenv)->GetMethodID(jenv, hashClass, "put",
        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

      /* Iterate through hash */
      zend_hash_internal_pointer_reset(Z_ARRVAL_P(arg));
      while(zend_hash_get_current_data(Z_ARRVAL_P(arg), (void**)&value) == SUCCESS) {
        jval = _java_makeObject(*value TSRMLS_CC);

        switch (zend_hash_get_current_key(Z_ARRVAL_P(arg), &string_key, &num_key, 0)) {
          case HASH_KEY_IS_STRING:
            Z_TYPE(key) = IS_STRING;
            Z_STRVAL(key) = string_key;
            Z_STRLEN(key) = strlen(string_key);
            jkey = _java_makeObject(&key TSRMLS_CC);
            break;
          case HASH_KEY_IS_LONG:
            Z_TYPE(key) = IS_LONG;
            Z_LVAL(key) = num_key;
            jkey = _java_makeObject(&key TSRMLS_CC);
            break;
          default: /* HASH_KEY_NON_EXISTANT */
            jkey = 0;
        }
        jold = (*jenv)->CallObjectMethod(jenv, result, put, jkey, jval);
        if (Z_TYPE_PP(value) != IS_OBJECT) (*jenv)->DeleteLocalRef(jenv, jval);
        zend_hash_move_forward(Z_ARRVAL_P(arg));
      }

      break;
      }

    default:
      result=0;
  }

  return result;
}
/* }}} */

/***************************************************************************/

/* {{{ _java_makeArray
 */
static jobjectArray _java_makeArray(int argc, pval** argv TSRMLS_DC)
{
  JNIEnv *jenv = JG(jenv);

  jclass objectClass = (*jenv)->FindClass(jenv, "java/lang/Object");
  jobjectArray result = (*jenv)->NewObjectArray(jenv, argc, objectClass, 0);
  jobject arg;
  int i;

  for (i=0; i<argc; i++) {
    arg = _java_makeObject(argv[i] TSRMLS_CC);
    (*jenv)->SetObjectArrayElement(jenv, result, i, arg);
    if (Z_TYPE_P(argv[i]) != IS_OBJECT) (*jenv)->DeleteLocalRef(jenv, arg);
  }
  return result;
}
/* }}} */

/* {{{ checkError 
 */
static int checkError(pval *value)
{
  if (Z_TYPE_P(value) == IS_EXCEPTION) {
    php_error(E_WARNING, "%s", Z_STRVAL_P(value));
    efree(Z_STRVAL_P(value));
    ZVAL_FALSE(value);
    return 1;
  };
  return 0;
}
/* }}} */

/***************************************************************************/

/* {{{ java_call_function_handler
 */
/*
 * Invoke a method on an object.  If method name is "java", create a new
 * object instead.
 */
void java_call_function_handler(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
  JNIEnv *jenv;

  pval *object = property_reference->object;
  zend_overloaded_element *function_name = (zend_overloaded_element *)
    property_reference->elements_list->tail->data;

  int arg_count = ZEND_NUM_ARGS();
  jlong result = 0;
  pval **arguments = (pval **) emalloc(sizeof(pval *)*arg_count);

  getParametersArray(ht, arg_count, arguments);

  if (iniUpdated && JG(jenv)) jvm_destroy(TSRMLS_C);
  if (!JG(jenv)) jvm_create(TSRMLS_C);
  if (!JG(jenv)) return;
  jenv = JG(jenv);

  if (!strcmp("java", Z_STRVAL(function_name->element))) {

    /* construct a Java object:
       First argument is the class name.  Any additional arguments will
       be treated as constructor parameters. */

    jmethodID co = (*jenv)->GetMethodID(jenv, JG(reflect_class), "CreateObject",
      "(Ljava/lang/String;[Ljava/lang/Object;J)V");
    jstring className;
    result = (jlong)(long)object;

    if (ZEND_NUM_ARGS() < 1) {
      php_error(E_ERROR, "Missing classname in new Java() call");
      return;
    }

    className=(*jenv)->NewStringUTF(jenv, Z_STRVAL_P(arguments[0]));
    (*jenv)->CallVoidMethod(jenv, JG(php_reflect), co,
      className, _java_makeArray(arg_count-1, arguments+1 TSRMLS_CC), result);

    (*jenv)->DeleteLocalRef(jenv, className);

  } else {

    pval **handle;
    int type;
    jobject obj;
    jstring method;

    /* invoke a method on the given object */

    jmethodID invoke = (*jenv)->GetMethodID(jenv, JG(reflect_class), "Invoke",
      "(Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/Object;J)V");
    zend_hash_index_find(Z_OBJPROP_P(object), 0, (void**) &handle);
    obj = zend_list_find(Z_LVAL_PP(handle), &type);
    method = (*jenv)->NewStringUTF(jenv, Z_STRVAL(function_name->element));
    result = (jlong)(long)return_value;

    (*jenv)->CallVoidMethod(jenv, JG(php_reflect), invoke,
      obj, method, _java_makeArray(arg_count, arguments TSRMLS_CC), result);

    (*jenv)->DeleteLocalRef(jenv, method);

  }

  efree(arguments);
  pval_destructor(&function_name->element);

  checkError((pval*)(long)result);
}
/* }}} */

/***************************************************************************/

/* {{{ proto object java_last_exception_get(void)
	 Get last Java exception */
PHP_FUNCTION(java_last_exception_get)
{
  jlong result = 0;
  jmethodID lastEx;

  if (ZEND_NUM_ARGS()!=0) WRONG_PARAM_COUNT;

  result = (jlong)(long)return_value;
  
  lastEx = (*JG(jenv))->GetMethodID(JG(jenv), JG(reflect_class), 
          "lastException", "(J)V");

  (*JG(jenv))->CallVoidMethod(JG(jenv), JG(php_reflect), lastEx, result);
}

/* }}} */

/***************************************************************************/

/* {{{ proto void java_last_exception_clear(void)
	 Clear last java extension */
PHP_FUNCTION(java_last_exception_clear)
{
  jlong result = 0;
  jmethodID clearEx;

  if (ZEND_NUM_ARGS()!=0) WRONG_PARAM_COUNT;

  result = (jlong)(long)return_value;
  
  clearEx = (*JG(jenv))->GetMethodID(JG(jenv), JG(reflect_class), 
          "clearException", "()V");

  (*JG(jenv))->CallVoidMethod(JG(jenv), JG(php_reflect), clearEx);
}

/* }}} */

/***************************************************************************/

/* {{{ _java_getset_property
 */
static pval _java_getset_property
  (zend_property_reference *property_reference, jobjectArray value TSRMLS_DC)
{
  pval presult;
  jlong result = 0;
  pval **pobject;
  jobject obj;
  int type;

  /* get the property name */
  zend_llist_element *element = property_reference->elements_list->head;
  zend_overloaded_element *property=(zend_overloaded_element *)element->data;
  jstring propName;

  JNIEnv *jenv;
  jenv = JG(jenv);

  propName = (*jenv)->NewStringUTF(jenv, Z_STRVAL(property->element));

  /* get the object */
  zend_hash_index_find(Z_OBJPROP_P(property_reference->object),
    0, (void **) &pobject);
  obj = zend_list_find(Z_LVAL_PP(pobject), &type);
  result = (jlong)(long) &presult;
  Z_TYPE(presult) = IS_NULL;

  if (!obj || (type!=le_jobject)) {
    php_error(E_ERROR,
      "Attempt to access a Java property on a non-Java object");
  } else {
    /* invoke the method */
    jmethodID gsp = (*jenv)->GetMethodID(jenv, JG(reflect_class), "GetSetProp",
      "(Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/Object;J)V");
    (*jenv)->CallVoidMethod
       (jenv, JG(php_reflect), gsp, obj, propName, value, result);
  }

  (*jenv)->DeleteLocalRef(jenv, propName);
  pval_destructor(&property->element);
  return presult;
}
/* }}} */

/* {{{ java_get_property_handler
 */
pval java_get_property_handler(zend_property_reference *property_reference)
{
  pval presult;
  TSRMLS_FETCH();

  presult = _java_getset_property(property_reference, 0 TSRMLS_CC);
  checkError(&presult);
  return presult;
}
/* }}} */

/* {{{ java_set_property_handler
 */
int java_set_property_handler(zend_property_reference *property_reference, pval *value)
{
  pval presult;
  TSRMLS_FETCH();

  presult = _java_getset_property(property_reference, _java_makeArray(1, &value TSRMLS_CC) TSRMLS_CC);
  return checkError(&presult) ? FAILURE : SUCCESS;
}
/* }}} */

/***************************************************************************/

/* {{{ _php_java_destructor
 */
static void _php_java_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	void *jobject = (void *)rsrc->ptr;

	if (JG(jenv)) (*JG(jenv))->DeleteGlobalRef(JG(jenv), jobject);
}
/* }}} */

/* {{{ alloc_java_globals_ctor
 */
static void alloc_java_globals_ctor(zend_java_globals *java_globals TSRMLS_DC)
{
	memset(java_globals, 0, sizeof(zend_java_globals));
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(java)
{
  INIT_OVERLOADED_CLASS_ENTRY(java_class_entry, "java", NULL,
    java_call_function_handler,
    java_get_property_handler,
    java_set_property_handler);

  zend_register_internal_class(&java_class_entry TSRMLS_CC);

  le_jobject = zend_register_list_destructors_ex(_php_java_destructor, NULL, "java", module_number);

  REGISTER_INI_ENTRIES();

  if (!classpath) classpath = getenv("CLASSPATH");

  if (!libpath) {
    libpath=PG(extension_dir);
  }

  ZEND_INIT_MODULE_GLOBALS(java, alloc_java_globals_ctor, NULL);

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(java) 
{
  UNREGISTER_INI_ENTRIES();
  if (JG(jvm)) jvm_destroy(TSRMLS_C);
  return SUCCESS;
}
/* }}} */

function_entry java_functions[] = {
  PHP_FE(java_last_exception_get, NULL)
  PHP_FE(java_last_exception_clear, NULL)
  {NULL, NULL, NULL}
};


static PHP_MINFO_FUNCTION(java) {
  DISPLAY_INI_ENTRIES();
}

zend_module_entry java_module_entry = {
    STANDARD_MODULE_HEADER,
	"java",
	java_functions,
	PHP_MINIT(java),
	PHP_MSHUTDOWN(java),
	NULL,
	NULL,
	PHP_MINFO(java),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(java)

/***************************************************************************/

/* {{{ Java_net_php_reflect_setResultFromString 
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setResultFromString
  (JNIEnv *jenv, jclass self, jlong result, jbyteArray jvalue)
{
  jboolean isCopy;
  jbyte *value = (*jenv)->GetByteArrayElements(jenv, jvalue, &isCopy);
  pval *presult = (pval*)(long)result;
  Z_TYPE_P(presult)=IS_STRING;
  Z_STRLEN_P(presult)=(*jenv)->GetArrayLength(jenv, jvalue);
  Z_STRVAL_P(presult)=emalloc(Z_STRLEN_P(presult)+1);
  memcpy(Z_STRVAL_P(presult), value, Z_STRLEN_P(presult));
  Z_STRVAL_P(presult)[Z_STRLEN_P(presult)]=0;
  if (isCopy) (*jenv)->ReleaseByteArrayElements(jenv, jvalue, value, 0);
}
/* }}} */

/* {{{ Java_net_php_reflect_setResultFromLong
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setResultFromLong
  (JNIEnv *jenv, jclass self, jlong result, jlong value)
{
  pval *presult = (pval*)(long)result;
  Z_TYPE_P(presult)=IS_LONG;
  Z_LVAL_P(presult)=(long)value;
}
/* }}} */

/* {{{ Java_net_php_reflect_setResultFromDouble 
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setResultFromDouble
  (JNIEnv *jenv, jclass self, jlong result, jdouble value)
{
  pval *presult = (pval*)(long)result;
  Z_TYPE_P(presult)=IS_DOUBLE;
  Z_DVAL_P(presult)=value;
}
/* }}} */

/* {{{ Java_net_php_reflect_setResultFromBoolean
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setResultFromBoolean
  (JNIEnv *jenv, jclass self, jlong result, jboolean value)
{
  pval *presult = (pval*)(long)result;
  Z_TYPE_P(presult)=IS_BOOL;
  Z_LVAL_P(presult)=value;
}
/* }}} */

/* {{{ Java_net_php_reflect_setResultFromObject 
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setResultFromObject
  (JNIEnv *jenv, jclass self, jlong result, jobject value)
{
  /* wrapper the java object in a pval object */
  pval *presult = (pval*)(long)result;
  pval *handle;
  TSRMLS_FETCH();
  
  if (Z_TYPE_P(presult) != IS_OBJECT) {
	object_init_ex(presult, &java_class_entry);
	presult->is_ref=1;
    presult->refcount=1;
  }

  ALLOC_ZVAL(handle);
  Z_TYPE_P(handle) = IS_LONG;
  Z_LVAL_P(handle) =
    zend_list_insert((*jenv)->NewGlobalRef(jenv, value), le_jobject);
  pval_copy_constructor(handle);
  INIT_PZVAL(handle);
  zend_hash_index_update(Z_OBJPROP_P(presult), 0, &handle, sizeof(pval *), NULL);
}
/* }}} */

/* {{{ Java_net_php_reflect_setResultFromArray
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setResultFromArray
  (JNIEnv *jenv, jclass self, jlong result)
{
  array_init( (pval*)(long)result );
}
/* }}} */

/* {{{ Java_net_php_reflect_nextElement
 */
JNIEXPORT jlong JNICALL Java_net_php_reflect_nextElement
  (JNIEnv *jenv, jclass self, jlong array)
{
  pval *result;
  pval *handle = (pval*)(long)array;
  ALLOC_ZVAL(result);
  zend_hash_next_index_insert(Z_ARRVAL_P(handle), &result, sizeof(zval *), NULL);
  return (jlong)(long)result;
}
/* }}} */

/* {{{ Java_net_php_reflect_hashIndexUpdate
 */
JNIEXPORT jlong JNICALL Java_net_php_reflect_hashIndexUpdate
  (JNIEnv *jenv, jclass self, jlong array, jlong key)
{
  pval *result;
  pval *handle = (pval*)(long)array;
  ALLOC_ZVAL(result);
  zend_hash_index_update(Z_ARRVAL_P(handle), (unsigned long)key, 
    &result, sizeof(zval *), NULL);
  return (jlong)(long)result;
}
/* }}} */

/* {{{ Java_net_php_reflect_hashUpdate
 */
JNIEXPORT jlong JNICALL Java_net_php_reflect_hashUpdate
  (JNIEnv *jenv, jclass self, jlong array, jbyteArray key)
{
  pval *result;
  pval pkey;
  pval *handle = (pval*)(long)array;
  ALLOC_ZVAL(result);
  Java_net_php_reflect_setResultFromString(jenv, self, (jlong)(long)&pkey, key);
  zend_hash_update(Z_ARRVAL_P(handle), Z_STRVAL(pkey), Z_STRLEN(pkey)+1,
    &result, sizeof(zval *), NULL);
  return (jlong)(long)result;
}
/* }}} */

/* {{{ Java_net_php_reflect_setException 
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setException
  (JNIEnv *jenv, jclass self, jlong result, jbyteArray value)
{
  pval *presult = (pval*)(long)result;
  Java_net_php_reflect_setResultFromString(jenv, self, result, value);
  Z_TYPE_P(presult)=IS_EXCEPTION;
}
/* }}} */

/* {{{ Java_net_php_reflect_setEnv
 */
JNIEXPORT void JNICALL Java_net_php_reflect_setEnv
  (JNIEnv *newJenv, jclass self TSRMLS_DC)
{
  jobject local_php_reflect;

  iniUpdated=0;
  JG(jenv)=newJenv;

  if (!self) self = (*JG(jenv))->FindClass(JG(jenv), "net/php/reflect");
  JG(reflect_class) = self;

  if (JG(php_reflect)) (*JG(jenv))->DeleteGlobalRef(JG(jenv), JG(php_reflect));
  local_php_reflect = (*JG(jenv))->AllocObject(JG(jenv), JG(reflect_class));
  JG(php_reflect) = (*JG(jenv))->NewGlobalRef(JG(jenv), local_php_reflect);
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
