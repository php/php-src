/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Timm Friebe <thekid@thekid.de>                              |
   |          George Schlossnagle <george@omniti.com>                     |
   |          Andrei Zmievski <andrei@gravitonic.com>                     |
   |          Marcus Boerger <helly@php.net>                              |
   |          Johannes Schlueter <johannes@php.net>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_reflection.h"
#include "ext/standard/info.h"

#include "zend.h"
#include "zend_API.h"
#include "zend_exceptions.h"
#include "zend_operators.h"
#include "zend_constants.h"
#include "zend_ini.h"
#include "zend_interfaces.h"
#include "zend_closures.h"
#include "zend_generators.h"
#include "zend_extensions.h"
#include "zend_builtin_functions.h"

#define reflection_update_property(object, name, value) do { \
		zval member; \
		ZVAL_STRINGL(&member, name, sizeof(name)-1); \
		zend_std_write_property(object, &member, value, NULL); \
		if (Z_REFCOUNTED_P(value)) Z_DELREF_P(value); \
		zval_ptr_dtor(&member); \
	} while (0)

/* Class entry pointers */
PHPAPI zend_class_entry *reflector_ptr;
PHPAPI zend_class_entry *reflection_exception_ptr;
PHPAPI zend_class_entry *reflection_ptr;
PHPAPI zend_class_entry *reflection_function_abstract_ptr;
PHPAPI zend_class_entry *reflection_function_ptr;
PHPAPI zend_class_entry *reflection_generator_ptr;
PHPAPI zend_class_entry *reflection_parameter_ptr;
PHPAPI zend_class_entry *reflection_type_ptr;
PHPAPI zend_class_entry *reflection_class_ptr;
PHPAPI zend_class_entry *reflection_object_ptr;
PHPAPI zend_class_entry *reflection_method_ptr;
PHPAPI zend_class_entry *reflection_property_ptr;
PHPAPI zend_class_entry *reflection_class_constant_ptr;
PHPAPI zend_class_entry *reflection_extension_ptr;
PHPAPI zend_class_entry *reflection_zend_extension_ptr;

#if MBO_0
ZEND_BEGIN_MODULE_GLOBALS(reflection)
	int dummy;
ZEND_END_MODULE_GLOBALS(reflection)

#ifdef ZTS
# define REFLECTION_G(v) \
	ZEND_TSRMG(reflection_globals_id, zend_reflection_globals*, v)
extern int reflection_globals_id;
#else
# define REFLECTION_G(v) (reflection_globals.v)
extern zend_reflection_globals reflectionglobals;
#endif

ZEND_DECLARE_MODULE_GLOBALS(reflection)
#endif /* MBO_0 */

/* Method macros */

#define METHOD_NOTSTATIC(ce)                                                                                \
	if ((Z_TYPE(EX(This)) != IS_OBJECT) || !instanceof_function(Z_OBJCE(EX(This)), ce)) {           \
		php_error_docref(NULL, E_ERROR, "%s() cannot be called statically", get_active_function_name());        \
		return;                                                                                             \
	}                                                                                                       \

/* Exception throwing macro */
#define _DO_THROW(msg)                                                                                      \
	zend_throw_exception(reflection_exception_ptr, msg, 0);                                       \
	return;                                                                                                 \

#define RETURN_ON_EXCEPTION                                                                                 \
	if (EG(exception) && EG(exception)->ce == reflection_exception_ptr) {                            \
		return;                                                                                             \
	}

#define GET_REFLECTION_OBJECT()	                                                                   			\
	intern = Z_REFLECTION_P(getThis());                                                      				\
	if (intern->ptr == NULL) {                                                            \
		RETURN_ON_EXCEPTION                                                                                 \
		php_error_docref(NULL, E_ERROR, "Internal error: Failed to retrieve the reflection object");                    \
	}                                                                                                       \

#define GET_REFLECTION_OBJECT_PTR(target)                                                                   \
	GET_REFLECTION_OBJECT()																					\
	target = intern->ptr;                                                                                   \

/* Class constants */
#define REGISTER_REFLECTION_CLASS_CONST_LONG(class_name, const_name, value)                                        \
	zend_declare_class_constant_long(reflection_ ## class_name ## _ptr, const_name, sizeof(const_name)-1, (zend_long)value);

/* {{{ Smart string functions */
typedef struct _string {
	zend_string *buf;
	size_t alloced;
} string;

static void string_init(string *str)
{
	str->buf= zend_string_alloc(1024, 0);
	str->alloced = 1024;
	ZSTR_VAL(str->buf)[0] = '\0';
	ZSTR_LEN(str->buf) = 0;
}

static string *string_printf(string *str, const char *format, ...)
{
	size_t len;
	va_list arg;
	char *s_tmp;

	va_start(arg, format);
	len = zend_vspprintf(&s_tmp, 0, format, arg);
	if (len) {
		register size_t nlen = (ZSTR_LEN(str->buf) + 1 + len + (1024 - 1)) & ~(1024 - 1);
		if (str->alloced < nlen) {
			size_t old_len = ZSTR_LEN(str->buf);
			str->alloced = nlen;
			str->buf = zend_string_extend(str->buf, str->alloced, 0);
			ZSTR_LEN(str->buf) = old_len;
		}
		memcpy(ZSTR_VAL(str->buf) + ZSTR_LEN(str->buf), s_tmp, len + 1);
		ZSTR_LEN(str->buf) += len;
	}
	efree(s_tmp);
	va_end(arg);
	return str;
}

static string *string_write(string *str, char *buf, size_t len)
{
	register size_t nlen = (ZSTR_LEN(str->buf) + 1 + len + (1024 - 1)) & ~(1024 - 1);
	if (str->alloced < nlen) {
		size_t old_len = ZSTR_LEN(str->buf);
		str->alloced = nlen;
		str->buf = zend_string_extend(str->buf, str->alloced, 0);
		ZSTR_LEN(str->buf) = old_len;
	}
	memcpy(ZSTR_VAL(str->buf) + ZSTR_LEN(str->buf), buf, len);
	ZSTR_LEN(str->buf) += len;
	ZSTR_VAL(str->buf)[ZSTR_LEN(str->buf)] = '\0';
	return str;
}

static string *string_append(string *str, string *append)
{
	if (ZSTR_LEN(append->buf) > 0) {
		string_write(str, ZSTR_VAL(append->buf), ZSTR_LEN(append->buf));
	}
	return str;
}

static void string_free(string *str)
{
	zend_string_release(str->buf);
	str->alloced = 0;
	str->buf = NULL;
}
/* }}} */

/* {{{ Object structure */

/* Struct for properties */
typedef struct _property_reference {
	zend_class_entry *ce;
	zend_property_info prop;
} property_reference;

/* Struct for parameters */
typedef struct _parameter_reference {
	uint32_t offset;
	uint32_t required;
	struct _zend_arg_info *arg_info;
	zend_function *fptr;
} parameter_reference;

/* Struct for type hints */
typedef struct _type_reference {
	struct _zend_arg_info *arg_info;
	zend_function *fptr;
} type_reference;

typedef enum {
	REF_TYPE_OTHER,      /* Must be 0 */
	REF_TYPE_FUNCTION,
	REF_TYPE_GENERATOR,
	REF_TYPE_PARAMETER,
	REF_TYPE_TYPE,
	REF_TYPE_PROPERTY,
	REF_TYPE_DYNAMIC_PROPERTY,
	REF_TYPE_CLASS_CONSTANT
} reflection_type_t;

/* Struct for reflection objects */
typedef struct {
	zval dummy; /* holder for the second property */
	zval obj;
	void *ptr;
	zend_class_entry *ce;
	reflection_type_t ref_type;
	unsigned int ignore_visibility:1;
	zend_object zo;
} reflection_object;

static inline reflection_object *reflection_object_from_obj(zend_object *obj) {
	return (reflection_object*)((char*)(obj) - XtOffsetOf(reflection_object, zo));
}

#define Z_REFLECTION_P(zv)  reflection_object_from_obj(Z_OBJ_P((zv)))
/* }}} */

static zend_object_handlers reflection_object_handlers;

static zval *_default_load_entry(zval *object, char *name, size_t name_len) /* {{{ */
{
	zval *value;

	if ((value = zend_hash_str_find_ind(Z_OBJPROP_P(object), name, name_len)) == NULL) {
		return NULL;
	}
	return value;
}
/* }}} */

static void _default_get_entry(zval *object, char *name, int name_len, zval *return_value) /* {{{ */
{
	zval *value;

	if ((value = _default_load_entry(object, name, name_len)) == NULL) {
		RETURN_FALSE;
	}
	ZVAL_DUP(return_value, value);
}
/* }}} */

#ifdef ilia_0
static void _default_lookup_entry(zval *object, char *name, int name_len, zval **return_value) /* {{{ */
{
	zval **value;

	if (zend_hash_find(Z_OBJPROP_P(object), name, name_len, (void **) &value) == FAILURE) {
		*return_value = NULL;
	} else {
		*return_value = *value;
	}
}
/* }}} */
#endif

static zend_function *_copy_function(zend_function *fptr) /* {{{ */
{
	if (fptr
		&& (fptr->internal_function.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))
	{
		zend_function *copy_fptr;
		copy_fptr = emalloc(sizeof(zend_function));
		memcpy(copy_fptr, fptr, sizeof(zend_function));
		copy_fptr->internal_function.function_name = zend_string_copy(fptr->internal_function.function_name);
		return copy_fptr;
	} else {
		/* no copy needed */
		return fptr;
	}
}
/* }}} */

static void _free_function(zend_function *fptr) /* {{{ */
{
	if (fptr
		&& (fptr->internal_function.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))
	{
		zend_string_release(fptr->internal_function.function_name);
		zend_free_trampoline(fptr);
	}
}
/* }}} */

static void reflection_free_objects_storage(zend_object *object) /* {{{ */
{
	reflection_object *intern = reflection_object_from_obj(object);
	parameter_reference *reference;
	property_reference *prop_reference;
	type_reference *typ_reference;

	if (intern->ptr) {
		switch (intern->ref_type) {
		case REF_TYPE_PARAMETER:
			reference = (parameter_reference*)intern->ptr;
			_free_function(reference->fptr);
			efree(intern->ptr);
			break;
		case REF_TYPE_TYPE:
			typ_reference = (type_reference*)intern->ptr;
			_free_function(typ_reference->fptr);
			efree(intern->ptr);
			break;
		case REF_TYPE_FUNCTION:
			_free_function(intern->ptr);
			break;
		case REF_TYPE_PROPERTY:
			efree(intern->ptr);
			break;
		case REF_TYPE_DYNAMIC_PROPERTY:
			prop_reference = (property_reference*)intern->ptr;
			zend_string_release(prop_reference->prop.name);
			efree(intern->ptr);
			break;
		case REF_TYPE_GENERATOR:
		case REF_TYPE_CLASS_CONSTANT:
		case REF_TYPE_OTHER:
			break;
		}
	}
	intern->ptr = NULL;
	zval_ptr_dtor(&intern->obj);
	zend_object_std_dtor(object);
}
/* }}} */

static zend_object *reflection_objects_new(zend_class_entry *class_type) /* {{{ */
{
	reflection_object *intern;

	intern = ecalloc(1, sizeof(reflection_object) + zend_object_properties_size(class_type));
	intern->zo.ce = class_type;

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);
	intern->zo.handlers = &reflection_object_handlers;
	return &intern->zo;
}
/* }}} */

static zval *reflection_instantiate(zend_class_entry *pce, zval *object) /* {{{ */
{
	object_init_ex(object, pce);
	return object;
}
/* }}} */

static void _const_string(string *str, char *name, zval *value, char *indent);
static void _function_string(string *str, zend_function *fptr, zend_class_entry *scope, char* indent);
static void _property_string(string *str, zend_property_info *prop, char *prop_name, char* indent);
static void _class_const_string(string *str, char *name, zend_class_constant *c, char* indent);
static void _class_string(string *str, zend_class_entry *ce, zval *obj, char *indent);
static void _extension_string(string *str, zend_module_entry *module, char *indent);
static void _zend_extension_string(string *str, zend_extension *extension, char *indent);

/* {{{ _class_string */
static void _class_string(string *str, zend_class_entry *ce, zval *obj, char *indent)
{
	int count, count_static_props = 0, count_static_funcs = 0, count_shadow_props = 0;
	string sub_indent;

	string_init(&sub_indent);
	string_printf(&sub_indent, "%s    ", indent);

	/* TBD: Repair indenting of doc comment (or is this to be done in the parser?) */
	if (ce->type == ZEND_USER_CLASS && ce->info.user.doc_comment) {
		string_printf(str, "%s%s", indent, ZSTR_VAL(ce->info.user.doc_comment));
		string_write(str, "\n", 1);
	}

	if (obj && Z_TYPE_P(obj) == IS_OBJECT) {
		string_printf(str, "%sObject of class [ ", indent);
	} else {
		char *kind = "Class";
		if (ce->ce_flags & ZEND_ACC_INTERFACE) {
			kind = "Interface";
		} else if (ce->ce_flags & ZEND_ACC_TRAIT) {
			kind = "Trait";
		}
		string_printf(str, "%s%s [ ", indent, kind);
	}
	string_printf(str, (ce->type == ZEND_USER_CLASS) ? "<user" : "<internal");
	if (ce->type == ZEND_INTERNAL_CLASS && ce->info.internal.module) {
		string_printf(str, ":%s", ce->info.internal.module->name);
	}
	string_printf(str, "> ");
	if (ce->get_iterator != NULL) {
		string_printf(str, "<iterateable> ");
	}
	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		string_printf(str, "interface ");
	} else if (ce->ce_flags & ZEND_ACC_TRAIT) {
		string_printf(str, "trait ");
	} else {
		if (ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
			string_printf(str, "abstract ");
		}
		if (ce->ce_flags & ZEND_ACC_FINAL) {
			string_printf(str, "final ");
		}
		string_printf(str, "class ");
	}
	string_printf(str, "%s", ZSTR_VAL(ce->name));
	if (ce->parent) {
		string_printf(str, " extends %s", ZSTR_VAL(ce->parent->name));
	}

	if (ce->num_interfaces) {
		uint32_t i;

		if (ce->ce_flags & ZEND_ACC_INTERFACE) {
			string_printf(str, " extends %s", ZSTR_VAL(ce->interfaces[0]->name));
		} else {
			string_printf(str, " implements %s", ZSTR_VAL(ce->interfaces[0]->name));
		}
		for (i = 1; i < ce->num_interfaces; ++i) {
			string_printf(str, ", %s", ZSTR_VAL(ce->interfaces[i]->name));
		}
	}
	string_printf(str, " ] {\n");

	/* The information where a class is declared is only available for user classes */
	if (ce->type == ZEND_USER_CLASS) {
		string_printf(str, "%s  @@ %s %d-%d\n", indent, ZSTR_VAL(ce->info.user.filename),
						ce->info.user.line_start, ce->info.user.line_end);
	}

	/* Constants */
	string_printf(str, "\n");
	count = zend_hash_num_elements(&ce->constants_table);
	string_printf(str, "%s  - Constants [%d] {\n", indent, count);
	if (count > 0) {
		zend_string *key;
		zend_class_constant *c;

		ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, key, c) {
			zval_update_constant_ex(&c->value, c->ce);
			_class_const_string(str, ZSTR_VAL(key), c, ZSTR_VAL(sub_indent.buf));
		} ZEND_HASH_FOREACH_END();
	}
	string_printf(str, "%s  }\n", indent);

	/* Static properties */
	/* counting static properties */
	count = zend_hash_num_elements(&ce->properties_info);
	if (count > 0) {
		zend_property_info *prop;

		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			if(prop->flags & ZEND_ACC_SHADOW) {
				count_shadow_props++;
			} else if (prop->flags & ZEND_ACC_STATIC) {
				count_static_props++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	/* static properties */
	string_printf(str, "\n%s  - Static properties [%d] {\n", indent, count_static_props);
	if (count_static_props > 0) {
		zend_property_info *prop;

		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			if ((prop->flags & ZEND_ACC_STATIC) && !(prop->flags & ZEND_ACC_SHADOW)) {
				_property_string(str, prop, NULL, ZSTR_VAL(sub_indent.buf));
			}
		} ZEND_HASH_FOREACH_END();
	}
	string_printf(str, "%s  }\n", indent);

	/* Static methods */
	/* counting static methods */
	count = zend_hash_num_elements(&ce->function_table);
	if (count > 0) {
		zend_function *mptr;

		ZEND_HASH_FOREACH_PTR(&ce->function_table, mptr) {
			if (mptr->common.fn_flags & ZEND_ACC_STATIC
				&& ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) == 0 || mptr->common.scope == ce))
			{
				count_static_funcs++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	/* static methods */
	string_printf(str, "\n%s  - Static methods [%d] {", indent, count_static_funcs);
	if (count_static_funcs > 0) {
		zend_function *mptr;

		ZEND_HASH_FOREACH_PTR(&ce->function_table, mptr) {
			if (mptr->common.fn_flags & ZEND_ACC_STATIC
				&& ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) == 0 || mptr->common.scope == ce))
			{
				string_printf(str, "\n");
				_function_string(str, mptr, ce, ZSTR_VAL(sub_indent.buf));
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		string_printf(str, "\n");
	}
	string_printf(str, "%s  }\n", indent);

	/* Default/Implicit properties */
	count = zend_hash_num_elements(&ce->properties_info) - count_static_props - count_shadow_props;
	string_printf(str, "\n%s  - Properties [%d] {\n", indent, count);
	if (count > 0) {
		zend_property_info *prop;

		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			if (!(prop->flags & (ZEND_ACC_STATIC|ZEND_ACC_SHADOW))) {
				_property_string(str, prop, NULL, ZSTR_VAL(sub_indent.buf));
			}
		} ZEND_HASH_FOREACH_END();
	}
	string_printf(str, "%s  }\n", indent);

	if (obj && Z_TYPE_P(obj) == IS_OBJECT && Z_OBJ_HT_P(obj)->get_properties) {
		string       dyn;
		HashTable    *properties = Z_OBJ_HT_P(obj)->get_properties(obj);
		zend_string  *prop_name;

		string_init(&dyn);
		count = 0;

		if (properties && zend_hash_num_elements(properties)) {
			ZEND_HASH_FOREACH_STR_KEY(properties, prop_name) {
				if (prop_name && ZSTR_LEN(prop_name) && ZSTR_VAL(prop_name)[0]) { /* skip all private and protected properties */
					if (!zend_hash_exists(&ce->properties_info, prop_name)) {
						count++;
						_property_string(&dyn, NULL, ZSTR_VAL(prop_name), ZSTR_VAL(sub_indent.buf));
					}
				}
			} ZEND_HASH_FOREACH_END();
		}

		string_printf(str, "\n%s  - Dynamic properties [%d] {\n", indent, count);
		string_append(str, &dyn);
		string_printf(str, "%s  }\n", indent);
		string_free(&dyn);
	}

	/* Non static methods */
	count = zend_hash_num_elements(&ce->function_table) - count_static_funcs;
	if (count > 0) {
		zend_function *mptr;
		zend_string *key;
		string dyn;

		count = 0;
		string_init(&dyn);

		ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->function_table, key, mptr) {
			if ((mptr->common.fn_flags & ZEND_ACC_STATIC) == 0
				&& ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) == 0 || mptr->common.scope == ce))
			{
				size_t len = ZSTR_LEN(mptr->common.function_name);

				/* Do not display old-style inherited constructors */
				if ((mptr->common.fn_flags & ZEND_ACC_CTOR) == 0
					|| mptr->common.scope == ce
					|| !key
					|| zend_binary_strcasecmp(ZSTR_VAL(key), ZSTR_LEN(key), ZSTR_VAL(mptr->common.function_name), len) == 0)
				{
					zend_function *closure;
					/* see if this is a closure */
					if (ce == zend_ce_closure && obj && (len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
						&& memcmp(ZSTR_VAL(mptr->common.function_name), ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
						&& (closure = zend_get_closure_invoke_method(Z_OBJ_P(obj))) != NULL)
					{
						mptr = closure;
					} else {
						closure = NULL;
					}
					string_printf(&dyn, "\n");
					_function_string(&dyn, mptr, ce, ZSTR_VAL(sub_indent.buf));
					count++;
					_free_function(closure);
				}
			}
		} ZEND_HASH_FOREACH_END();
		string_printf(str, "\n%s  - Methods [%d] {", indent, count);
		if (!count) {
			string_printf(str, "\n");
		}
		string_append(str, &dyn);
		string_free(&dyn);
	} else {
		string_printf(str, "\n%s  - Methods [0] {\n", indent);
	}
	string_printf(str, "%s  }\n", indent);

	string_printf(str, "%s}\n", indent);
	string_free(&sub_indent);
}
/* }}} */

/* {{{ _const_string */
static void _const_string(string *str, char *name, zval *value, char *indent)
{
	char *type = zend_zval_type_name(value);
	zend_string *value_str = zval_get_string(value);

	string_printf(str, "%s    Constant [ %s %s ] { %s }\n",
					indent, type, name, ZSTR_VAL(value_str));

	zend_string_release(value_str);
}
/* }}} */

/* {{{ _class_const_string */
static void _class_const_string(string *str, char *name, zend_class_constant *c, char *indent)
{
	char *visibility = zend_visibility_string(Z_ACCESS_FLAGS(c->value));
	zend_string *value_str;
	char *type;

	zval_update_constant_ex(&c->value, c->ce);
	value_str = zval_get_string(&c->value);
	type = zend_zval_type_name(&c->value);

	string_printf(str, "%sConstant [ %s %s %s ] { %s }\n",
					indent, visibility, type, name, ZSTR_VAL(value_str));

	zend_string_release(value_str);
}
/* }}} */

/* {{{ _get_recv_opcode */
static zend_op* _get_recv_op(zend_op_array *op_array, uint32_t offset)
{
	zend_op *op = op_array->opcodes;
	zend_op *end = op + op_array->last;

	++offset;
	while (op < end) {
		if ((op->opcode == ZEND_RECV || op->opcode == ZEND_RECV_INIT
		    || op->opcode == ZEND_RECV_VARIADIC) && op->op1.num == (zend_long)offset)
		{
			return op;
		}
		++op;
	}
	return NULL;
}
/* }}} */

/* {{{ _parameter_string */
static void _parameter_string(string *str, zend_function *fptr, struct _zend_arg_info *arg_info, uint32_t offset, uint32_t required, char* indent)
{
	string_printf(str, "Parameter #%d [ ", offset);
	if (offset >= required) {
		string_printf(str, "<optional> ");
	} else {
		string_printf(str, "<required> ");
	}
	if (arg_info->class_name) {
		string_printf(str, "%s ",
			(fptr->type == ZEND_INTERNAL_FUNCTION &&
			 !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) ?
			((zend_internal_arg_info*)arg_info)->class_name :
			ZSTR_VAL(arg_info->class_name));
		if (arg_info->allow_null) {
			string_printf(str, "or NULL ");
		}
	} else if (arg_info->type_hint) {
		string_printf(str, "%s ", zend_get_type_by_const(arg_info->type_hint));
		if (arg_info->allow_null) {
			string_printf(str, "or NULL ");
		}
	}
	if (arg_info->pass_by_reference) {
		string_write(str, "&", sizeof("&")-1);
	}
	if (arg_info->is_variadic) {
		string_write(str, "...", sizeof("...")-1);
	}
	if (arg_info->name) {
		string_printf(str, "$%s",
			(fptr->type == ZEND_INTERNAL_FUNCTION &&
			 !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) ?
			((zend_internal_arg_info*)arg_info)->name :
			ZSTR_VAL(arg_info->name));
	} else {
		string_printf(str, "$param%d", offset);
	}
	if (fptr->type == ZEND_USER_FUNCTION && offset >= required) {
		zend_op *precv = _get_recv_op((zend_op_array*)fptr, offset);
		if (precv && precv->opcode == ZEND_RECV_INIT && precv->op2_type != IS_UNUSED) {
			zval zv;

			string_write(str, " = ", sizeof(" = ")-1);
			ZVAL_DUP(&zv, RT_CONSTANT(&fptr->op_array, precv->op2));
			zval_update_constant_ex(&zv, fptr->common.scope);
			if (Z_TYPE(zv) == IS_TRUE) {
				string_write(str, "true", sizeof("true")-1);
			} else if (Z_TYPE(zv) == IS_FALSE) {
				string_write(str, "false", sizeof("false")-1);
			} else if (Z_TYPE(zv) == IS_NULL) {
				string_write(str, "NULL", sizeof("NULL")-1);
			} else if (Z_TYPE(zv) == IS_STRING) {
				string_write(str, "'", sizeof("'")-1);
				string_write(str, Z_STRVAL(zv), MIN(Z_STRLEN(zv), 15));
				if (Z_STRLEN(zv) > 15) {
					string_write(str, "...", sizeof("...")-1);
				}
				string_write(str, "'", sizeof("'")-1);
			} else if (Z_TYPE(zv) == IS_ARRAY) {
				string_write(str, "Array", sizeof("Array")-1);
			} else {
				zend_string *zv_str = zval_get_string(&zv);
				string_write(str, ZSTR_VAL(zv_str), ZSTR_LEN(zv_str));
				zend_string_release(zv_str);
			}
			zval_ptr_dtor(&zv);
		}
	}
	string_write(str, " ]", sizeof(" ]")-1);
}
/* }}} */

/* {{{ _function_parameter_string */
static void _function_parameter_string(string *str, zend_function *fptr, char* indent)
{
	struct _zend_arg_info *arg_info = fptr->common.arg_info;
	uint32_t i, num_args, required = fptr->common.required_num_args;

	if (!arg_info) {
		return;
	}

	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}
	string_printf(str, "\n");
	string_printf(str, "%s- Parameters [%d] {\n", indent, num_args);
	for (i = 0; i < num_args; i++) {
		string_printf(str, "%s  ", indent);
		_parameter_string(str, fptr, arg_info, i, required, indent);
		string_write(str, "\n", sizeof("\n")-1);
		arg_info++;
	}
	string_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _function_closure_string */
static void _function_closure_string(string *str, zend_function *fptr, char* indent)
{
	uint32_t i, count;
	zend_string *key;
	HashTable *static_variables;

	if (fptr->type != ZEND_USER_FUNCTION || !fptr->op_array.static_variables) {
		return;
	}

	static_variables = fptr->op_array.static_variables;
	count = zend_hash_num_elements(static_variables);

	if (!count) {
		return;
	}

	string_printf(str, "\n");
	string_printf(str, "%s- Bound Variables [%d] {\n", indent, zend_hash_num_elements(static_variables));
	i = 0;
	ZEND_HASH_FOREACH_STR_KEY(static_variables, key) {
		string_printf(str, "%s    Variable #%d [ $%s ]\n", indent, i++, ZSTR_VAL(key));
	} ZEND_HASH_FOREACH_END();
	string_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _function_string */
static void _function_string(string *str, zend_function *fptr, zend_class_entry *scope, char* indent)
{
	string param_indent;
	zend_function *overwrites;
	zend_string *lc_name;
	size_t lc_name_len;

	/* TBD: Repair indenting of doc comment (or is this to be done in the parser?)
	 * What's "wrong" is that any whitespace before the doc comment start is
	 * swallowed, leading to an unaligned comment.
	 */
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		string_printf(str, "%s%s\n", indent, ZSTR_VAL(fptr->op_array.doc_comment));
	}

	string_write(str, indent, strlen(indent));
	string_printf(str, fptr->common.fn_flags & ZEND_ACC_CLOSURE ? "Closure [ " : (fptr->common.scope ? "Method [ " : "Function [ "));
	string_printf(str, (fptr->type == ZEND_USER_FUNCTION) ? "<user" : "<internal");
	if (fptr->common.fn_flags & ZEND_ACC_DEPRECATED) {
		string_printf(str, ", deprecated");
	}
	if (fptr->type == ZEND_INTERNAL_FUNCTION && ((zend_internal_function*)fptr)->module) {
		string_printf(str, ":%s", ((zend_internal_function*)fptr)->module->name);
	}

	if (scope && fptr->common.scope) {
		if (fptr->common.scope != scope) {
			string_printf(str, ", inherits %s", ZSTR_VAL(fptr->common.scope->name));
		} else if (fptr->common.scope->parent) {
			lc_name_len = ZSTR_LEN(fptr->common.function_name);
			lc_name = zend_string_alloc(lc_name_len, 0);
			zend_str_tolower_copy(ZSTR_VAL(lc_name), ZSTR_VAL(fptr->common.function_name), lc_name_len);
			if ((overwrites = zend_hash_find_ptr(&fptr->common.scope->parent->function_table, lc_name)) != NULL) {
				if (fptr->common.scope != overwrites->common.scope) {
					string_printf(str, ", overwrites %s", ZSTR_VAL(overwrites->common.scope->name));
				}
			}
			efree(lc_name);
		}
	}
	if (fptr->common.prototype && fptr->common.prototype->common.scope) {
		string_printf(str, ", prototype %s", ZSTR_VAL(fptr->common.prototype->common.scope->name));
	}
	if (fptr->common.fn_flags & ZEND_ACC_CTOR) {
		string_printf(str, ", ctor");
	}
	if (fptr->common.fn_flags & ZEND_ACC_DTOR) {
		string_printf(str, ", dtor");
	}
	string_printf(str, "> ");

	if (fptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
		string_printf(str, "abstract ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_FINAL) {
		string_printf(str, "final ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_STATIC) {
		string_printf(str, "static ");
	}

	if (fptr->common.scope) {
		/* These are mutually exclusive */
		switch (fptr->common.fn_flags & ZEND_ACC_PPP_MASK) {
			case ZEND_ACC_PUBLIC:
				string_printf(str, "public ");
				break;
			case ZEND_ACC_PRIVATE:
				string_printf(str, "private ");
				break;
			case ZEND_ACC_PROTECTED:
				string_printf(str, "protected ");
				break;
			default:
				string_printf(str, "<visibility error> ");
				break;
		}
		string_printf(str, "method ");
	} else {
		string_printf(str, "function ");
	}

	if (fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) {
		string_printf(str, "&");
	}
	string_printf(str, "%s ] {\n", ZSTR_VAL(fptr->common.function_name));
	/* The information where a function is declared is only available for user classes */
	if (fptr->type == ZEND_USER_FUNCTION) {
		string_printf(str, "%s  @@ %s %d - %d\n", indent,
						ZSTR_VAL(fptr->op_array.filename),
						fptr->op_array.line_start,
						fptr->op_array.line_end);
	}
	string_init(&param_indent);
	string_printf(&param_indent, "%s  ", indent);
	if (fptr->common.fn_flags & ZEND_ACC_CLOSURE) {
		_function_closure_string(str, fptr, ZSTR_VAL(param_indent.buf));
	}
	_function_parameter_string(str, fptr, ZSTR_VAL(param_indent.buf));
	string_free(&param_indent);
	if (fptr->op_array.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		string_printf(str, "  %s- Return [ ", indent);
		if (fptr->common.arg_info[-1].class_name) {
			string_printf(str, "%s ",
				(fptr->type == ZEND_INTERNAL_FUNCTION &&
				 !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) ?
					((zend_internal_arg_info*)(fptr->common.arg_info - 1))->class_name :
					ZSTR_VAL(fptr->common.arg_info[-1].class_name));
			if (fptr->common.arg_info[-1].allow_null) {
				string_printf(str, "or NULL ");
			}
		} else if (fptr->common.arg_info[-1].type_hint) {
			string_printf(str, "%s ", zend_get_type_by_const(fptr->common.arg_info[-1].type_hint));
			if (fptr->common.arg_info[-1].allow_null) {
				string_printf(str, "or NULL ");
			}
		}
		string_printf(str, "]\n");
	}
	string_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _property_string */
static void _property_string(string *str, zend_property_info *prop, char *prop_name, char* indent)
{
	const char *class_name;

	string_printf(str, "%sProperty [ ", indent);
	if (!prop) {
		string_printf(str, "<dynamic> public $%s", prop_name);
	} else {
		if (!(prop->flags & ZEND_ACC_STATIC)) {
			if (prop->flags & ZEND_ACC_IMPLICIT_PUBLIC) {
				string_write(str, "<implicit> ", sizeof("<implicit> ") - 1);
			} else {
				string_write(str, "<default> ", sizeof("<default> ") - 1);
			}
		}

		/* These are mutually exclusive */
		switch (prop->flags & ZEND_ACC_PPP_MASK) {
			case ZEND_ACC_PUBLIC:
				string_printf(str, "public ");
				break;
			case ZEND_ACC_PRIVATE:
				string_printf(str, "private ");
				break;
			case ZEND_ACC_PROTECTED:
				string_printf(str, "protected ");
				break;
		}
		if(prop->flags & ZEND_ACC_STATIC) {
			string_printf(str, "static ");
		}

		zend_unmangle_property_name(prop->name, &class_name, (const char**)&prop_name);
		string_printf(str, "$%s", prop_name);
	}

	string_printf(str, " ]\n");
}
/* }}} */

static int _extension_ini_string(zval *el, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zend_ini_entry *ini_entry = (zend_ini_entry*)Z_PTR_P(el);
	string *str = va_arg(args, string *);
	char *indent = va_arg(args, char *);
	int number = va_arg(args, int);
	char *comma = "";

	if (number == ini_entry->module_number) {
		string_printf(str, "    %sEntry [ %s <", indent, ZSTR_VAL(ini_entry->name));
		if (ini_entry->modifiable == ZEND_INI_ALL) {
			string_printf(str, "ALL");
		} else {
			if (ini_entry->modifiable & ZEND_INI_USER) {
				string_printf(str, "USER");
				comma = ",";
			}
			if (ini_entry->modifiable & ZEND_INI_PERDIR) {
				string_printf(str, "%sPERDIR", comma);
				comma = ",";
			}
			if (ini_entry->modifiable & ZEND_INI_SYSTEM) {
				string_printf(str, "%sSYSTEM", comma);
			}
		}

		string_printf(str, "> ]\n");
		string_printf(str, "    %s  Current = '%s'\n", indent, ini_entry->value ? ZSTR_VAL(ini_entry->value) : "");
		if (ini_entry->modified) {
			string_printf(str, "    %s  Default = '%s'\n", indent, ini_entry->orig_value ? ZSTR_VAL(ini_entry->orig_value) : "");
		}
		string_printf(str, "    %s}\n", indent);
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int _extension_class_string(zval *el, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zend_class_entry *ce = (zend_class_entry*)Z_PTR_P(el);
	string *str = va_arg(args, string *);
	char *indent = va_arg(args, char *);
	struct _zend_module_entry *module = va_arg(args, struct _zend_module_entry*);
	int *num_classes = va_arg(args, int*);

	if ((ce->type == ZEND_INTERNAL_CLASS) && ce->info.internal.module && !strcasecmp(ce->info.internal.module->name, module->name)) {
		/* dump class if it is not an alias */
		if (!zend_binary_strcasecmp(ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), ZSTR_VAL(hash_key->key), ZSTR_LEN(hash_key->key))) {
			string_printf(str, "\n");
			_class_string(str, ce, NULL, indent);
			(*num_classes)++;
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static int _extension_const_string(zval *el, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zend_constant *constant = (zend_constant*)Z_PTR_P(el);
	string *str = va_arg(args, string *);
	char *indent = va_arg(args, char *);
	struct _zend_module_entry *module = va_arg(args, struct _zend_module_entry*);
	int *num_classes = va_arg(args, int*);

	if (constant->module_number  == module->module_number) {
		_const_string(str, ZSTR_VAL(constant->name), &constant->value, indent);
		(*num_classes)++;
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

static void _extension_string(string *str, zend_module_entry *module, char *indent) /* {{{ */
{
	string_printf(str, "%sExtension [ ", indent);
	if (module->type == MODULE_PERSISTENT) {
		string_printf(str, "<persistent>");
	}
	if (module->type == MODULE_TEMPORARY) {
		string_printf(str, "<temporary>" );
	}
	string_printf(str, " extension #%d %s version %s ] {\n",
					module->module_number, module->name,
					(module->version == NO_VERSION_YET) ? "<no_version>" : module->version);

	if (module->deps) {
		const zend_module_dep* dep = module->deps;

		string_printf(str, "\n  - Dependencies {\n");

		while(dep->name) {
			string_printf(str, "%s    Dependency [ %s (", indent, dep->name);

			switch(dep->type) {
			case MODULE_DEP_REQUIRED:
				string_write(str, "Required", sizeof("Required") - 1);
				break;
			case MODULE_DEP_CONFLICTS:
				string_write(str, "Conflicts", sizeof("Conflicts") - 1);
				break;
			case MODULE_DEP_OPTIONAL:
				string_write(str, "Optional", sizeof("Optional") - 1);
				break;
			default:
				string_write(str, "Error", sizeof("Error") - 1); /* shouldn't happen */
				break;
			}

			if (dep->rel) {
				string_printf(str, " %s", dep->rel);
			}
			if (dep->version) {
				string_printf(str, " %s", dep->version);
			}
			string_write(str, ") ]\n", sizeof(") ]\n") - 1);
			dep++;
		}
		string_printf(str, "%s  }\n", indent);
	}

	{
		string str_ini;
		string_init(&str_ini);
		zend_hash_apply_with_arguments(EG(ini_directives), (apply_func_args_t) _extension_ini_string, 3, &str_ini, indent, module->module_number);
		if (ZSTR_LEN(str_ini.buf) > 0) {
			string_printf(str, "\n  - INI {\n");
			string_append(str, &str_ini);
			string_printf(str, "%s  }\n", indent);
		}
		string_free(&str_ini);
	}

	{
		string str_constants;
		int num_constants = 0;

		string_init(&str_constants);
		zend_hash_apply_with_arguments(EG(zend_constants), (apply_func_args_t) _extension_const_string, 4, &str_constants, indent, module, &num_constants);
		if (num_constants) {
			string_printf(str, "\n  - Constants [%d] {\n", num_constants);
			string_append(str, &str_constants);
			string_printf(str, "%s  }\n", indent);
		}
		string_free(&str_constants);
	}

	{
		zend_function *fptr;
		int first = 1;

		ZEND_HASH_FOREACH_PTR(CG(function_table), fptr) {
			if (fptr->common.type==ZEND_INTERNAL_FUNCTION
				&& fptr->internal_function.module == module) {
				if (first) {
					string_printf(str, "\n  - Functions {\n");
					first = 0;
				}
				_function_string(str, fptr, NULL, "    ");
			}
		} ZEND_HASH_FOREACH_END();
		if (!first) {
			string_printf(str, "%s  }\n", indent);
		}
	}

	{
		string str_classes;
		string sub_indent;
		int num_classes = 0;

		string_init(&sub_indent);
		string_printf(&sub_indent, "%s    ", indent);
		string_init(&str_classes);
		zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) _extension_class_string, 4, &str_classes, ZSTR_VAL(sub_indent.buf), module, &num_classes);
		if (num_classes) {
			string_printf(str, "\n  - Classes [%d] {", num_classes);
			string_append(str, &str_classes);
			string_printf(str, "%s  }\n", indent);
		}
		string_free(&str_classes);
		string_free(&sub_indent);
	}

	string_printf(str, "%s}\n", indent);
}
/* }}} */

static void _zend_extension_string(string *str, zend_extension *extension, char *indent) /* {{{ */
{
	string_printf(str, "%sZend Extension [ %s ", indent, extension->name);

	if (extension->version) {
		string_printf(str, "%s ", extension->version);
	}
	if (extension->copyright) {
		string_printf(str, "%s ", extension->copyright);
	}
	if (extension->author) {
		string_printf(str, "by %s ", extension->author);
	}
	if (extension->URL) {
		string_printf(str, "<%s> ", extension->URL);
	}

	string_printf(str, "]\n");
}
/* }}} */

/* {{{ _function_check_flag */
static void _function_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_function *mptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	RETURN_BOOL(mptr->common.fn_flags & mask);
}
/* }}} */

/* {{{ zend_reflection_class_factory */
PHPAPI void zend_reflection_class_factory(zend_class_entry *ce, zval *object)
{
	reflection_object *intern;
	zval name;

	ZVAL_STR_COPY(&name, ce->name);
	reflection_instantiate(reflection_class_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = ce;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = ce;
	reflection_update_property(object, "name", &name);
}
/* }}} */

/* {{{ reflection_extension_factory */
static void reflection_extension_factory(zval *object, const char *name_str)
{
	reflection_object *intern;
	zval name;
	size_t name_len = strlen(name_str);
	zend_string *lcname;
	struct _zend_module_entry *module;

	lcname = zend_string_alloc(name_len, 0);
	zend_str_tolower_copy(ZSTR_VAL(lcname), name_str, name_len);
	module = zend_hash_find_ptr(&module_registry, lcname);
	zend_string_free(lcname);
	if (!module) {
		return;
	}

	reflection_instantiate(reflection_extension_ptr, object);
	intern = Z_REFLECTION_P(object);
	ZVAL_STRINGL(&name, module->name, name_len);
	intern->ptr = module;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = NULL;
	reflection_update_property(object, "name", &name);
}
/* }}} */

/* {{{ reflection_parameter_factory */
static void reflection_parameter_factory(zend_function *fptr, zval *closure_object, struct _zend_arg_info *arg_info, uint32_t offset, uint32_t required, zval *object)
{
	reflection_object *intern;
	parameter_reference *reference;
	zval name;

	if (arg_info->name) {
		if (fptr->type == ZEND_INTERNAL_FUNCTION &&
		    !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
			ZVAL_STRING(&name, ((zend_internal_arg_info*)arg_info)->name);
		} else {
			ZVAL_STR_COPY(&name, arg_info->name);
		}
	} else {
		ZVAL_NULL(&name);
	}
	reflection_instantiate(reflection_parameter_ptr, object);
	intern = Z_REFLECTION_P(object);
	reference = (parameter_reference*) emalloc(sizeof(parameter_reference));
	reference->arg_info = arg_info;
	reference->offset = offset;
	reference->required = required;
	reference->fptr = fptr;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_PARAMETER;
	intern->ce = fptr->common.scope;
	if (closure_object) {
		Z_ADDREF_P(closure_object);
		ZVAL_COPY_VALUE(&intern->obj, closure_object);
	}
	reflection_update_property(object, "name", &name);
}
/* }}} */

/* {{{ reflection_type_factory */
static void reflection_type_factory(zend_function *fptr, zval *closure_object, struct _zend_arg_info *arg_info, zval *object)
{
	reflection_object *intern;
	type_reference *reference;

	reflection_instantiate(reflection_type_ptr, object);
	intern = Z_REFLECTION_P(object);
	reference = (type_reference*) emalloc(sizeof(type_reference));
	reference->arg_info = arg_info;
	reference->fptr = fptr;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_TYPE;
	intern->ce = fptr->common.scope;
	if (closure_object) {
		Z_ADDREF_P(closure_object);
		ZVAL_COPY_VALUE(&intern->obj, closure_object);
	}
}
/* }}} */

/* {{{ reflection_function_factory */
static void reflection_function_factory(zend_function *function, zval *closure_object, zval *object)
{
	reflection_object *intern;
	zval name;

	ZVAL_STR_COPY(&name, function->common.function_name);

	reflection_instantiate(reflection_function_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = function;
	intern->ref_type = REF_TYPE_FUNCTION;
	intern->ce = NULL;
	if (closure_object) {
		Z_ADDREF_P(closure_object);
		ZVAL_COPY_VALUE(&intern->obj, closure_object);
	}
	reflection_update_property(object, "name", &name);
}
/* }}} */

/* {{{ reflection_method_factory */
static void reflection_method_factory(zend_class_entry *ce, zend_function *method, zval *closure_object, zval *object)
{
	reflection_object *intern;
	zval name;
	zval classname;

	ZVAL_STR_COPY(&name, (method->common.scope && method->common.scope->trait_aliases)?
			zend_resolve_method_name(ce, method) : method->common.function_name);
	ZVAL_STR_COPY(&classname, method->common.scope->name);
	reflection_instantiate(reflection_method_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = method;
	intern->ref_type = REF_TYPE_FUNCTION;
	intern->ce = ce;
	if (closure_object) {
		Z_ADDREF_P(closure_object);
		ZVAL_COPY_VALUE(&intern->obj, closure_object);
	}
	reflection_update_property(object, "name", &name);
	reflection_update_property(object, "class", &classname);
}
/* }}} */

/* {{{ reflection_property_factory */
static void reflection_property_factory(zend_class_entry *ce, zend_property_info *prop, zval *object)
{
	reflection_object *intern;
	zval name;
	zval classname;
	property_reference *reference;
	const char *class_name, *prop_name;
	size_t prop_name_len;

	zend_unmangle_property_name_ex(prop->name, &class_name, &prop_name, &prop_name_len);

	if (!(prop->flags & ZEND_ACC_PRIVATE)) {
		/* we have to search the class hierarchy for this (implicit) public or protected property */
		zend_class_entry *tmp_ce = ce, *store_ce = ce;
		zend_property_info *tmp_info = NULL;

		while (tmp_ce && (tmp_info = zend_hash_str_find_ptr(&tmp_ce->properties_info, prop_name, prop_name_len)) == NULL) {
			ce = tmp_ce;
			tmp_ce = tmp_ce->parent;
		}

		if (tmp_info && !(tmp_info->flags & ZEND_ACC_SHADOW)) { /* found something and it's not a parent's private */
			prop = tmp_info;
		} else { /* not found, use initial value */
			ce = store_ce;
		}
	}

	ZVAL_STRINGL(&name, prop_name, prop_name_len);
	ZVAL_STR_COPY(&classname, prop->ce->name);

	reflection_instantiate(reflection_property_ptr, object);
	intern = Z_REFLECTION_P(object);
	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->ce = ce;
	reference->prop = *prop;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_PROPERTY;
	intern->ce = ce;
	intern->ignore_visibility = 0;
	reflection_update_property(object, "name", &name);
	reflection_update_property(object, "class", &classname);
}
/* }}} */

/* {{{ reflection_class_constant_factory */
static void reflection_class_constant_factory(zend_class_entry *ce, zend_string *name_str, zend_class_constant *constant, zval *object)
{
	reflection_object *intern;
	zval name;
	zval classname;

	ZVAL_STR_COPY(&name, name_str);
	ZVAL_STR_COPY(&classname, ce->name);

	reflection_instantiate(reflection_class_constant_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = constant;
	intern->ref_type = REF_TYPE_CLASS_CONSTANT;
	intern->ce = constant->ce;
	intern->ignore_visibility = 0;
	reflection_update_property(object, "name", &name);
	reflection_update_property(object, "class", &classname);
}
/* }}} */

/* {{{ _reflection_export */
static void _reflection_export(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_ptr, int ctor_argc)
{
	zval reflector;
	zval output, *output_ptr = &output;
	zval *argument_ptr, *argument2_ptr;
	zval retval, params[2];
	int result;
	int return_output = 0;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	if (ctor_argc == 1) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &argument_ptr, &return_output) == FAILURE) {
			return;
		}
		ZVAL_COPY_VALUE(&params[0], argument_ptr);
		ZVAL_NULL(&params[1]);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz|b", &argument_ptr, &argument2_ptr, &return_output) == FAILURE) {
			return;
		}
		ZVAL_COPY_VALUE(&params[0], argument_ptr);
		ZVAL_COPY_VALUE(&params[1], argument2_ptr);
	}

	/* Create object */
	if (object_and_properties_init(&reflector, ce_ptr, NULL) == FAILURE) {
		_DO_THROW("Could not create reflector");
	}

	/* Call __construct() */

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = Z_OBJ(reflector);
	fci.retval = &retval;
	fci.param_count = ctor_argc;
	fci.params = params;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = ce_ptr->constructor;
	fcc.calling_scope = ce_ptr;
	fcc.called_scope = Z_OBJCE(reflector);
	fcc.object = Z_OBJ(reflector);

	result = zend_call_function(&fci, &fcc);

	zval_ptr_dtor(&retval);

	if (EG(exception)) {
		zval_ptr_dtor(&reflector);
		return;
	}
	if (result == FAILURE) {
		zval_ptr_dtor(&reflector);
		_DO_THROW("Could not create reflector");
	}

	/* Call static reflection::export */
	ZVAL_BOOL(&output, return_output);
	ZVAL_COPY_VALUE(&params[0], &reflector);
	ZVAL_COPY_VALUE(&params[1], output_ptr);

	ZVAL_STRINGL(&fci.function_name, "reflection::export", sizeof("reflection::export") - 1);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = 2;
	fci.params = params;
	fci.no_separation = 1;

	result = zend_call_function(&fci, NULL);

	zval_ptr_dtor(&fci.function_name);

	if (result == FAILURE && EG(exception) == NULL) {
		zval_ptr_dtor(&reflector);
		zval_ptr_dtor(&retval);
		_DO_THROW("Could not execute reflection::export()");
	}

	if (return_output) {
		ZVAL_COPY_VALUE(return_value, &retval);
	} else {
		zval_ptr_dtor(&retval);
	}

	/* Destruct reflector which is no longer needed */
	zval_ptr_dtor(&reflector);
}
/* }}} */

/* {{{ _reflection_param_get_default_param */
static parameter_reference *_reflection_param_get_default_param(INTERNAL_FUNCTION_PARAMETERS)
{
	reflection_object *intern;
	parameter_reference *param;

	intern = Z_REFLECTION_P(getThis());
	if (intern->ptr == NULL) {
		if (EG(exception) && EG(exception)->ce == reflection_exception_ptr) {
			return NULL;
		}
		php_error_docref(NULL, E_ERROR, "Internal error: Failed to retrieve the reflection object");
	}

	param = intern->ptr;
	if (param->fptr->type != ZEND_USER_FUNCTION) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Cannot determine default value for internal functions");
		return NULL;
	}

	return param;
}
/* }}} */

/* {{{ _reflection_param_get_default_precv */
static zend_op *_reflection_param_get_default_precv(INTERNAL_FUNCTION_PARAMETERS, parameter_reference *param)
{
	zend_op *precv;

	if (param == NULL) {
		return NULL;
	}

	precv = _get_recv_op((zend_op_array*)param->fptr, param->offset);
	if (!precv || precv->opcode != ZEND_RECV_INIT || precv->op2_type == IS_UNUSED) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Internal error: Failed to retrieve the default value");
		return NULL;
	}

	return precv;
}
/* }}} */

/* {{{ Preventing __clone from being called */
ZEND_METHOD(reflection, __clone)
{
	/* Should never be executable */
	_DO_THROW("Cannot clone object using __clone()");
}
/* }}} */

/* {{{ proto public static mixed Reflection::export(Reflector r [, bool return])
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection, export)
{
	zval *object, fname, retval;
	int result;
	zend_bool return_output = 0;

#ifndef FAST_ZPP
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &object, reflector_ptr, &return_output) == FAILURE) {
		return;
	}
#else
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(object, reflector_ptr)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(return_output)
	ZEND_PARSE_PARAMETERS_END();
#endif

	/* Invoke the __toString() method */
	ZVAL_STRINGL(&fname, "__tostring", sizeof("__tostring") - 1);
	result= call_user_function_ex(NULL, object, &fname, &retval, 0, NULL, 0, NULL);
	zval_dtor(&fname);

	if (result == FAILURE) {
		_DO_THROW("Invocation of method __toString() failed");
		/* Returns from this function */
	}

	if (Z_TYPE(retval) == IS_UNDEF) {
		php_error_docref(NULL, E_WARNING, "%s::__toString() did not return anything", ZSTR_VAL(Z_OBJCE_P(object)->name));
		RETURN_FALSE;
	}

	if (return_output) {
		ZVAL_COPY_VALUE(return_value, &retval);
	} else {
		/* No need for _r variant, return of __toString should always be a string */
		zend_print_zval(&retval, 0);
		zend_printf("\n");
		zval_ptr_dtor(&retval);
	}
}
/* }}} */

/* {{{ proto public static array Reflection::getModifierNames(int modifiers)
   Returns an array of modifier names */
ZEND_METHOD(reflection, getModifierNames)
{
	zend_long modifiers;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &modifiers) == FAILURE) {
		return;
	}

	array_init(return_value);

	if (modifiers & (ZEND_ACC_ABSTRACT | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		add_next_index_stringl(return_value, "abstract", sizeof("abstract")-1);
	}
	if (modifiers & ZEND_ACC_FINAL) {
		add_next_index_stringl(return_value, "final", sizeof("final")-1);
	}
	if (modifiers & ZEND_ACC_IMPLICIT_PUBLIC) {
		add_next_index_stringl(return_value, "public", sizeof("public")-1);
	}

	/* These are mutually exclusive */
	switch (modifiers & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PUBLIC:
			add_next_index_stringl(return_value, "public", sizeof("public")-1);
			break;
		case ZEND_ACC_PRIVATE:
			add_next_index_stringl(return_value, "private", sizeof("private")-1);
			break;
		case ZEND_ACC_PROTECTED:
			add_next_index_stringl(return_value, "protected", sizeof("protected")-1);
			break;
	}

	if (modifiers & ZEND_ACC_STATIC) {
		add_next_index_stringl(return_value, "static", sizeof("static")-1);
	}
}
/* }}} */

/* {{{ proto public static mixed ReflectionFunction::export(string name [, bool return])
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_function, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_function_ptr, 1);
}
/* }}} */

/* {{{ proto public void ReflectionFunction::__construct(string name)
   Constructor. Throws an Exception in case the given function does not exist */
ZEND_METHOD(reflection_function, __construct)
{
	zval name;
	zval *object;
	zval *closure = NULL;
	char *lcname, *nsname;
	reflection_object *intern;
	zend_function *fptr;
	char *name_str;
	size_t name_len;

	object = getThis();
	intern = Z_REFLECTION_P(object);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "O", &closure, zend_ce_closure) == SUCCESS) {
		fptr = (zend_function*)zend_get_closure_method_def(closure);
		Z_ADDREF_P(closure);
	} else { 
		if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &name_str, &name_len) == FAILURE) {
			return;
		}

		lcname = zend_str_tolower_dup(name_str, name_len);

		/* Ignore leading "\" */
		nsname = lcname;
		if (lcname[0] == '\\') {
			nsname = &lcname[1];
			name_len--;
		}

		if ((fptr = zend_hash_str_find_ptr(EG(function_table), nsname, name_len)) == NULL) {
			efree(lcname);
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Function %s() does not exist", name_str);
			return;
		}
		efree(lcname);
	}

	ZVAL_STR_COPY(&name, fptr->common.function_name);
	reflection_update_property(object, "name", &name);
	intern->ptr = fptr;
	intern->ref_type = REF_TYPE_FUNCTION;
	if (closure) {
		ZVAL_COPY_VALUE(&intern->obj, closure);
	} else {
		ZVAL_UNDEF(&intern->obj);
	}
	intern->ce = NULL;
}
/* }}} */

/* {{{ proto public string ReflectionFunction::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_function, __toString)
{
	reflection_object *intern;
	zend_function *fptr;
	string str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	string_init(&str);
	_function_string(&str, fptr, intern->ce, "");
	RETURN_NEW_STR(str.buf);
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getName()
   Returns this function's name */
ZEND_METHOD(reflection_function, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_entry(getThis(), "name", sizeof("name")-1, return_value);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isClosure()
   Returns whether this is a closure */
ZEND_METHOD(reflection_function, isClosure)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->common.fn_flags & ZEND_ACC_CLOSURE);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::getClosureThis()
   Returns this pointer bound to closure */
ZEND_METHOD(reflection_function, getClosureThis)
{
	reflection_object *intern;
	zval* closure_this;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT();
	if (!Z_ISUNDEF(intern->obj)) {
		closure_this = zend_get_closure_this_ptr(&intern->obj);
		if (!Z_ISUNDEF_P(closure_this)) {
			ZVAL_COPY(return_value, closure_this);
		}
	}
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionFunction::getClosureScopeClass()
   Returns the scope associated to the closure */
ZEND_METHOD(reflection_function, getClosureScopeClass)
{
	reflection_object *intern;
	const zend_function *closure_func;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT();
	if (!Z_ISUNDEF(intern->obj)) {
		closure_func = zend_get_closure_method_def(&intern->obj);
		if (closure_func && closure_func->common.scope) {
			zend_reflection_class_factory(closure_func->common.scope, return_value);
		}
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionFunction::getClosure()
   Returns a dynamically created closure for the function */
ZEND_METHOD(reflection_function, getClosure)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);

	if (!Z_ISUNDEF(intern->obj)) {
		/* Closures are immutable objects */
		ZVAL_COPY(return_value, &intern->obj);
	} else {
		zend_create_fake_closure(return_value, fptr, NULL, NULL, NULL);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isInternal()
   Returns whether this is an internal function */
ZEND_METHOD(reflection_function, isInternal)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_INTERNAL_FUNCTION);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isUserDefined()
   Returns whether this is a user-defined function */
ZEND_METHOD(reflection_function, isUserDefined)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_USER_FUNCTION);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isDisabled()
   Returns whether this function has been disabled or not */
ZEND_METHOD(reflection_function, isDisabled)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC(reflection_function_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_INTERNAL_FUNCTION && fptr->internal_function.handler == zif_display_disabled_function);
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getFileName()
   Returns the filename of the file this function was declared in */
ZEND_METHOD(reflection_function, getFileName)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_STR_COPY(fptr->op_array.filename);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionFunction::getStartLine()
   Returns the line this function's declaration starts at */
ZEND_METHOD(reflection_function, getStartLine)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionFunction::getEndLine()
   Returns the line this function's declaration ends at */
ZEND_METHOD(reflection_function, getEndLine)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getDocComment()
   Returns the doc comment for this function */
ZEND_METHOD(reflection_function, getDocComment)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		RETURN_STR_COPY(fptr->op_array.doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public array ReflectionFunction::getStaticVariables()
   Returns an associative array containing this function's static variables and their values */
ZEND_METHOD(reflection_function, getStaticVariables)
{
	reflection_object *intern;
	zend_function *fptr;
	zval *val;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);

	/* Return an empty array in case no static variables exist */
	array_init(return_value);
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.static_variables != NULL) {
		if (GC_REFCOUNT(fptr->op_array.static_variables) > 1) {
			if (!(GC_FLAGS(fptr->op_array.static_variables) & IS_ARRAY_IMMUTABLE)) {
				GC_REFCOUNT(fptr->op_array.static_variables)--;
			}
			fptr->op_array.static_variables = zend_array_dup(fptr->op_array.static_variables);
		}
		ZEND_HASH_FOREACH_VAL(fptr->op_array.static_variables, val) {
			if (UNEXPECTED(zval_update_constant_ex(val, fptr->common.scope) != SUCCESS)) {
				return;
			}
		} ZEND_HASH_FOREACH_END();
		zend_hash_copy(Z_ARRVAL_P(return_value), fptr->op_array.static_variables, zval_add_ref);
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionFunction::invoke([mixed* args])
   Invokes the function */
ZEND_METHOD(reflection_function, invoke)
{
	zval retval;
	zval *params = NULL;
	int result, num_args = 0;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC(reflection_function_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &params, &num_args) == FAILURE) {
		return;
	}

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = num_args;
	fci.params = params;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = fptr;
	fcc.calling_scope = zend_get_executed_scope();
	fcc.called_scope = NULL;
	fcc.object = NULL;

	result = zend_call_function(&fci, &fcc);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of function %s() failed", ZSTR_VAL(fptr->common.function_name));
		return;
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionFunction::invokeArgs(array args)
   Invokes the function and pass its arguments as array. */
ZEND_METHOD(reflection_function, invokeArgs)
{
	zval retval;
	zval *params, *val;
	int result;
	int i, argc;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	reflection_object *intern;
	zend_function *fptr;
	zval *param_array;

	METHOD_NOTSTATIC(reflection_function_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &param_array) == FAILURE) {
		return;
	}

	argc = zend_hash_num_elements(Z_ARRVAL_P(param_array));

	params = safe_emalloc(sizeof(zval), argc, 0);
	argc = 0;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(param_array), val) {
		ZVAL_COPY(&params[argc], val);
		argc++;
	} ZEND_HASH_FOREACH_END();

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = argc;
	fci.params = params;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = fptr;
	fcc.calling_scope = zend_get_executed_scope();
	fcc.called_scope = NULL;
	fcc.object = NULL;

	result = zend_call_function(&fci, &fcc);

	for (i = 0; i < argc; i++) {
		zval_ptr_dtor(&params[i]);
	}
	efree(params);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of function %s() failed", ZSTR_VAL(fptr->common.function_name));
		return;
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::returnsReference()
   Gets whether this function returns a reference */
ZEND_METHOD(reflection_function, returnsReference)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC(reflection_function_abstract_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	RETURN_BOOL((fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::getNumberOfParameters()
   Gets the number of required parameters */
ZEND_METHOD(reflection_function, getNumberOfParameters)
{
	reflection_object *intern;
	zend_function *fptr;
	uint32_t num_args;

	METHOD_NOTSTATIC(reflection_function_abstract_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}

	RETURN_LONG(num_args);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::getNumberOfRequiredParameters()
   Gets the number of required parameters */
ZEND_METHOD(reflection_function, getNumberOfRequiredParameters)
{
	reflection_object *intern;
	zend_function *fptr;

	METHOD_NOTSTATIC(reflection_function_abstract_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	RETURN_LONG(fptr->common.required_num_args);
}
/* }}} */

/* {{{ proto public ReflectionParameter[] ReflectionFunction::getParameters()
   Returns an array of parameter objects for this function */
ZEND_METHOD(reflection_function, getParameters)
{
	reflection_object *intern;
	zend_function *fptr;
	uint32_t i, num_args;
	struct _zend_arg_info *arg_info;

	METHOD_NOTSTATIC(reflection_function_abstract_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	arg_info= fptr->common.arg_info;
	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}

	array_init(return_value);
	for (i = 0; i < num_args; i++) {
		zval parameter;

		reflection_parameter_factory(_copy_function(fptr), Z_ISUNDEF(intern->obj)? NULL : &intern->obj, arg_info, i, fptr->common.required_num_args, &parameter);
		add_next_index_zval(return_value, &parameter);

		arg_info++;
	}
}
/* }}} */

/* {{{ proto public ReflectionExtension|NULL ReflectionFunction::getExtension()
   Returns NULL or the extension the function belongs to */
ZEND_METHOD(reflection_function, getExtension)
{
	reflection_object *intern;
	zend_function *fptr;
	zend_internal_function *internal;

	METHOD_NOTSTATIC(reflection_function_abstract_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	if (fptr->type != ZEND_INTERNAL_FUNCTION) {
		RETURN_NULL();
	}

	internal = (zend_internal_function *)fptr;
	if (internal->module) {
		reflection_extension_factory(return_value, internal->module->name);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto public string|false ReflectionFunction::getExtensionName()
   Returns false or the name of the extension the function belongs to */
ZEND_METHOD(reflection_function, getExtensionName)
{
	reflection_object *intern;
	zend_function *fptr;
	zend_internal_function *internal;

	METHOD_NOTSTATIC(reflection_function_abstract_ptr);
	GET_REFLECTION_OBJECT_PTR(fptr);

	if (fptr->type != ZEND_INTERNAL_FUNCTION) {
		RETURN_FALSE;
	}

	internal = (zend_internal_function *)fptr;
	if (internal->module) {
		RETURN_STRING(internal->module->name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public void ReflectionGenerator::__construct(object Generator) */
ZEND_METHOD(reflection_generator, __construct)
{
	zval *generator, *object;
	reflection_object *intern;
	zend_execute_data *ex;

	object = getThis();
	intern = Z_REFLECTION_P(object);

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "O", &generator, zend_ce_generator) == FAILURE) {
		return;
	}

	ex = ((zend_generator *) Z_OBJ_P(generator))->execute_data;
	if (!ex) {
		zend_throw_exception(NULL, "Cannot create ReflectionGenerator based on a terminated Generator", 0);
		return;
	}

	intern->ref_type = REF_TYPE_GENERATOR;
	ZVAL_COPY(&intern->obj, generator);
	intern->ce = zend_ce_generator;
}
/* }}} */

#define REFLECTION_CHECK_VALID_GENERATOR(ex) \
	if (!ex) { \
		zend_throw_exception(NULL, "Cannot fetch information from a terminated Generator", 0); \
		return; \
	}

/* {{{ proto public array ReflectionGenerator::getTrace($options = DEBUG_BACKTRACE_PROVIDE_OBJECT) */
ZEND_METHOD(reflection_generator, getTrace)
{
	zend_long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(getThis())->obj);
	zend_generator *root_generator;
	zend_execute_data *ex_backup = EG(current_execute_data);
	zend_execute_data *ex = generator->execute_data;
	zend_execute_data *root_prev = NULL, *cur_prev;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &options) == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	root_generator = zend_generator_get_current(generator);

	cur_prev = generator->execute_data->prev_execute_data;
	if (generator == root_generator) {
		generator->execute_data->prev_execute_data = NULL;
	} else {
		root_prev = root_generator->execute_data->prev_execute_data;
		generator->execute_fake.prev_execute_data = NULL;
		root_generator->execute_data->prev_execute_data = &generator->execute_fake;
	}

	EG(current_execute_data) = root_generator->execute_data;
	zend_fetch_debug_backtrace(return_value, 0, options, 0);
	EG(current_execute_data) = ex_backup;

	root_generator->execute_data->prev_execute_data = root_prev;
	generator->execute_data->prev_execute_data = cur_prev;
}
/* }}} */

/* {{{ proto public int ReflectionGenerator::getExecutingLine() */
ZEND_METHOD(reflection_generator, getExecutingLine)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(getThis())->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	ZVAL_LONG(return_value, ex->opline->lineno);
}
/* }}} */

/* {{{ proto public string ReflectionGenerator::getExecutingFile() */
ZEND_METHOD(reflection_generator, getExecutingFile)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(getThis())->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	ZVAL_STR_COPY(return_value, ex->func->op_array.filename);
}
/* }}} */

/* {{{ proto public ReflectionFunctionAbstract ReflectionGenerator::getFunction() */
ZEND_METHOD(reflection_generator, getFunction)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(getThis())->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	if (ex->func->common.fn_flags & ZEND_ACC_CLOSURE) {
		zval closure;
		ZVAL_OBJ(&closure, (zend_object *) ex->func->common.prototype);
		reflection_function_factory(ex->func, &closure, return_value);
	} else if (ex->func->op_array.scope) {
		reflection_method_factory(ex->func->op_array.scope, ex->func, NULL, return_value);
	} else {
		reflection_function_factory(ex->func, NULL, return_value);
	}
}
/* }}} */

/* {{{ proto public object ReflectionGenerator::getThis() */
ZEND_METHOD(reflection_generator, getThis)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(getThis())->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	if (Z_TYPE(ex->This) == IS_OBJECT) {
		ZVAL_COPY(return_value, &ex->This);
	} else {
		ZVAL_NULL(return_value);
	}
}
/* }}} */

/* {{{ proto public Generator ReflectionGenerator::getExecutingGenerator() */
ZEND_METHOD(reflection_generator, getExecutingGenerator)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(getThis())->obj);
	zend_execute_data *ex = generator->execute_data;
	zend_generator *current;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	current = zend_generator_get_current(generator);
	++GC_REFCOUNT(&current->std);

	ZVAL_OBJ(return_value, (zend_object *) current);
}
/* }}} */


/* {{{ proto public static mixed ReflectionParameter::export(mixed function, mixed parameter [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_parameter, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_parameter_ptr, 2);
}
/* }}} */

/* {{{ proto public void ReflectionParameter::__construct(mixed function, mixed parameter)
   Constructor. Throws an Exception in case the given method does not exist */
ZEND_METHOD(reflection_parameter, __construct)
{
	parameter_reference *ref;
	zval *reference, *parameter;
	zval *object;
	zval name;
	reflection_object *intern;
	zend_function *fptr;
	struct _zend_arg_info *arg_info;
	int position;
	uint32_t num_args;
	zend_class_entry *ce = NULL;
	zend_bool is_closure = 0;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zz", &reference, &parameter) == FAILURE) {
		return;
	}

	object = getThis();
	intern = Z_REFLECTION_P(object);

	/* First, find the function */
	switch (Z_TYPE_P(reference)) {
		case IS_STRING: {
				size_t lcname_len;
				char *lcname;

				lcname_len = Z_STRLEN_P(reference);
				lcname = zend_str_tolower_dup(Z_STRVAL_P(reference), lcname_len);
				if ((fptr = zend_hash_str_find_ptr(EG(function_table), lcname, lcname_len)) == NULL) {
					efree(lcname);
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Function %s() does not exist", Z_STRVAL_P(reference));
					return;
				}
				efree(lcname);
			}
			ce = fptr->common.scope;
			break;

		case IS_ARRAY: {
				zval *classref;
				zval *method;
				size_t lcname_len;
				char *lcname;

				if (((classref =zend_hash_index_find(Z_ARRVAL_P(reference), 0)) == NULL)
					|| ((method = zend_hash_index_find(Z_ARRVAL_P(reference), 1)) == NULL))
				{
					_DO_THROW("Expected array($object, $method) or array($classname, $method)");
					/* returns out of this function */
				}

				if (Z_TYPE_P(classref) == IS_OBJECT) {
					ce = Z_OBJCE_P(classref);
				} else {
					convert_to_string_ex(classref);
					if ((ce = zend_lookup_class(Z_STR_P(classref))) == NULL) {
						zend_throw_exception_ex(reflection_exception_ptr, 0,
								"Class %s does not exist", Z_STRVAL_P(classref));
						return;
					}
				}

				convert_to_string_ex(method);
				lcname_len = Z_STRLEN_P(method);
				lcname = zend_str_tolower_dup(Z_STRVAL_P(method), lcname_len);
				if (ce == zend_ce_closure && Z_TYPE_P(classref) == IS_OBJECT
					&& (lcname_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
					&& memcmp(lcname, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
					&& (fptr = zend_get_closure_invoke_method(Z_OBJ_P(classref))) != NULL)
				{
					/* nothing to do. don't set is_closure since is the invoke handler,
					   not the closure itself */
				} else if ((fptr = zend_hash_str_find_ptr(&ce->function_table, lcname, lcname_len)) == NULL) {
					efree(lcname);
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Method %s::%s() does not exist", ZSTR_VAL(ce->name), Z_STRVAL_P(method));
					return;
				}
				efree(lcname);
			}
			break;

		case IS_OBJECT: {
				ce = Z_OBJCE_P(reference);

				if (instanceof_function(ce, zend_ce_closure)) {
					fptr = (zend_function *)zend_get_closure_method_def(reference);
					Z_ADDREF_P(reference);
					is_closure = 1;
				} else if ((fptr = zend_hash_str_find_ptr(&ce->function_table, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME))) == NULL) {
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Method %s::%s() does not exist", ZSTR_VAL(ce->name), ZEND_INVOKE_FUNC_NAME);
					return;
				}
			}
			break;

		default:
			_DO_THROW("The parameter class is expected to be either a string, an array(class, method) or a callable object");
			/* returns out of this function */
	}

	/* Now, search for the parameter */
	arg_info = fptr->common.arg_info;
	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}
	if (Z_TYPE_P(parameter) == IS_LONG) {
		position= (int)Z_LVAL_P(parameter);
		if (position < 0 || (uint32_t)position >= num_args) {
			if (fptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
				if (fptr->type != ZEND_OVERLOADED_FUNCTION) {
					zend_string_release(fptr->common.function_name);
				}
				zend_free_trampoline(fptr);
			}
			if (is_closure) {
				zval_ptr_dtor(reference);
			}
			_DO_THROW("The parameter specified by its offset could not be found");
			/* returns out of this function */
		}
	} else {
		uint32_t i;

		position= -1;
		convert_to_string_ex(parameter);
		if (fptr->type == ZEND_INTERNAL_FUNCTION &&
		    !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
			for (i = 0; i < num_args; i++) {
				if (arg_info[i].name) {
					if (strcmp(((zend_internal_arg_info*)arg_info)[i].name, Z_STRVAL_P(parameter)) == 0) {
						position= i;
						break;
					}

				}
			}
		} else {
			for (i = 0; i < num_args; i++) {
				if (arg_info[i].name) {
					if (strcmp(ZSTR_VAL(arg_info[i].name), Z_STRVAL_P(parameter)) == 0) {
						position= i;
						break;
					}
				}
			}
		}
		if (position == -1) {
			if (fptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
				if (fptr->type != ZEND_OVERLOADED_FUNCTION) {
					zend_string_release(fptr->common.function_name);
				}
				zend_free_trampoline(fptr);
			}
			if (is_closure) {
				zval_ptr_dtor(reference);
			}
			_DO_THROW("The parameter specified by its name could not be found");
			/* returns out of this function */
		}
	}

	if (arg_info[position].name) {
		if (fptr->type == ZEND_INTERNAL_FUNCTION &&
		    !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
			ZVAL_STRING(&name, ((zend_internal_arg_info*)arg_info)[position].name);
		} else {
			ZVAL_STR_COPY(&name, arg_info[position].name);
		}
	} else {
		ZVAL_NULL(&name);
	}
	reflection_update_property(object, "name", &name);

	ref = (parameter_reference*) emalloc(sizeof(parameter_reference));
	ref->arg_info = &arg_info[position];
	ref->offset = (uint32_t)position;
	ref->required = fptr->common.required_num_args;
	ref->fptr = fptr;
	/* TODO: copy fptr */
	intern->ptr = ref;
	intern->ref_type = REF_TYPE_PARAMETER;
	intern->ce = ce;
	if (reference && is_closure) {
		ZVAL_COPY_VALUE(&intern->obj, reference);
	}
}
/* }}} */

/* {{{ proto public string ReflectionParameter::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_parameter, __toString)
{
	reflection_object *intern;
	parameter_reference *param;
	string str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);
	string_init(&str);
	_parameter_string(&str, param->fptr, param->arg_info, param->offset, param->required, "");
	RETURN_NEW_STR(str.buf);
}

/* }}} */

/* {{{ proto public string ReflectionParameter::getName()
   Returns this parameters's name */
ZEND_METHOD(reflection_parameter, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_entry(getThis(), "name", sizeof("name")-1, return_value);
}
/* }}} */

/* {{{ proto public ReflectionFunction ReflectionParameter::getDeclaringFunction()
   Returns the ReflectionFunction for the function of this parameter */
ZEND_METHOD(reflection_parameter, getDeclaringFunction)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (!param->fptr->common.scope) {
		reflection_function_factory(_copy_function(param->fptr), Z_ISUNDEF(intern->obj)? NULL : &intern->obj, return_value);
	} else {
		reflection_method_factory(param->fptr->common.scope, _copy_function(param->fptr), Z_ISUNDEF(intern->obj)? NULL : &intern->obj, return_value);
	}
}
/* }}} */

/* {{{ proto public ReflectionClass|NULL ReflectionParameter::getDeclaringClass()
   Returns in which class this parameter is defined (not the type of the parameter) */
ZEND_METHOD(reflection_parameter, getDeclaringClass)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (param->fptr->common.scope) {
		zend_reflection_class_factory(param->fptr->common.scope, return_value);
	}
}
/* }}} */

/* {{{ proto public ReflectionClass|NULL ReflectionParameter::getClass()
   Returns this parameters's class hint or NULL if there is none */
ZEND_METHOD(reflection_parameter, getClass)
{
	reflection_object *intern;
	parameter_reference *param;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (param->arg_info->class_name) {
		/* Class name is stored as a string, we might also get "self" or "parent"
		 * - For "self", simply use the function scope. If scope is NULL then
		 *   the function is global and thus self does not make any sense
		 *
		 * - For "parent", use the function scope's parent. If scope is NULL then
		 *   the function is global and thus parent does not make any sense.
		 *   If the parent is NULL then the class does not extend anything and
		 *   thus parent does not make any sense, either.
		 *
		 * TODO: Think about moving these checks to the compiler or some sort of
		 * lint-mode.
		 */
		const char *class_name;
		size_t class_name_len;

		if (param->fptr->type == ZEND_INTERNAL_FUNCTION &&
		    !(param->fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
			class_name = ((zend_internal_arg_info*)param->arg_info)->class_name;
			class_name_len = strlen(class_name);
		} else {
			class_name = ZSTR_VAL(param->arg_info->class_name);
			class_name_len = ZSTR_LEN(param->arg_info->class_name);
		}
		if (0 == zend_binary_strcasecmp(class_name, class_name_len, "self", sizeof("self")- 1)) {
			ce = param->fptr->common.scope;
			if (!ce) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter uses 'self' as type hint but function is not a class member!");
				return;
			}
		} else if (0 == zend_binary_strcasecmp(class_name, class_name_len, "parent", sizeof("parent")- 1)) {
			ce = param->fptr->common.scope;
			if (!ce) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter uses 'parent' as type hint but function is not a class member!");
				return;
			}
			if (!ce->parent) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter uses 'parent' as type hint although class does not have a parent!");
				return;
			}
			ce = ce->parent;
		} else {
			if (param->fptr->type == ZEND_INTERNAL_FUNCTION &&
			    !(param->fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
				zend_string *name = zend_string_init(class_name, class_name_len, 0);
				ce = zend_lookup_class(name);
				zend_string_release(name);
			} else {
				ce = zend_lookup_class(param->arg_info->class_name);
			}
			if (!ce) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Class %s does not exist", class_name);
				return;
			}
		}
		zend_reflection_class_factory(ce, return_value);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::hasType()
   Returns whether parameter has a type */
ZEND_METHOD(reflection_parameter, hasType)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->type_hint != 0);
}
/* }}} */

/* {{{ proto public ReflectionType ReflectionParameter::getType()
   Returns the type associated with the parameter */
ZEND_METHOD(reflection_parameter, getType)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (((param->fptr->type == ZEND_INTERNAL_FUNCTION &&
	      !(param->fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) ?
		((zend_internal_arg_info*)param->arg_info)->type_hint :
		param->arg_info->type_hint) == 0)
	{
		RETURN_NULL();
	}
	reflection_type_factory(_copy_function(param->fptr), Z_ISUNDEF(intern->obj)? NULL : &intern->obj, param->arg_info, return_value);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isArray()
   Returns whether parameter MUST be an array */
ZEND_METHOD(reflection_parameter, isArray)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->type_hint == IS_ARRAY);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isCallable()
   Returns whether parameter MUST be callable */
ZEND_METHOD(reflection_parameter, isCallable)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->type_hint == IS_CALLABLE);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::allowsNull()
   Returns whether NULL is allowed as this parameters's value */
ZEND_METHOD(reflection_parameter, allowsNull)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->allow_null);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isPassedByReference()
   Returns whether this parameters is passed to by reference */
ZEND_METHOD(reflection_parameter, isPassedByReference)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->pass_by_reference);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::canBePassedByValue()
   Returns whether this parameter can be passed by value */
ZEND_METHOD(reflection_parameter, canBePassedByValue)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	/* true if it's ZEND_SEND_BY_VAL or ZEND_SEND_PREFER_REF */
	RETVAL_BOOL(param->arg_info->pass_by_reference != ZEND_SEND_BY_REF);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::getPosition()
   Returns whether this parameter is an optional parameter */
ZEND_METHOD(reflection_parameter, getPosition)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_LONG(param->offset);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isOptional()
   Returns whether this parameter is an optional parameter */
ZEND_METHOD(reflection_parameter, isOptional)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->offset >= param->required);
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isDefaultValueAvailable()
   Returns whether the default value of this parameter is available */
ZEND_METHOD(reflection_parameter, isDefaultValueAvailable)
{
	reflection_object *intern;
	parameter_reference *param;
	zend_op *precv;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (param->fptr->type != ZEND_USER_FUNCTION)
	{
		RETURN_FALSE;
	}

	precv = _get_recv_op((zend_op_array*)param->fptr, param->offset);
	if (!precv || precv->opcode != ZEND_RECV_INIT || precv->op2_type == IS_UNUSED) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::getDefaultValue()
   Returns the default value of this parameter or throws an exception */
ZEND_METHOD(reflection_parameter, getDefaultValue)
{
	parameter_reference *param;
	zend_op *precv;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	param = _reflection_param_get_default_param(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (!param) {
		return;
	}

	precv = _reflection_param_get_default_precv(INTERNAL_FUNCTION_PARAM_PASSTHRU, param);
	if (!precv) {
		return;
	}

	ZVAL_DUP(return_value, RT_CONSTANT(&param->fptr->op_array, precv->op2));
	if (Z_CONSTANT_P(return_value)) {
		zval_update_constant_ex(return_value, param->fptr->common.scope);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isDefaultValueConstant()
   Returns whether the default value of this parameter is constant */
ZEND_METHOD(reflection_parameter, isDefaultValueConstant)
{
	zend_op *precv;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	param = _reflection_param_get_default_param(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (!param) {
		RETURN_FALSE;
	}

	precv = _reflection_param_get_default_precv(INTERNAL_FUNCTION_PARAM_PASSTHRU, param);
	if (precv && Z_TYPE_P(RT_CONSTANT(&param->fptr->op_array, precv->op2)) == IS_CONSTANT) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public mixed ReflectionParameter::getDefaultValueConstantName()
   Returns the default value's constant name if default value is constant or null */
ZEND_METHOD(reflection_parameter, getDefaultValueConstantName)
{
	zend_op *precv;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	param = _reflection_param_get_default_param(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (!param) {
		return;
	}

	precv = _reflection_param_get_default_precv(INTERNAL_FUNCTION_PARAM_PASSTHRU, param);
	if (precv && Z_TYPE_P(RT_CONSTANT(&param->fptr->op_array, precv->op2)) == IS_CONSTANT) {
		RETURN_STR_COPY(Z_STR_P(RT_CONSTANT(&param->fptr->op_array, precv->op2)));
	}
}
/* }}} */

/* {{{ proto public bool ReflectionParameter::isVariadic()
   Returns whether this parameter is a variadic parameter */
ZEND_METHOD(reflection_parameter, isVariadic)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->is_variadic);
}
/* }}} */

/* {{{ proto public bool ReflectionType::allowsNull()
  Returns whether parameter MAY be null */
ZEND_METHOD(reflection_type, allowsNull)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->allow_null);
}
/* }}} */

/* {{{ proto public bool ReflectionType::isBuiltin()
  Returns whether parameter is a builtin type */
ZEND_METHOD(reflection_type, isBuiltin)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(param->arg_info->type_hint != IS_OBJECT);
}
/* }}} */

/* {{{ proto public string ReflectionType::__toString()
   Return the text of the type hint */
ZEND_METHOD(reflection_type, __toString)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	switch (param->arg_info->type_hint) {
		case IS_ARRAY:    RETURN_STRINGL("array", sizeof("array") - 1);
		case IS_CALLABLE: RETURN_STRINGL("callable", sizeof("callable") - 1);
		case IS_OBJECT:
			if (param->fptr->type == ZEND_INTERNAL_FUNCTION &&
			    !(param->fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
				RETURN_STRING(((zend_internal_arg_info*)param->arg_info)->class_name);
			}
			RETURN_STR_COPY(param->arg_info->class_name);
		case IS_STRING:   RETURN_STRINGL("string", sizeof("string") - 1);
		case _IS_BOOL:    RETURN_STRINGL("bool", sizeof("bool") - 1);
		case IS_LONG:     RETURN_STRINGL("int", sizeof("int") - 1);
		case IS_DOUBLE:   RETURN_STRINGL("float", sizeof("float") - 1);
		case IS_VOID:     RETURN_STRINGL("void", sizeof("void") - 1);
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}
/* }}} */

/* {{{ proto public static mixed ReflectionMethod::export(mixed class, string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_method, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_method_ptr, 2);
}
/* }}} */

/* {{{ proto public void ReflectionMethod::__construct(mixed class_or_method [, string name])
   Constructor. Throws an Exception in case the given method does not exist */
ZEND_METHOD(reflection_method, __construct)
{
	zval name, *classname;
	zval *object, *orig_obj;
	reflection_object *intern;
	char *lcname;
	zend_class_entry *ce;
	zend_function *mptr;
	char *name_str, *tmp;
	size_t name_len, tmp_len;
	zval ztmp;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "zs", &classname, &name_str, &name_len) == FAILURE) {
		if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &name_str, &name_len) == FAILURE) {
			return;
		}

		if ((tmp = strstr(name_str, "::")) == NULL) {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Invalid method name %s", name_str);
			return;
		}
		classname = &ztmp;
		tmp_len = tmp - name_str;
		ZVAL_STRINGL(classname, name_str, tmp_len);
		name_len = name_len - (tmp_len + 2);
		name_str = tmp + 2;
		orig_obj = NULL;
	} else if (Z_TYPE_P(classname) == IS_OBJECT) {
		orig_obj = classname;
	} else {
		orig_obj = NULL;
	}

	object = getThis();
	intern = Z_REFLECTION_P(object);

	/* Find the class entry */
	switch (Z_TYPE_P(classname)) {
		case IS_STRING:
			if ((ce = zend_lookup_class(Z_STR_P(classname))) == NULL) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Class %s does not exist", Z_STRVAL_P(classname));
				if (classname == &ztmp) {
					zval_dtor(&ztmp);
				}
				return;
			}
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_P(classname);
			break;

		default:
			if (classname == &ztmp) {
				zval_dtor(&ztmp);
			}
			_DO_THROW("The parameter class is expected to be either a string or an object");
			/* returns out of this function */
	}

	if (classname == &ztmp) {
		zval_dtor(&ztmp);
	}

	lcname = zend_str_tolower_dup(name_str, name_len);

	if (ce == zend_ce_closure && orig_obj && (name_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
		&& memcmp(lcname, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
		&& (mptr = zend_get_closure_invoke_method(Z_OBJ_P(orig_obj))) != NULL)
	{
		/* do nothing, mptr already set */
	} else if ((mptr = zend_hash_str_find_ptr(&ce->function_table, lcname, name_len)) == NULL) {
		efree(lcname);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Method %s::%s() does not exist", ZSTR_VAL(ce->name), name_str);
		return;
	}
	efree(lcname);

	ZVAL_STR_COPY(&name, mptr->common.scope->name);
	reflection_update_property(object, "class", &name);
	ZVAL_STR_COPY(&name, mptr->common.function_name);
	reflection_update_property(object, "name", &name);
	intern->ptr = mptr;
	intern->ref_type = REF_TYPE_FUNCTION;
	intern->ce = ce;
}
/* }}} */

/* {{{ proto public string ReflectionMethod::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_method, __toString)
{
	reflection_object *intern;
	zend_function *mptr;
	string str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	string_init(&str);
	_function_string(&str, mptr, intern->ce, "");
	RETURN_NEW_STR(str.buf);
}
/* }}} */

/* {{{ proto public mixed ReflectionMethod::getClosure([mixed object])
   Invokes the function */
ZEND_METHOD(reflection_method, getClosure)
{
	reflection_object *intern;
	zval *obj;
	zend_function *mptr;

	METHOD_NOTSTATIC(reflection_method_ptr);
	GET_REFLECTION_OBJECT_PTR(mptr);

	if (mptr->common.fn_flags & ZEND_ACC_STATIC)  {
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, mptr->common.scope, NULL);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
			return;
		}

		if (!instanceof_function(Z_OBJCE_P(obj), mptr->common.scope)) {
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			/* Returns from this function */
		}

		/* This is an original closure object and __invoke is to be called. */
		if (Z_OBJCE_P(obj) == zend_ce_closure &&
			(mptr->internal_function.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))
		{
			ZVAL_COPY(return_value, obj);
		} else {
			zend_create_fake_closure(return_value, mptr, mptr->common.scope, Z_OBJCE_P(obj), obj);
		}
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionMethod::invoke(mixed object, mixed* args)
   Invokes the method. */
ZEND_METHOD(reflection_method, invoke)
{
	zval retval;
	zval *params = NULL;
	zend_object *object;
	reflection_object *intern;
	zend_function *mptr;
	int result, num_args = 0;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_class_entry *obj_ce;

	METHOD_NOTSTATIC(reflection_method_ptr);

	GET_REFLECTION_OBJECT_PTR(mptr);

	if ((!(mptr->common.fn_flags & ZEND_ACC_PUBLIC)
		 || (mptr->common.fn_flags & ZEND_ACC_ABSTRACT))
		 && intern->ignore_visibility == 0)
	{
		if (mptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Trying to invoke abstract method %s::%s()",
				ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		} else {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Trying to invoke %s method %s::%s() from scope %s",
				mptr->common.fn_flags & ZEND_ACC_PROTECTED ? "protected" : "private",
				ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name),
				ZSTR_VAL(Z_OBJCE_P(getThis())->name));
		}
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &params, &num_args) == FAILURE) {
		return;
	}

	/* In case this is a static method, we should'nt pass an object_ptr
	 * (which is used as calling context aka $this). We can thus ignore the
	 * first parameter.
	 *
	 * Else, we verify that the given object is an instance of the class.
	 */
	if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
		object = NULL;
		obj_ce = mptr->common.scope;
	} else {
		if (Z_TYPE(params[0]) != IS_OBJECT) {
			_DO_THROW("Non-object passed to Invoke()");
			/* Returns from this function */
		}

		obj_ce = Z_OBJCE(params[0]);

		if (!instanceof_function(obj_ce, mptr->common.scope)) {
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			/* Returns from this function */
		}

		object = Z_OBJ(params[0]);
	}

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = object;
	fci.retval = &retval;
	fci.param_count = num_args - 1;
	fci.params = params + 1;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = mptr;
	fcc.calling_scope = obj_ce;
	fcc.called_scope = intern->ce;
	fcc.object = object;

	result = zend_call_function(&fci, &fcc);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of method %s::%s() failed", ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		return;
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionMethod::invokeArgs(mixed object, array args)
   Invokes the function and pass its arguments as array. */
ZEND_METHOD(reflection_method, invokeArgs)
{
	zval retval;
	zval *params, *val, *object;
	reflection_object *intern;
	zend_function *mptr;
	int i, argc;
	int result;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_class_entry *obj_ce;
	zval *param_array;

	METHOD_NOTSTATIC(reflection_method_ptr);

	GET_REFLECTION_OBJECT_PTR(mptr);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o!a", &object, &param_array) == FAILURE) {
		return;
	}

	if ((!(mptr->common.fn_flags & ZEND_ACC_PUBLIC)
		 || (mptr->common.fn_flags & ZEND_ACC_ABSTRACT))
		 && intern->ignore_visibility == 0)
	{
		if (mptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Trying to invoke abstract method %s::%s()",
				ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		} else {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Trying to invoke %s method %s::%s() from scope %s",
				mptr->common.fn_flags & ZEND_ACC_PROTECTED ? "protected" : "private",
				ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name),
				ZSTR_VAL(Z_OBJCE_P(getThis())->name));
		}
		return;
	}

	argc = zend_hash_num_elements(Z_ARRVAL_P(param_array));

	params = safe_emalloc(sizeof(zval), argc, 0);
	argc = 0;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(param_array), val) {
		ZVAL_COPY(&params[argc], val);
		argc++;
	} ZEND_HASH_FOREACH_END();

	/* In case this is a static method, we should'nt pass an object_ptr
	 * (which is used as calling context aka $this). We can thus ignore the
	 * first parameter.
	 *
	 * Else, we verify that the given object is an instance of the class.
	 */
	if (mptr->common.fn_flags & ZEND_ACC_STATIC) {
		object = NULL;
		obj_ce = mptr->common.scope;
	} else {
		if (!object) {
			efree(params);
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Trying to invoke non static method %s::%s() without an object",
				ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
			return;
		}

		obj_ce = Z_OBJCE_P(object);

		if (!instanceof_function(obj_ce, mptr->common.scope)) {
			efree(params);
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			/* Returns from this function */
		}
	}

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = object ? Z_OBJ_P(object) : NULL;
	fci.retval = &retval;
	fci.param_count = argc;
	fci.params = params;
	fci.no_separation = 1;

	fcc.initialized = 1;
	fcc.function_handler = mptr;
	fcc.calling_scope = obj_ce;
	fcc.called_scope = intern->ce;
	fcc.object = (object) ? Z_OBJ_P(object) : NULL;

	/*
	 * Copy the zend_function when calling via handler (e.g. Closure::__invoke())
	 */
	if ((mptr->internal_function.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		fcc.function_handler = _copy_function(mptr);
	}

	result = zend_call_function(&fci, &fcc);

	for (i = 0; i < argc; i++) {
		zval_ptr_dtor(&params[i]);
	}
	efree(params);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of method %s::%s() failed", ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		return;
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isFinal()
   Returns whether this method is final */
ZEND_METHOD(reflection_method, isFinal)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isAbstract()
   Returns whether this method is abstract */
ZEND_METHOD(reflection_method, isAbstract)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_ABSTRACT);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isPublic()
   Returns whether this method is public */
ZEND_METHOD(reflection_method, isPublic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isPrivate()
   Returns whether this method is private */
ZEND_METHOD(reflection_method, isPrivate)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isProtected()
   Returns whether this method is protected */
ZEND_METHOD(reflection_method, isProtected)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isStatic()
   Returns whether this method is static */
ZEND_METHOD(reflection_method, isStatic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isDeprecated()
   Returns whether this function is deprecated */
ZEND_METHOD(reflection_function, isDeprecated)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_DEPRECATED);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isGenerator()
   Returns whether this function is a generator */
ZEND_METHOD(reflection_function, isGenerator)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_GENERATOR);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::isVariadic()
   Returns whether this function is variadic */
ZEND_METHOD(reflection_function, isVariadic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_VARIADIC);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::inNamespace()
   Returns whether this function is defined in namespace */
ZEND_METHOD(reflection_function, inNamespace)
{
	zval *name;
	const char *backslash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if ((name = _default_load_entry(getThis(), "name", sizeof("name")-1)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getNamespaceName()
   Returns the name of namespace where this function is defined */
ZEND_METHOD(reflection_function, getNamespaceName)
{
	zval *name;
	const char *backslash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if ((name = _default_load_entry(getThis(), "name", sizeof("name")-1)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_STRINGL(Z_STRVAL_P(name), backslash - Z_STRVAL_P(name));
	}
	RETURN_EMPTY_STRING();
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getShortName()
   Returns the short name of the function (without namespace part) */
ZEND_METHOD(reflection_function, getShortName)
{
	zval *name;
	const char *backslash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if ((name = _default_load_entry(getThis(), "name", sizeof("name")-1)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_STRINGL(backslash + 1, Z_STRLEN_P(name) - (backslash - Z_STRVAL_P(name) + 1));
	}
	ZVAL_DEREF(name);
	ZVAL_COPY(return_value, name);
}
/* }}} */

/* {{{ proto public bool ReflectionFunctionAbstract:hasReturnType()
   Return whether the function has a return type */
ZEND_METHOD(reflection_function, hasReturnType)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(fptr);

	RETVAL_BOOL(fptr->op_array.fn_flags & ZEND_ACC_HAS_RETURN_TYPE);
}
/* }}} */

/* {{{ proto public string ReflectionFunctionAbstract::getReturnType()
   Returns the return type associated with the function */
ZEND_METHOD(reflection_function, getReturnType)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (!(fptr->op_array.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
		RETURN_NULL();
	}

	reflection_type_factory(_copy_function(fptr), Z_ISUNDEF(intern->obj)? NULL : &intern->obj, &fptr->common.arg_info[-1], return_value);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isConstructor()
   Returns whether this method is the constructor */
ZEND_METHOD(reflection_method, isConstructor)
{
	reflection_object *intern;
	zend_function *mptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	/* we need to check if the ctor is the ctor of the class level we we
	 * looking at since we might be looking at an inherited old style ctor
	 * defined in base class. */
	RETURN_BOOL(mptr->common.fn_flags & ZEND_ACC_CTOR && intern->ce->constructor && intern->ce->constructor->common.scope == mptr->common.scope);
}
/* }}} */

/* {{{ proto public bool ReflectionMethod::isDestructor()
   Returns whether this method is static */
ZEND_METHOD(reflection_method, isDestructor)
{
	reflection_object *intern;
	zend_function *mptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	RETURN_BOOL(mptr->common.fn_flags & ZEND_ACC_DTOR);
}
/* }}} */

/* {{{ proto public int ReflectionMethod::getModifiers()
   Returns a bitfield of the access modifiers for this method */
ZEND_METHOD(reflection_method, getModifiers)
{
	reflection_object *intern;
	zend_function *mptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(mptr);

	RETURN_LONG(mptr->common.fn_flags);
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionMethod::getDeclaringClass()
   Get the declaring class */
ZEND_METHOD(reflection_method, getDeclaringClass)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC(reflection_method_ptr);
	GET_REFLECTION_OBJECT_PTR(mptr);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_reflection_class_factory(mptr->common.scope, return_value);
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionMethod::getPrototype()
   Get the prototype */
ZEND_METHOD(reflection_method, getPrototype)
{
	reflection_object *intern;
	zend_function *mptr;

	METHOD_NOTSTATIC(reflection_method_ptr);
	GET_REFLECTION_OBJECT_PTR(mptr);

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (!mptr->common.prototype) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Method %s::%s does not have a prototype", ZSTR_VAL(intern->ce->name), ZSTR_VAL(mptr->common.function_name));
		return;
	}

	reflection_method_factory(mptr->common.prototype->common.scope, mptr->common.prototype, NULL, return_value);
}
/* }}} */

/* {{{ proto public void ReflectionMethod::setAccessible(bool visible)
   Sets whether non-public methods can be invoked */
ZEND_METHOD(reflection_method, setAccessible)
{
	reflection_object *intern;
	zend_bool visible;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &visible) == FAILURE) {
		return;
	}

	intern = Z_REFLECTION_P(getThis());

	intern->ignore_visibility = visible;
}
/* }}} */

/* {{{ proto public void ReflectionClassConstant::__construct(mixed class, string name)
   Constructor. Throws an Exception in case the given class constant does not exist */
ZEND_METHOD(reflection_class_constant, __construct)
{
	zval *classname, *object, name, cname;
	zend_string *constname;
	reflection_object *intern;
	zend_class_entry *ce;
	zend_class_constant *constant = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zS", &classname, &constname) == FAILURE) {
		return;
	}

	object = getThis();
	intern = Z_REFLECTION_P(object);

	/* Find the class entry */
	switch (Z_TYPE_P(classname)) {
		case IS_STRING:
			if ((ce = zend_lookup_class(Z_STR_P(classname))) == NULL) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Class %s does not exist", Z_STRVAL_P(classname));
				return;
			}
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_P(classname);
			break;

		default:
			_DO_THROW("The parameter class is expected to be either a string or an object");
			/* returns out of this function */
	}

	if ((constant = zend_hash_find_ptr(&ce->constants_table, constname)) == NULL) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class Constant %s::%s does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(constname));
		return;
	}

	ZVAL_STR_COPY(&name, constname);
	ZVAL_STR_COPY(&cname, ce->name);

	intern->ptr = constant;
	intern->ref_type = REF_TYPE_CLASS_CONSTANT;
	intern->ce = constant->ce;
	intern->ignore_visibility = 0;
	reflection_update_property(object, "name", &name);
	reflection_update_property(object, "class", &cname);
}
/* }}} */

/* {{{ proto public string ReflectionClassConstant::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_class_constant, __toString)
{
	reflection_object *intern;
	zend_class_constant *ref;
	string str;
	zval name;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	string_init(&str);
	_default_get_entry(getThis(), "name", sizeof("name")-1, &name);
	_class_const_string(&str, Z_STRVAL(name), ref, "");
	zval_ptr_dtor(&name);
	RETURN_NEW_STR(str.buf);
}
/* }}} */

/* {{{ proto public string ReflectionClassConstant::getName()
   Returns the constant' name */
ZEND_METHOD(reflection_class_constant, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_entry(getThis(), "name", sizeof("name")-1, return_value);
}
/* }}} */

static void _class_constant_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask) /* {{{ */
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(Z_ACCESS_FLAGS(ref->value) & mask);
}
/* }}} */

/* {{{ proto public bool ReflectionClassConstant::isPublic()
   Returns whether this constant is public */
ZEND_METHOD(reflection_class_constant, isPublic)
{
	_class_constant_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC | ZEND_ACC_IMPLICIT_PUBLIC);
}
/* }}} */

/* {{{ proto public bool ReflectionClassConstant::isPrivate()
   Returns whether this constant is private */
ZEND_METHOD(reflection_class_constant, isPrivate)
{
	_class_constant_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ proto public bool ReflectionClassConstant::isProtected()
   Returns whether this constant is protected */
ZEND_METHOD(reflection_class_constant, isProtected)
{
	_class_constant_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ proto public int ReflectionClassConstant::getModifiers()
   Returns a bitfield of the access modifiers for this constant */
ZEND_METHOD(reflection_class_constant, getModifiers)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	RETURN_LONG(Z_ACCESS_FLAGS(ref->value));
}
/* }}} */

/* {{{ proto public mixed ReflectionClassConstant::getValue()
   Returns this constant's value */
ZEND_METHOD(reflection_class_constant, getValue)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	ZVAL_DUP(return_value, &ref->value);
	if (Z_CONSTANT_P(return_value)) {
		zval_update_constant_ex(return_value, ref->ce);
	}
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionClassConstant::getDeclaringClass()
   Get the declaring class */
ZEND_METHOD(reflection_class_constant, getDeclaringClass)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	zend_reflection_class_factory(ref->ce, return_value);
}
/* }}} */

/* {{{ proto public string ReflectionClassConstant::getDocComment()
   Returns the doc comment for this constant */
ZEND_METHOD(reflection_class_constant, getDocComment)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	if (ref->doc_comment) {
		RETURN_STR_COPY(ref->doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public static mixed ReflectionClass::export(mixed argument [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_class, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_class_ptr, 1);
}
/* }}} */

/* {{{ reflection_class_object_ctor */
static void reflection_class_object_ctor(INTERNAL_FUNCTION_PARAMETERS, int is_object)
{
	zval *argument;
	zval *object;
	zval classname;
	reflection_object *intern;
	zend_class_entry *ce;

	if (is_object) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &argument) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "z/", &argument) == FAILURE) {
			return;
		}
	}

	object = getThis();
	intern = Z_REFLECTION_P(object);

	if (Z_TYPE_P(argument) == IS_OBJECT) {
		ZVAL_STR_COPY(&classname, Z_OBJCE_P(argument)->name);
		reflection_update_property(object, "name", &classname);
		intern->ptr = Z_OBJCE_P(argument);
		if (is_object) {
			ZVAL_COPY_VALUE(&intern->obj, argument);
			zval_add_ref(argument);
		}
	} else {
		convert_to_string_ex(argument);
		if ((ce = zend_lookup_class(Z_STR_P(argument))) == NULL) {
			if (!EG(exception)) {
				zend_throw_exception_ex(reflection_exception_ptr, -1, "Class %s does not exist", Z_STRVAL_P(argument));
			}
			return;
		}

		ZVAL_STR_COPY(&classname, ce->name);
		reflection_update_property(object, "name", &classname);

		intern->ptr = ce;
	}
	intern->ref_type = REF_TYPE_OTHER;
}
/* }}} */

/* {{{ proto public void ReflectionClass::__construct(mixed argument) throws ReflectionException
   Constructor. Takes a string or an instance as an argument */
ZEND_METHOD(reflection_class, __construct)
{
	reflection_class_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ add_class_vars */
static void add_class_vars(zend_class_entry *ce, int statics, zval *return_value)
{
	zend_property_info *prop_info;
	zval *prop, prop_copy;
	zend_string *key;

	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->properties_info, key, prop_info) {
		if (((prop_info->flags & ZEND_ACC_SHADOW) &&
		     prop_info->ce != ce) ||
		    ((prop_info->flags & ZEND_ACC_PROTECTED) &&
		     !zend_check_protected(prop_info->ce, ce)) ||
		    ((prop_info->flags & ZEND_ACC_PRIVATE) &&
		     prop_info->ce != ce)) {
			continue;
		}
		prop = NULL;
		if (statics && (prop_info->flags & ZEND_ACC_STATIC) != 0) {
			prop = &ce->default_static_members_table[prop_info->offset];
		} else if (!statics && (prop_info->flags & ZEND_ACC_STATIC) == 0) {
			prop = &ce->default_properties_table[OBJ_PROP_TO_NUM(prop_info->offset)];
		}
		if (!prop) {
			continue;
		}

		/* copy: enforce read only access */
		ZVAL_DEREF(prop);
		ZVAL_DUP(&prop_copy, prop);

		/* this is necessary to make it able to work with default array
		* properties, returned to user */
		if (Z_CONSTANT(prop_copy)) {
			if (UNEXPECTED(zval_update_constant_ex(&prop_copy, NULL) != SUCCESS)) {
				return;
			}
		}

		zend_hash_update(Z_ARRVAL_P(return_value), key, &prop_copy);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto public array ReflectionClass::getStaticProperties()
   Returns an associative array containing all static property values of the class */
ZEND_METHOD(reflection_class, getStaticProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		return;
	}

	array_init(return_value);
	add_class_vars(ce, 1, return_value);
}
/* }}} */

/* {{{ proto public mixed ReflectionClass::getStaticPropertyValue(string name [, mixed default])
   Returns the value of a static property */
ZEND_METHOD(reflection_class, getStaticPropertyValue)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *name;
	zval *prop, *def_value = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def_value) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		return;
	}
	prop = zend_std_get_static_property(ce, name, 1);
	if (!prop) {
		if (def_value) {
			ZVAL_COPY(return_value, def_value);
		} else {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Class %s does not have a property named %s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}
		return;
	} else {
		ZVAL_DEREF(prop);
		ZVAL_COPY(return_value, prop);
	}
}
/* }}} */

/* {{{ proto public void ReflectionClass::setStaticPropertyValue(string $name, mixed $value)
   Sets the value of a static property */
ZEND_METHOD(reflection_class, setStaticPropertyValue)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *name;
	zval *variable_ptr, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		return;
	}
	variable_ptr = zend_std_get_static_property(ce, name, 1);
	if (!variable_ptr) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Class %s does not have a property named %s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		return;
	}
	ZVAL_DEREF(variable_ptr);
	zval_ptr_dtor(variable_ptr);
	ZVAL_COPY(variable_ptr, value);
}
/* }}} */

/* {{{ proto public array ReflectionClass::getDefaultProperties()
   Returns an associative array containing copies of all default property values of the class */
ZEND_METHOD(reflection_class, getDefaultProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);
	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		return;
	}
	add_class_vars(ce, 1, return_value);
	add_class_vars(ce, 0, return_value);
}
/* }}} */

/* {{{ proto public string ReflectionClass::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_class, __toString)
{
	reflection_object *intern;
	zend_class_entry *ce;
	string str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	string_init(&str);
	_class_string(&str, ce, &intern->obj, "");
	RETURN_NEW_STR(str.buf);
}
/* }}} */

/* {{{ proto public string ReflectionClass::getName()
   Returns the class' name */
ZEND_METHOD(reflection_class, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_entry(getThis(), "name", sizeof("name")-1, return_value);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInternal()
   Returns whether this class is an internal class */
ZEND_METHOD(reflection_class, isInternal)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_INTERNAL_CLASS);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isUserDefined()
   Returns whether this class is user-defined */
ZEND_METHOD(reflection_class, isUserDefined)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_USER_CLASS);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isAnonymous()
   Returns whether this class is anonymous */
ZEND_METHOD(reflection_class, isAnonymous)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->ce_flags & ZEND_ACC_ANON_CLASS);
}
/* }}} */

/* {{{ proto public string ReflectionClass::getFileName()
   Returns the filename of the file this class was declared in */
ZEND_METHOD(reflection_class, getFileName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_STR_COPY(ce->info.user.filename);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionClass::getStartLine()
   Returns the line this class' declaration starts at */
ZEND_METHOD(reflection_class, getStartLine)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(ce->info.user.line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionClass::getEndLine()
   Returns the line this class' declaration ends at */
ZEND_METHOD(reflection_class, getEndLine)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_LONG(ce->info.user.line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public string ReflectionClass::getDocComment()
   Returns the doc comment for this class */
ZEND_METHOD(reflection_class, getDocComment)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS && ce->info.user.doc_comment) {
		RETURN_STR_COPY(ce->info.user.doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public ReflectionMethod ReflectionClass::getConstructor()
   Returns the class' constructor if there is one, NULL otherwise */
ZEND_METHOD(reflection_class, getConstructor)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->constructor) {
		reflection_method_factory(ce, ce->constructor, NULL, return_value);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ proto public bool ReflectionClass::hasMethod(string name)
   Returns whether a method exists or not */
ZEND_METHOD(reflection_class, hasMethod)
{
	reflection_object *intern;
	zend_class_entry *ce;
	char *name, *lc_name;
	size_t name_len;

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	lc_name = zend_str_tolower_dup(name, name_len);
	if ((ce == zend_ce_closure && (name_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
		&& memcmp(lc_name, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0)
		|| zend_hash_str_exists(&ce->function_table, lc_name, name_len)) {
		efree(lc_name);
		RETURN_TRUE;
	} else {
		efree(lc_name);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public ReflectionMethod ReflectionClass::getMethod(string name) throws ReflectionException
   Returns the class' method specified by its name */
ZEND_METHOD(reflection_class, getMethod)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_function *mptr;
	zval obj_tmp;
	char *name, *lc_name;
	size_t name_len;

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	lc_name = zend_str_tolower_dup(name, name_len);
	if (ce == zend_ce_closure && !Z_ISUNDEF(intern->obj) && (name_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
		&& memcmp(lc_name, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
		&& (mptr = zend_get_closure_invoke_method(Z_OBJ(intern->obj))) != NULL)
	{
		/* don't assign closure_object since we only reflect the invoke handler
		   method and not the closure definition itself */
		reflection_method_factory(ce, mptr, NULL, return_value);
		efree(lc_name);
	} else if (ce == zend_ce_closure && Z_ISUNDEF(intern->obj) && (name_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
		&& memcmp(lc_name, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
		&& object_init_ex(&obj_tmp, ce) == SUCCESS && (mptr = zend_get_closure_invoke_method(Z_OBJ(obj_tmp))) != NULL) {
		/* don't assign closure_object since we only reflect the invoke handler
		   method and not the closure definition itself */
		reflection_method_factory(ce, mptr, NULL, return_value);
		zval_dtor(&obj_tmp);
		efree(lc_name);
	} else if ((mptr = zend_hash_str_find_ptr(&ce->function_table, lc_name, name_len)) != NULL) {
		reflection_method_factory(ce, mptr, NULL, return_value);
		efree(lc_name);
	} else {
		efree(lc_name);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Method %s does not exist", name);
		return;
	}
}
/* }}} */

/* {{{ _addmethod */
static void _addmethod(zend_function *mptr, zend_class_entry *ce, zval *retval, zend_long filter, zval *obj)
{
	zval method;
	size_t len = ZSTR_LEN(mptr->common.function_name);
	zend_function *closure;
	if (mptr->common.fn_flags & filter) {
		if (ce == zend_ce_closure && obj && (len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
			&& memcmp(ZSTR_VAL(mptr->common.function_name), ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
			&& (closure = zend_get_closure_invoke_method(Z_OBJ_P(obj))) != NULL)
		{
			mptr = closure;
		}
		/* don't assign closure_object since we only reflect the invoke handler
		   method and not the closure definition itself, even if we have a
		   closure */
		reflection_method_factory(ce, mptr, NULL, &method);
		add_next_index_zval(retval, &method);
	}
}
/* }}} */

/* {{{ _addmethod */
static int _addmethod_va(zval *el, int num_args, va_list args, zend_hash_key *hash_key)
{
	zend_function *mptr = (zend_function*)Z_PTR_P(el);
	zend_class_entry *ce = *va_arg(args, zend_class_entry**);
	zval *retval = va_arg(args, zval*);
	long filter = va_arg(args, long);
	zval *obj = va_arg(args, zval *);

	_addmethod(mptr, ce, retval, filter, obj);
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto public ReflectionMethod[] ReflectionClass::getMethods([long $filter])
   Returns an array of this class' methods */
ZEND_METHOD(reflection_class, getMethods)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_long filter = 0;
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (argc) {
		if (zend_parse_parameters(argc, "|l", &filter) == FAILURE) {
			return;
		}
	} else {
		/* No parameters given, default to "return all" */
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	zend_hash_apply_with_arguments(&ce->function_table, (apply_func_args_t) _addmethod_va, 4, &ce, return_value, filter, intern->obj);
	if (Z_TYPE(intern->obj) != IS_UNDEF && instanceof_function(ce, zend_ce_closure)) {
		zend_function *closure = zend_get_closure_invoke_method(Z_OBJ(intern->obj));
		if (closure) {
			_addmethod(closure, ce, return_value, filter, &intern->obj);
			_free_function(closure);
		}
	}
}
/* }}} */

/* {{{ proto public bool ReflectionClass::hasProperty(string name)
   Returns whether a property exists or not */
ZEND_METHOD(reflection_class, hasProperty)
{
	reflection_object *intern;
	zend_property_info *property_info;
	zend_class_entry *ce;
	zend_string *name;
	zval property;

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if ((property_info = zend_hash_find_ptr(&ce->properties_info, name)) != NULL) {
		if (property_info->flags & ZEND_ACC_SHADOW) {
			RETURN_FALSE;
		}
		RETURN_TRUE;
	} else {
		if (Z_TYPE(intern->obj) != IS_UNDEF && Z_OBJ_HANDLER(intern->obj, has_property)) {
			ZVAL_STR_COPY(&property, name);
			if (Z_OBJ_HANDLER(intern->obj, has_property)(&intern->obj, &property, 2, NULL)) {
				zval_ptr_dtor(&property);
				RETURN_TRUE;
			}
			zval_ptr_dtor(&property);
		}
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public ReflectionProperty ReflectionClass::getProperty(string name) throws ReflectionException
   Returns the class' property specified by its name */
ZEND_METHOD(reflection_class, getProperty)
{
	reflection_object *intern;
	zend_class_entry *ce, *ce2;
	zend_property_info *property_info;
	zend_string *name, *classname;
	char *tmp, *str_name;
	size_t classname_len, str_name_len;

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if ((property_info = zend_hash_find_ptr(&ce->properties_info, name)) != NULL) {
		if ((property_info->flags & ZEND_ACC_SHADOW) == 0) {
			reflection_property_factory(ce, property_info, return_value);
			return;
		}
	} else if (Z_TYPE(intern->obj) != IS_UNDEF) {
		/* Check for dynamic properties */
		if (zend_hash_exists(Z_OBJ_HT(intern->obj)->get_properties(&intern->obj), name)) {
			zend_property_info property_info_tmp;
			property_info_tmp.flags = ZEND_ACC_IMPLICIT_PUBLIC;
			property_info_tmp.name = zend_string_copy(name);
			property_info_tmp.doc_comment = NULL;
			property_info_tmp.ce = ce;

			reflection_property_factory(ce, &property_info_tmp, return_value);
			intern = Z_REFLECTION_P(return_value);
			intern->ref_type = REF_TYPE_DYNAMIC_PROPERTY;
			return;
		}
	}
	str_name = ZSTR_VAL(name);
	str_name_len = ZSTR_LEN(name);
	if ((tmp = strstr(ZSTR_VAL(name), "::")) != NULL) {
		classname_len = tmp - ZSTR_VAL(name);
		classname = zend_string_alloc(classname_len, 0);
		zend_str_tolower_copy(ZSTR_VAL(classname), ZSTR_VAL(name), classname_len);
		ZSTR_VAL(classname)[classname_len] = '\0';
		str_name_len = ZSTR_LEN(name) - (classname_len + 2);
		str_name = tmp + 2;

		ce2 = zend_lookup_class(classname);
		if (!ce2) {
			if (!EG(exception)) {
				zend_throw_exception_ex(reflection_exception_ptr, -1, "Class %s does not exist", ZSTR_VAL(classname));
			}
			zend_string_release(classname);
			return;
		}
		zend_string_release(classname);

		if (!instanceof_function(ce, ce2)) {
			zend_throw_exception_ex(reflection_exception_ptr, -1, "Fully qualified property name %s::%s does not specify a base class of %s", ZSTR_VAL(ce2->name), str_name, ZSTR_VAL(ce->name));
			return;
		}
		ce = ce2;

		if ((property_info = zend_hash_str_find_ptr(&ce->properties_info, str_name, str_name_len)) != NULL && (property_info->flags & ZEND_ACC_SHADOW) == 0) {
			reflection_property_factory(ce, property_info, return_value);
			return;
		}
	}
	zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Property %s does not exist", str_name);
}
/* }}} */

/* {{{ _addproperty */
static int _addproperty(zval *el, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval property;
	zend_property_info *pptr = (zend_property_info*)Z_PTR_P(el);
	zend_class_entry *ce = *va_arg(args, zend_class_entry**);
	zval *retval = va_arg(args, zval*);
	long filter = va_arg(args, long);

	if (pptr->flags	& ZEND_ACC_SHADOW) {
		return 0;
	}

	if (pptr->flags	& filter) {
		reflection_property_factory(ce, pptr, &property);
		add_next_index_zval(retval, &property);
	}
	return 0;
}
/* }}} */

/* {{{ _adddynproperty */
static int _adddynproperty(zval *ptr, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval property;
	zend_class_entry *ce = *va_arg(args, zend_class_entry**);
	zval *retval = va_arg(args, zval*);

	/* under some circumstances, the properties hash table may contain numeric
	 * properties (e.g. when casting from array). This is a WONT FIX bug, at
	 * least for the moment. Ignore these */
	if (hash_key->key == NULL) {
		return 0;
	}

	if (ZSTR_VAL(hash_key->key)[0] == '\0') {
		return 0; /* non public cannot be dynamic */
	}

	if (zend_get_property_info(ce, hash_key->key, 1) == NULL) {
		zend_property_info property_info;

		property_info.doc_comment = NULL;
		property_info.flags = ZEND_ACC_IMPLICIT_PUBLIC;
		property_info.name = hash_key->key;
		property_info.ce = ce;
		property_info.offset = -1;
		reflection_property_factory(ce, &property_info, &property);
		add_next_index_zval(retval, &property);
	}
	return 0;
}
/* }}} */

/* {{{ proto public ReflectionProperty[] ReflectionClass::getProperties([long $filter])
   Returns an array of this class' properties */
ZEND_METHOD(reflection_class, getProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_long filter = 0;
	int argc = ZEND_NUM_ARGS();

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (argc) {
		if (zend_parse_parameters(argc, "|l", &filter) == FAILURE) {
			return;
		}
	} else {
		/* No parameters given, default to "return all" */
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	zend_hash_apply_with_arguments(&ce->properties_info, (apply_func_args_t) _addproperty, 3, &ce, return_value, filter);

	if (Z_TYPE(intern->obj) != IS_UNDEF && (filter & ZEND_ACC_PUBLIC) != 0 && Z_OBJ_HT(intern->obj)->get_properties) {
		HashTable *properties = Z_OBJ_HT(intern->obj)->get_properties(&intern->obj);
		zend_hash_apply_with_arguments(properties, (apply_func_args_t) _adddynproperty, 2, &ce, return_value);
	}
}
/* }}} */

/* {{{ proto public bool ReflectionClass::hasConstant(string name)
   Returns whether a constant exists or not */
ZEND_METHOD(reflection_class, hasConstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *name;

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if (zend_hash_exists(&ce->constants_table, name)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public array ReflectionClass::getConstants()
   Returns an associative array containing this class' constants and their values */
ZEND_METHOD(reflection_class, getConstants)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *key;
	zend_class_constant *c;
	zval *val;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, key, c) {
		if (UNEXPECTED(zval_update_constant_ex(&c->value, ce) != SUCCESS)) {
			zend_array_destroy(Z_ARRVAL_P(return_value));
			return;
		}
		val = zend_hash_add_new(Z_ARRVAL_P(return_value), key, &c->value);
		Z_TRY_ADDREF_P(val);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto public array ReflectionClass::getReflectionConstants()
   Returns an associative array containing this class' constants as ReflectionClassConstant objects */
ZEND_METHOD(reflection_class, getReflectionConstants)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *name;
	zend_class_constant *constant;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, name, constant) {
		zval class_const;
		reflection_class_constant_factory(ce, name, constant, &class_const);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &class_const);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto public mixed ReflectionClass::getConstant(string name)
   Returns the class' constant specified by its name */
ZEND_METHOD(reflection_class, getConstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_class_constant *c;
	zend_string *name;

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	ZEND_HASH_FOREACH_PTR(&ce->constants_table, c) {
		if (UNEXPECTED(zval_update_constant_ex(&c->value, ce) != SUCCESS)) {
			return;
		}
	} ZEND_HASH_FOREACH_END();
	if ((c = zend_hash_find_ptr(&ce->constants_table, name)) == NULL) {
		RETURN_FALSE;
	}
	ZVAL_DUP(return_value, &c->value);
}
/* }}} */

/* {{{ proto public mixed ReflectionClass::getReflectionConstant(string name)
   Returns the class' constant as ReflectionClassConstant objects */
ZEND_METHOD(reflection_class, getReflectionConstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_class_constant *constant;
	zend_string *name;

	GET_REFLECTION_OBJECT_PTR(ce);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	if ((constant = zend_hash_find_ptr(&ce->constants_table, name)) == NULL) {
		RETURN_FALSE;
	}
	reflection_class_constant_factory(ce, name, constant, return_value);
}
/* }}} */

/* {{{ _class_check_flag */
static void _class_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETVAL_BOOL(ce->ce_flags & mask);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInstantiable()
   Returns whether this class is instantiable */
ZEND_METHOD(reflection_class, isInstantiable)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS | ZEND_ACC_IMPLICIT_ABSTRACT_CLASS)) {
		RETURN_FALSE;
	}

	/* Basically, the class is instantiable. Though, if there is a constructor
	 * and it is not publicly accessible, it isn't! */
	if (!ce->constructor) {
		RETURN_TRUE;
	}

	RETURN_BOOL(ce->constructor->common.fn_flags & ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isCloneable()
   Returns whether this class is cloneable */
ZEND_METHOD(reflection_class, isCloneable)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval obj;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_TRAIT | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS | ZEND_ACC_IMPLICIT_ABSTRACT_CLASS)) {
		RETURN_FALSE;
	}
	if (!Z_ISUNDEF(intern->obj)) {
		if (ce->clone) {
			RETURN_BOOL(ce->clone->common.fn_flags & ZEND_ACC_PUBLIC);
		} else {
			RETURN_BOOL(Z_OBJ_HANDLER(intern->obj, clone_obj) != NULL);
		}
	} else {
		if (ce->clone) {
			RETURN_BOOL(ce->clone->common.fn_flags & ZEND_ACC_PUBLIC);
		} else {
			if (UNEXPECTED(object_init_ex(&obj, ce) != SUCCESS)) {
				return;
			}
			RETVAL_BOOL(Z_OBJ_HANDLER(obj, clone_obj) != NULL);
			zval_dtor(&obj);
		}
	}
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInterface()
   Returns whether this is an interface or a class */
ZEND_METHOD(reflection_class, isInterface)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_INTERFACE);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isTrait()
   Returns whether this is a trait */
ZEND_METHOD(reflection_class, isTrait)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_TRAIT);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isFinal()
   Returns whether this class is final */
ZEND_METHOD(reflection_class, isFinal)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL);
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isAbstract()
   Returns whether this class is abstract */
ZEND_METHOD(reflection_class, isAbstract)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
}
/* }}} */

/* {{{ proto public int ReflectionClass::getModifiers()
   Returns a bitfield of the access modifiers for this class */
ZEND_METHOD(reflection_class, getModifiers)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_LONG(ce->ce_flags & ~(ZEND_ACC_CONSTANTS_UPDATED|ZEND_ACC_USE_GUARDS));
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInstance(stdclass object)
   Returns whether the given object is an instance of this class */
ZEND_METHOD(reflection_class, isInstance)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval *object;

	METHOD_NOTSTATIC(reflection_class_ptr);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &object) == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(instanceof_function(Z_OBJCE_P(object), ce));
}
/* }}} */

/* {{{ proto public stdclass ReflectionClass::newInstance(mixed* args, ...)
   Returns an instance of this class */
ZEND_METHOD(reflection_class, newInstance)
{
	zval retval;
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	zend_function *constructor;

	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(object_init_ex(return_value, ce) != SUCCESS)) {
		return;
	}

	old_scope = EG(fake_scope);
	EG(fake_scope) = ce;
	constructor = Z_OBJ_HT_P(return_value)->get_constructor(Z_OBJ_P(return_value));
	EG(fake_scope) = old_scope;

	/* Run the constructor if there is one */
	if (constructor) {
		zval *params = NULL;
		int ret, i, num_args = 0;
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;

		if (!(constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Access to non-public constructor of class %s", ZSTR_VAL(ce->name));
			zval_dtor(return_value);
			RETURN_NULL();
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &params, &num_args) == FAILURE) {
			zval_dtor(return_value);
			RETURN_FALSE;
		}

		for (i = 0; i < num_args; i++) {
			if (Z_REFCOUNTED(params[i])) Z_ADDREF(params[i]);
		}

		fci.size = sizeof(fci);
		ZVAL_UNDEF(&fci.function_name);
		fci.object = Z_OBJ_P(return_value);
		fci.retval = &retval;
		fci.param_count = num_args;
		fci.params = params;
		fci.no_separation = 1;

		fcc.initialized = 1;
		fcc.function_handler = constructor;
		fcc.calling_scope = zend_get_executed_scope();;
		fcc.called_scope = Z_OBJCE_P(return_value);
		fcc.object = Z_OBJ_P(return_value);

		ret = zend_call_function(&fci, &fcc);
		zval_ptr_dtor(&retval);
		for (i = 0; i < num_args; i++) {
			zval_ptr_dtor(&params[i]);
		}
		if (ret == FAILURE) {
			php_error_docref(NULL, E_WARNING, "Invocation of %s's constructor failed", ZSTR_VAL(ce->name));
			zval_dtor(return_value);
			RETURN_NULL();
		}
	} else if (ZEND_NUM_ARGS()) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s does not have a constructor, so you cannot pass any constructor arguments", ZSTR_VAL(ce->name));
	}
}
/* }}} */

/* {{{ proto public stdclass ReflectionClass::newInstanceWithoutConstructor()
   Returns an instance of this class without invoking its constructor */
ZEND_METHOD(reflection_class, newInstanceWithoutConstructor)
{
	reflection_object *intern;
	zend_class_entry *ce;

	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->create_object != NULL && ce->ce_flags & ZEND_ACC_FINAL) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s is an internal class marked as final that cannot be instantiated without invoking its constructor", ZSTR_VAL(ce->name));
		return;
	}

	object_init_ex(return_value, ce);
}
/* }}} */

/* {{{ proto public stdclass ReflectionClass::newInstanceArgs([array args])
   Returns an instance of this class */
ZEND_METHOD(reflection_class, newInstanceArgs)
{
	zval retval, *val;
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	int ret, i, argc = 0;
	HashTable *args;
	zend_function *constructor;


	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|h", &args) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS() > 0) {
		argc = args->nNumOfElements;
	}

	if (UNEXPECTED(object_init_ex(return_value, ce) != SUCCESS)) {
		return;
	}

	old_scope = EG(fake_scope);
	EG(fake_scope) = ce;
	constructor = Z_OBJ_HT_P(return_value)->get_constructor(Z_OBJ_P(return_value));
	EG(fake_scope) = old_scope;

	/* Run the constructor if there is one */
	if (constructor) {
		zval *params = NULL;
		zend_fcall_info fci;
		zend_fcall_info_cache fcc;

		if (!(constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Access to non-public constructor of class %s", ZSTR_VAL(ce->name));
			zval_dtor(return_value);
			RETURN_NULL();
		}

		if (argc) {
			params = safe_emalloc(sizeof(zval), argc, 0);
			argc = 0;
			ZEND_HASH_FOREACH_VAL(args, val) {
				ZVAL_COPY(&params[argc], val);
				argc++;
			} ZEND_HASH_FOREACH_END();
		}

		fci.size = sizeof(fci);
		ZVAL_UNDEF(&fci.function_name);
		fci.object = Z_OBJ_P(return_value);
		fci.retval = &retval;
		fci.param_count = argc;
		fci.params = params;
		fci.no_separation = 1;

		fcc.initialized = 1;
		fcc.function_handler = constructor;
		fcc.calling_scope = zend_get_executed_scope();
		fcc.called_scope = Z_OBJCE_P(return_value);
		fcc.object = Z_OBJ_P(return_value);

		ret = zend_call_function(&fci, &fcc);
		zval_ptr_dtor(&retval);
		if (params) {
			for (i = 0; i < argc; i++) {
				zval_ptr_dtor(&params[i]);
			}
			efree(params);
		}
		if (ret == FAILURE) {
			zval_ptr_dtor(&retval);
			php_error_docref(NULL, E_WARNING, "Invocation of %s's constructor failed", ZSTR_VAL(ce->name));
			zval_dtor(return_value);
			RETURN_NULL();
		}
	} else if (argc) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s does not have a constructor, so you cannot pass any constructor arguments", ZSTR_VAL(ce->name));
	}
}
/* }}} */

/* {{{ proto public ReflectionClass[] ReflectionClass::getInterfaces()
   Returns an array of interfaces this class implements */
ZEND_METHOD(reflection_class, getInterfaces)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	/* Return an empty array if this class implements no interfaces */
	array_init(return_value);

	if (ce->num_interfaces) {
		uint32_t i;

		for (i=0; i < ce->num_interfaces; i++) {
			zval interface;
			zend_reflection_class_factory(ce->interfaces[i], &interface);
			zend_hash_update(Z_ARRVAL_P(return_value), ce->interfaces[i]->name, &interface);
		}
	}
}
/* }}} */

/* {{{ proto public String[] ReflectionClass::getInterfaceNames()
   Returns an array of names of interfaces this class implements */
ZEND_METHOD(reflection_class, getInterfaceNames)
{
	reflection_object *intern;
	zend_class_entry *ce;
	uint32_t i;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	/* Return an empty array if this class implements no interfaces */
	array_init(return_value);

	for (i=0; i < ce->num_interfaces; i++) {
		add_next_index_str(return_value, zend_string_copy(ce->interfaces[i]->name));
	}
}
/* }}} */

/* {{{ proto public ReflectionClass[] ReflectionClass::getTraits()
   Returns an array of traits used by this class */
ZEND_METHOD(reflection_class, getTraits)
{
	reflection_object *intern;
	zend_class_entry *ce;
	uint32_t i;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);

	for (i=0; i < ce->num_traits; i++) {
		zval trait;
		zend_reflection_class_factory(ce->traits[i], &trait);
		zend_hash_update(Z_ARRVAL_P(return_value), ce->traits[i]->name, &trait);
	}
}
/* }}} */

/* {{{ proto public String[] ReflectionClass::getTraitNames()
   Returns an array of names of traits used by this class */
ZEND_METHOD(reflection_class, getTraitNames)
{
	reflection_object *intern;
	zend_class_entry *ce;
	uint32_t i;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);

	for (i=0; i < ce->num_traits; i++) {
		add_next_index_str(return_value, zend_string_copy(ce->traits[i]->name));
	}
}
/* }}} */

/* {{{ proto public arra ReflectionClass::getTraitaliases()
   Returns an array of trait aliases */
ZEND_METHOD(reflection_class, getTraitAliases)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);

	if (ce->trait_aliases) {
		uint32_t i = 0;
		while (ce->trait_aliases[i]) {
			zend_string *mname;
			zend_trait_method_reference *cur_ref = ce->trait_aliases[i]->trait_method;

			if (ce->trait_aliases[i]->alias) {

				mname = zend_string_alloc(ZSTR_LEN(cur_ref->ce->name) + ZSTR_LEN(cur_ref->method_name) + 2, 0);
				snprintf(ZSTR_VAL(mname), ZSTR_LEN(mname) + 1, "%s::%s", ZSTR_VAL(cur_ref->ce->name), ZSTR_VAL(cur_ref->method_name));
				add_assoc_str_ex(return_value, ZSTR_VAL(ce->trait_aliases[i]->alias), ZSTR_LEN(ce->trait_aliases[i]->alias), mname);
			}
			i++;
		}
	}
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionClass::getParentClass()
   Returns the class' parent class, or, if none exists, FALSE */
ZEND_METHOD(reflection_class, getParentClass)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->parent) {
		zend_reflection_class_factory(ce->parent, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isSubclassOf(string|ReflectionClass class)
   Returns whether this class is a subclass of another class */
ZEND_METHOD(reflection_class, isSubclassOf)
{
	reflection_object *intern, *argument;
	zend_class_entry *ce, *class_ce;
	zval *class_name;

	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &class_name) == FAILURE) {
		return;
	}

	switch (Z_TYPE_P(class_name)) {
		case IS_STRING:
			if ((class_ce = zend_lookup_class(Z_STR_P(class_name))) == NULL) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Class %s does not exist", Z_STRVAL_P(class_name));
				return;
			}
			break;
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(class_name), reflection_class_ptr)) {
				argument = Z_REFLECTION_P(class_name);
				if (argument->ptr == NULL) {
					php_error_docref(NULL, E_ERROR, "Internal error: Failed to retrieve the argument's reflection object");
					/* Bails out */
				}
				class_ce = argument->ptr;
				break;
			}
			/* no break */
		default:
			zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter one must either be a string or a ReflectionClass object");
			return;
	}

	RETURN_BOOL((ce != class_ce && instanceof_function(ce, class_ce)));
}
/* }}} */

/* {{{ proto public bool ReflectionClass::implementsInterface(string|ReflectionClass interface_name)
   Returns whether this class is a subclass of another class */
ZEND_METHOD(reflection_class, implementsInterface)
{
	reflection_object *intern, *argument;
	zend_class_entry *ce, *interface_ce;
	zval *interface;

	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &interface) == FAILURE) {
		return;
	}

	switch (Z_TYPE_P(interface)) {
		case IS_STRING:
			if ((interface_ce = zend_lookup_class(Z_STR_P(interface))) == NULL) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Interface %s does not exist", Z_STRVAL_P(interface));
				return;
			}
			break;
		case IS_OBJECT:
			if (instanceof_function(Z_OBJCE_P(interface), reflection_class_ptr)) {
				argument = Z_REFLECTION_P(interface);
				if (argument->ptr == NULL) {
					php_error_docref(NULL, E_ERROR, "Internal error: Failed to retrieve the argument's reflection object");
					/* Bails out */
				}
				interface_ce = argument->ptr;
				break;
			}
			/* no break */
		default:
			zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter one must either be a string or a ReflectionClass object");
			return;
	}

	if (!(interface_ce->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Interface %s is a Class", ZSTR_VAL(interface_ce->name));
		return;
	}
	RETURN_BOOL(instanceof_function(ce, interface_ce));
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isIterateable()
   Returns whether this class is iterateable (can be used inside foreach) */
ZEND_METHOD(reflection_class, isIterateable)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_BOOL(ce->get_iterator != NULL);
}
/* }}} */

/* {{{ proto public ReflectionExtension|NULL ReflectionClass::getExtension()
   Returns NULL or the extension the class belongs to */
ZEND_METHOD(reflection_class, getExtension)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	if ((ce->type == ZEND_INTERNAL_CLASS) && ce->info.internal.module) {
		reflection_extension_factory(return_value, ce->info.internal.module->name);
	}
}
/* }}} */

/* {{{ proto public string|false ReflectionClass::getExtensionName()
   Returns false or the name of the extension the class belongs to */
ZEND_METHOD(reflection_class, getExtensionName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	METHOD_NOTSTATIC(reflection_class_ptr);
	GET_REFLECTION_OBJECT_PTR(ce);

	if ((ce->type == ZEND_INTERNAL_CLASS) && ce->info.internal.module) {
		RETURN_STRING(ce->info.internal.module->name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto public bool ReflectionClass::inNamespace()
   Returns whether this class is defined in namespace */
ZEND_METHOD(reflection_class, inNamespace)
{
	zval *name;
	const char *backslash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if ((name = _default_load_entry(getThis(), "name", sizeof("name")-1)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public string ReflectionClass::getNamespaceName()
   Returns the name of namespace where this class is defined */
ZEND_METHOD(reflection_class, getNamespaceName)
{
	zval *name;
	const char *backslash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if ((name = _default_load_entry(getThis(), "name", sizeof("name")-1)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_STRINGL(Z_STRVAL_P(name), backslash - Z_STRVAL_P(name));
	}
	RETURN_EMPTY_STRING();
}
/* }}} */

/* {{{ proto public string ReflectionClass::getShortName()
   Returns the short name of the class (without namespace part) */
ZEND_METHOD(reflection_class, getShortName)
{
	zval *name;
	const char *backslash;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	if ((name = _default_load_entry(getThis(), "name", sizeof("name")-1)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_STRINGL(backslash + 1, Z_STRLEN_P(name) - (backslash - Z_STRVAL_P(name) + 1));
	}
	ZVAL_DEREF(name);
	ZVAL_COPY(return_value, name);
}
/* }}} */

/* {{{ proto public static mixed ReflectionObject::export(mixed argument [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_object, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_object_ptr, 1);
}
/* }}} */

/* {{{ proto public void ReflectionObject::__construct(mixed argument) throws ReflectionException
   Constructor. Takes an instance as an argument */
ZEND_METHOD(reflection_object, __construct)
{
	reflection_class_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto public static mixed ReflectionProperty::export(mixed class, string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_property, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_property_ptr, 2);
}
/* }}} */

/* {{{ proto public static mixed ReflectionClassConstant::export(mixed class, string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_class_constant, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_class_constant_ptr, 2);
}
/* }}} */

/* {{{ proto public void ReflectionProperty::__construct(mixed class, string name)
   Constructor. Throws an Exception in case the given property does not exist */
ZEND_METHOD(reflection_property, __construct)
{
	zval propname, cname, *classname;
	char *name_str;
	size_t name_len;
	int dynam_prop = 0;
	zval *object;
	reflection_object *intern;
	zend_class_entry *ce;
	zend_property_info *property_info = NULL;
	property_reference *reference;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zs", &classname, &name_str, &name_len) == FAILURE) {
		return;
	}

	object = getThis();
	intern = Z_REFLECTION_P(object);

	/* Find the class entry */
	switch (Z_TYPE_P(classname)) {
		case IS_STRING:
			if ((ce = zend_lookup_class(Z_STR_P(classname))) == NULL) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Class %s does not exist", Z_STRVAL_P(classname));
				return;
			}
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_P(classname);
			break;

		default:
			_DO_THROW("The parameter class is expected to be either a string or an object");
			/* returns out of this function */
	}

	if ((property_info = zend_hash_str_find_ptr(&ce->properties_info, name_str, name_len)) == NULL || (property_info->flags & ZEND_ACC_SHADOW)) {
		/* Check for dynamic properties */
		if (property_info == NULL && Z_TYPE_P(classname) == IS_OBJECT && Z_OBJ_HT_P(classname)->get_properties) {
			if (zend_hash_str_exists(Z_OBJ_HT_P(classname)->get_properties(classname), name_str, name_len)) {
				dynam_prop = 1;
			}
		}
		if (dynam_prop == 0) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Property %s::$%s does not exist", ZSTR_VAL(ce->name), name_str);
			return;
		}
	}

	if (dynam_prop == 0 && (property_info->flags & ZEND_ACC_PRIVATE) == 0) {
		/* we have to search the class hierarchy for this (implicit) public or protected property */
		zend_class_entry *tmp_ce = ce;
		zend_property_info *tmp_info;

		while (tmp_ce && (tmp_info = zend_hash_str_find_ptr(&tmp_ce->properties_info, name_str, name_len)) == NULL) {
			ce = tmp_ce;
			property_info = tmp_info;
			tmp_ce = tmp_ce->parent;
		}
	}

	if (dynam_prop == 0) {
		const char *class_name, *prop_name;
		size_t prop_name_len;
		zend_unmangle_property_name_ex(property_info->name, &class_name, &prop_name, &prop_name_len);
		ZVAL_STR_COPY(&cname, property_info->ce->name);
		ZVAL_STRINGL(&propname, prop_name, prop_name_len);
	} else {
		ZVAL_STR_COPY(&cname, ce->name);
		ZVAL_STRINGL(&propname, name_str, name_len);
	}
	reflection_update_property(object, "class", &cname);
	reflection_update_property(object, "name", &propname);

	reference = (property_reference*) emalloc(sizeof(property_reference));
	if (dynam_prop) {
		reference->prop.flags = ZEND_ACC_IMPLICIT_PUBLIC;
		reference->prop.name = Z_STR(propname);
		reference->prop.doc_comment = NULL;
		reference->prop.ce = ce;
	} else {
		reference->prop = *property_info;
	}
	reference->ce = ce;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_PROPERTY;
	intern->ce = ce;
	intern->ignore_visibility = 0;
}
/* }}} */

/* {{{ proto public string ReflectionProperty::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_property, __toString)
{
	reflection_object *intern;
	property_reference *ref;
	string str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	string_init(&str);
	_property_string(&str, &ref->prop, NULL, "");
	RETURN_NEW_STR(str.buf);
}
/* }}} */

/* {{{ proto public string ReflectionProperty::getName()
   Returns the class' name */
ZEND_METHOD(reflection_property, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_entry(getThis(), "name", sizeof("name")-1, return_value);
}
/* }}} */

static void _property_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask) /* {{{ */
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(ref->prop.flags & mask);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isPublic()
   Returns whether this property is public */
ZEND_METHOD(reflection_property, isPublic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC | ZEND_ACC_IMPLICIT_PUBLIC);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isPrivate()
   Returns whether this property is private */
ZEND_METHOD(reflection_property, isPrivate)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isProtected()
   Returns whether this property is protected */
ZEND_METHOD(reflection_property, isProtected)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isStatic()
   Returns whether this property is static */
ZEND_METHOD(reflection_property, isStatic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::isDefault()
   Returns whether this property is default (declared at compilation time). */
ZEND_METHOD(reflection_property, isDefault)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ~ZEND_ACC_IMPLICIT_PUBLIC);
}
/* }}} */

/* {{{ proto public int ReflectionProperty::getModifiers()
   Returns a bitfield of the access modifiers for this property */
ZEND_METHOD(reflection_property, getModifiers)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	RETURN_LONG(ref->prop.flags);
}
/* }}} */

/* {{{ proto public mixed ReflectionProperty::getValue([stdclass object])
   Returns this property's value */
ZEND_METHOD(reflection_property, getValue)
{
	reflection_object *intern;
	property_reference *ref;
	zval *object, *name;
	zval *member_p = NULL;

	METHOD_NOTSTATIC(reflection_property_ptr);
	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(ref->prop.flags & (ZEND_ACC_PUBLIC | ZEND_ACC_IMPLICIT_PUBLIC)) && intern->ignore_visibility == 0) {
		name = _default_load_entry(getThis(), "name", sizeof("name")-1);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public member %s::%s", ZSTR_VAL(intern->ce->name), Z_STRVAL_P(name));
		return;
	}

	if ((ref->prop.flags & ZEND_ACC_STATIC)) {
		if (UNEXPECTED(zend_update_class_constants(intern->ce) != SUCCESS)) {
			return;
		}
		if (Z_TYPE(CE_STATIC_MEMBERS(intern->ce)[ref->prop.offset]) == IS_UNDEF) {
			php_error_docref(NULL, E_ERROR, "Internal error: Could not find the property %s::%s", ZSTR_VAL(intern->ce->name), ZSTR_VAL(ref->prop.name));
			/* Bails out */
		}
		member_p = &CE_STATIC_MEMBERS(intern->ce)[ref->prop.offset];
		ZVAL_DEREF(member_p);
		ZVAL_COPY(return_value, member_p);
	} else {
		const char *class_name, *prop_name;
		size_t prop_name_len;
		zval rv;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &object) == FAILURE) {
			return;
		}

		if (!instanceof_function(Z_OBJCE_P(object), ref->ce)) {
			_DO_THROW("Given object is not an instance of the class this property was declared in");
			/* Returns from this function */
		}

		zend_unmangle_property_name_ex(ref->prop.name, &class_name, &prop_name, &prop_name_len);
		member_p = zend_read_property(ref->ce, object, prop_name, prop_name_len, 0, &rv);
		if (member_p != &rv) {
			ZVAL_DEREF(member_p);
			ZVAL_COPY(return_value, member_p);
		} else {
			if (Z_ISREF_P(member_p)) {
				zend_unwrap_reference(member_p);
			}
			ZVAL_COPY_VALUE(return_value, member_p);
		}
	}
}
/* }}} */

/* {{{ proto public void ReflectionProperty::setValue([stdclass object,] mixed value)
   Sets this property's value */
ZEND_METHOD(reflection_property, setValue)
{
	reflection_object *intern;
	property_reference *ref;
	zval *variable_ptr;
	zval *object, *name;
	zval *value;
	zval *tmp;

	METHOD_NOTSTATIC(reflection_property_ptr);
	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(ref->prop.flags & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		name = _default_load_entry(getThis(), "name", sizeof("name")-1);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public member %s::%s", ZSTR_VAL(intern->ce->name), Z_STRVAL_P(name));
		return;
	}

	if ((ref->prop.flags & ZEND_ACC_STATIC)) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &tmp, &value) == FAILURE) {
				return;
			}
		}
		if (UNEXPECTED(zend_update_class_constants(intern->ce) != SUCCESS)) {
			return;
		}

		if (Z_TYPE(CE_STATIC_MEMBERS(intern->ce)[ref->prop.offset]) == IS_UNDEF) {
			php_error_docref(NULL, E_ERROR, "Internal error: Could not find the property %s::%s", ZSTR_VAL(intern->ce->name), ZSTR_VAL(ref->prop.name));
			/* Bails out */
		}
		variable_ptr = &CE_STATIC_MEMBERS(intern->ce)[ref->prop.offset];
		if (variable_ptr != value) {
			zval garbage;

			ZVAL_DEREF(variable_ptr);
			ZVAL_DEREF(value);

			ZVAL_COPY_VALUE(&garbage, variable_ptr);

			ZVAL_COPY(variable_ptr, value);

			zval_ptr_dtor(&garbage);
		}
	} else {
		const char *class_name, *prop_name;
		size_t prop_name_len;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "oz", &object, &value) == FAILURE) {
			return;
		}

		zend_unmangle_property_name_ex(ref->prop.name, &class_name, &prop_name, &prop_name_len);
		zend_update_property(ref->ce, object, prop_name, prop_name_len, value);
	}
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionProperty::getDeclaringClass()
   Get the declaring class */
ZEND_METHOD(reflection_property, getDeclaringClass)
{
	reflection_object *intern;
	property_reference *ref;
	zend_class_entry *tmp_ce, *ce;
	zend_property_info *tmp_info;
	const char *prop_name, *class_name;
	size_t prop_name_len;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	if (zend_unmangle_property_name_ex(ref->prop.name, &class_name, &prop_name, &prop_name_len) != SUCCESS) {
		RETURN_FALSE;
	}

	ce = tmp_ce = ref->ce;
	while (tmp_ce && (tmp_info = zend_hash_str_find_ptr(&tmp_ce->properties_info, prop_name, prop_name_len)) != NULL) {
		if (tmp_info->flags & ZEND_ACC_PRIVATE || tmp_info->flags & ZEND_ACC_SHADOW) {
			/* it's a private property, so it can't be inherited */
			break;
		}
		ce = tmp_ce;
		if (tmp_ce == tmp_info->ce) {
			/* declared in this class, done */
			break;
		}
		tmp_ce = tmp_ce->parent;
	}

	zend_reflection_class_factory(ce, return_value);
}
/* }}} */

/* {{{ proto public string ReflectionProperty::getDocComment()
   Returns the doc comment for this property */
ZEND_METHOD(reflection_property, getDocComment)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	if (ref->prop.doc_comment) {
		RETURN_STR_COPY(ref->prop.doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto public int ReflectionProperty::setAccessible(bool visible)
   Sets whether non-public properties can be requested */
ZEND_METHOD(reflection_property, setAccessible)
{
	reflection_object *intern;
	zend_bool visible;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &visible) == FAILURE) {
		return;
	}

	intern = Z_REFLECTION_P(getThis());

	intern->ignore_visibility = visible;
}
/* }}} */

/* {{{ proto public static mixed ReflectionExtension::export(string name [, bool return]) throws ReflectionException
   Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_extension, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_extension_ptr, 1);
}
/* }}} */

/* {{{ proto public void ReflectionExtension::__construct(string name)
   Constructor. Throws an Exception in case the given extension does not exist */
ZEND_METHOD(reflection_extension, __construct)
{
	zval name;
	zval *object;
	char *lcname;
	reflection_object *intern;
	zend_module_entry *module;
	char *name_str;
	size_t name_len;
	ALLOCA_FLAG(use_heap)

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &name_str, &name_len) == FAILURE) {
		return;
	}

	object = getThis();
	intern = Z_REFLECTION_P(object);
	lcname = do_alloca(name_len + 1, use_heap);
	zend_str_tolower_copy(lcname, name_str, name_len);
	if ((module = zend_hash_str_find_ptr(&module_registry, lcname, name_len)) == NULL) {
		free_alloca(lcname, use_heap);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Extension %s does not exist", name_str);
		return;
	}
	free_alloca(lcname, use_heap);
	ZVAL_STRING(&name, module->name);
	reflection_update_property(object, "name", &name);
	intern->ptr = module;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = NULL;
}
/* }}} */

/* {{{ proto public string ReflectionExtension::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_extension, __toString)
{
	reflection_object *intern;
	zend_module_entry *module;
	string str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);
	string_init(&str);
	_extension_string(&str, module, "");
	RETURN_NEW_STR(str.buf);
}
/* }}} */

/* {{{ proto public string ReflectionExtension::getName()
   Returns this extension's name */
ZEND_METHOD(reflection_extension, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_entry(getThis(), "name", sizeof("name")-1, return_value);
}
/* }}} */

/* {{{ proto public string ReflectionExtension::getVersion()
   Returns this extension's version */
ZEND_METHOD(reflection_extension, getVersion)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	/* An extension does not necessarily have a version number */
	if (module->version == NO_VERSION_YET) {
		RETURN_NULL();
	} else {
		RETURN_STRING(module->version);
	}
}
/* }}} */

/* {{{ proto public ReflectionFunction[] ReflectionExtension::getFunctions()
   Returns an array of this extension's functions */
ZEND_METHOD(reflection_extension, getFunctions)
{
	reflection_object *intern;
	zend_module_entry *module;
	zval function;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	ZEND_HASH_FOREACH_PTR(CG(function_table), fptr) {
		if (fptr->common.type==ZEND_INTERNAL_FUNCTION
			&& fptr->internal_function.module == module) {
			reflection_function_factory(fptr, NULL, &function);
			zend_hash_update(Z_ARRVAL_P(return_value), fptr->common.function_name, &function);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

static int _addconstant(zval *el, int num_args, va_list args, zend_hash_key *hash_key) /* {{{ */
{
	zval const_val;
	zend_constant *constant = (zend_constant*)Z_PTR_P(el);
	zval *retval = va_arg(args, zval*);
	int number = va_arg(args, int);

	if (number == constant->module_number) {
		ZVAL_DUP(&const_val, &constant->value);
		zend_hash_update(Z_ARRVAL_P(retval), constant->name, &const_val);
	}
	return 0;
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getConstants()
   Returns an associative array containing this extension's constants and their values */
ZEND_METHOD(reflection_extension, getConstants)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(zend_constants), (apply_func_args_t) _addconstant, 2, return_value, module->module_number);
}
/* }}} */

/* {{{ _addinientry */
static int _addinientry(zval *el, int num_args, va_list args, zend_hash_key *hash_key)
{
	zend_ini_entry *ini_entry = (zend_ini_entry*)Z_PTR_P(el);
	zval *retval = va_arg(args, zval*);
	int number = va_arg(args, int);

	if (number == ini_entry->module_number) {
		if (ini_entry->value) {
			zval zv;

			ZVAL_STR_COPY(&zv, ini_entry->value);
			zend_symtable_update(Z_ARRVAL_P(retval), ini_entry->name, &zv);
		} else {
			zend_symtable_update(Z_ARRVAL_P(retval), ini_entry->name, &EG(uninitialized_zval));
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getINIEntries()
   Returns an associative array containing this extension's INI entries and their values */
ZEND_METHOD(reflection_extension, getINIEntries)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(ini_directives), (apply_func_args_t) _addinientry, 2, return_value, module->module_number);
}
/* }}} */

/* {{{ add_extension_class */
static int add_extension_class(zval *zv, int num_args, va_list args, zend_hash_key *hash_key)
{
	zend_class_entry *ce = Z_PTR_P(zv);
	zval *class_array = va_arg(args, zval*), zclass;
	struct _zend_module_entry *module = va_arg(args, struct _zend_module_entry*);
	int add_reflection_class = va_arg(args, int);

	if ((ce->type == ZEND_INTERNAL_CLASS) && ce->info.internal.module && !strcasecmp(ce->info.internal.module->name, module->name)) {
		zend_string *name;

		if (zend_binary_strcasecmp(ZSTR_VAL(ce->name), ZSTR_LEN(ce->name), ZSTR_VAL(hash_key->key), ZSTR_LEN(hash_key->key))) {
			/* This is an class alias, use alias name */
			name = hash_key->key;
		} else {
			/* Use class name */
			name = ce->name;
		}
		if (add_reflection_class) {
			zend_reflection_class_factory(ce, &zclass);
			zend_hash_update(Z_ARRVAL_P(class_array), name, &zclass);
		} else {
			add_next_index_str(class_array, zend_string_copy(name));
		}
	}
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ proto public ReflectionClass[] ReflectionExtension::getClasses()
   Returns an array containing ReflectionClass objects for all classes of this extension */
ZEND_METHOD(reflection_extension, getClasses)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) add_extension_class, 3, return_value, module, 1);
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getClassNames()
   Returns an array containing all names of all classes of this extension */
ZEND_METHOD(reflection_extension, getClassNames)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	zend_hash_apply_with_arguments(EG(class_table), (apply_func_args_t) add_extension_class, 3, return_value, module, 0);
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getDependencies()
   Returns an array containing all names of all extensions this extension depends on */
ZEND_METHOD(reflection_extension, getDependencies)
{
	reflection_object *intern;
	zend_module_entry *module;
	const zend_module_dep *dep;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);

	dep = module->deps;

	if (!dep)
	{
		return;
	}

	while(dep->name) {
		zend_string *relation;
		char *rel_type;
		size_t len = 0;

		switch(dep->type) {
			case MODULE_DEP_REQUIRED:
				rel_type = "Required";
				len += sizeof("Required") - 1;
				break;
			case MODULE_DEP_CONFLICTS:
				rel_type = "Conflicts";
				len += sizeof("Conflicts") - 1;
				break;
			case MODULE_DEP_OPTIONAL:
				rel_type = "Optional";
				len += sizeof("Optional") - 1;
				break;
			default:
				rel_type = "Error"; /* shouldn't happen */
				len += sizeof("Error") - 1;
				break;
		}

		if (dep->rel) {
			len += strlen(dep->rel) + 1;
		}

		if (dep->version) {
			len += strlen(dep->version) + 1;
		}

		relation = zend_string_alloc(len, 0);
		snprintf(ZSTR_VAL(relation), ZSTR_LEN(relation) + 1, "%s%s%s%s%s",
						rel_type,
						dep->rel ? " " : "",
						dep->rel ? dep->rel : "",
						dep->version ? " " : "",
						dep->version ? dep->version : "");
		add_assoc_str(return_value, dep->name, relation);
		dep++;
	}
}
/* }}} */

/* {{{ proto public void ReflectionExtension::info()
       Prints phpinfo block for the extension */
ZEND_METHOD(reflection_extension, info)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	php_info_print_module(module);
}
/* }}} */

/* {{{ proto public bool ReflectionExtension::isPersistent()
       Returns whether this extension is persistent */
ZEND_METHOD(reflection_extension, isPersistent)
{
	reflection_object *intern;
    zend_module_entry *module;

    if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	RETURN_BOOL(module->type == MODULE_PERSISTENT);
}
/* }}} */

/* {{{ proto public bool ReflectionExtension::isTemporary()
       Returns whether this extension is temporary */
ZEND_METHOD(reflection_extension, isTemporary)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	RETURN_BOOL(module->type == MODULE_TEMPORARY);
}
/* }}} */

/* {{{ proto public static mixed ReflectionZendExtension::export(string name [, bool return]) throws ReflectionException
 *    Exports a reflection object. Returns the output if TRUE is specified for return, printing it otherwise. */
ZEND_METHOD(reflection_zend_extension, export)
{
	_reflection_export(INTERNAL_FUNCTION_PARAM_PASSTHRU, reflection_zend_extension_ptr, 1);
}
/* }}} */

/* {{{ proto public void ReflectionZendExtension::__construct(string name)
       Constructor. Throws an Exception in case the given Zend extension does not exist */
ZEND_METHOD(reflection_zend_extension, __construct)
{
	zval name;
	zval *object;
	reflection_object *intern;
	zend_extension *extension;
	char *name_str;
	size_t name_len;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &name_str, &name_len) == FAILURE) {
		return;
	}

	object = getThis();
	intern = Z_REFLECTION_P(object);

	extension = zend_get_extension(name_str);
	if (!extension) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Zend Extension %s does not exist", name_str);
		return;
	}
	ZVAL_STRING(&name, extension->name);
	reflection_update_property(object, "name", &name);
	intern->ptr = extension;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = NULL;
}
/* }}} */

/* {{{ proto public string ReflectionZendExtension::__toString()
       Returns a string representation */
ZEND_METHOD(reflection_zend_extension, __toString)
{
	reflection_object *intern;
	zend_extension *extension;
	string str;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(extension);
	string_init(&str);
	_zend_extension_string(&str, extension, "");
	RETURN_NEW_STR(str.buf);
}
/* }}} */

/* {{{ proto public string ReflectionZendExtension::getName()
       Returns the name of this Zend extension */
ZEND_METHOD(reflection_zend_extension, getName)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	RETURN_STRING(extension->name);
}
/* }}} */

/* {{{ proto public string ReflectionZendExtension::getVersion()
       Returns the version information of this Zend extension */
ZEND_METHOD(reflection_zend_extension, getVersion)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->version) {
		RETURN_STRING(extension->version);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto public void ReflectionZendExtension::getAuthor()
 * Returns the name of this Zend extension's author */
ZEND_METHOD(reflection_zend_extension, getAuthor)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->author) {
		RETURN_STRING(extension->author);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto public void ReflectionZendExtension::getURL()
       Returns this Zend extension's URL*/
ZEND_METHOD(reflection_zend_extension, getURL)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->URL) {
		RETURN_STRING(extension->URL);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto public void ReflectionZendExtension::getCopyright()
       Returns this Zend extension's copyright information */
ZEND_METHOD(reflection_zend_extension, getCopyright)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->copyright) {
		RETURN_STRING(extension->copyright);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ method tables */
static const zend_function_entry reflection_exception_functions[] = {
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO(arginfo_reflection__void, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO(arginfo_reflection_getModifierNames, 0)
	ZEND_ARG_INFO(0, modifiers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_export, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, reflector, Reflector, 0)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_functions[] = {
	ZEND_ME(reflection, getModifierNames, arginfo_reflection_getModifierNames, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(reflection, export, arginfo_reflection_export, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

static const zend_function_entry reflector_functions[] = {
	ZEND_FENTRY(export, NULL, NULL, ZEND_ACC_STATIC|ZEND_ACC_ABSTRACT|ZEND_ACC_PUBLIC)
	ZEND_ABSTRACT_ME(reflector, __toString, arginfo_reflection__void)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_function_export, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_function___construct, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_function_invoke, 0, 0, 0)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_function_invokeArgs, 0)
	ZEND_ARG_ARRAY_INFO(0, args, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_function_abstract_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_function, inNamespace, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, isClosure, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, isDeprecated, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, isInternal, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, isUserDefined, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, isGenerator, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, isVariadic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getClosureThis, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getClosureScopeClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getDocComment, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getEndLine, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getExtension, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getExtensionName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getFileName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getNamespaceName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getNumberOfParameters, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getNumberOfRequiredParameters, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getParameters, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getShortName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getStartLine, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getStaticVariables, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, returnsReference, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, hasReturnType, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, getReturnType, arginfo_reflection__void, 0)
	PHP_FE_END
};

static const zend_function_entry reflection_function_functions[] = {
	ZEND_ME(reflection_function, __construct, arginfo_reflection_function___construct, 0)
	ZEND_ME(reflection_function, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, export, arginfo_reflection_function_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_function, isDisabled, arginfo_reflection__void, 0)
	ZEND_ME(reflection_function, invoke, arginfo_reflection_function_invoke, 0)
	ZEND_ME(reflection_function, invokeArgs, arginfo_reflection_function_invokeArgs, 0)
	ZEND_ME(reflection_function, getClosure, arginfo_reflection__void, 0)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO(arginfo_reflection_generator___construct, 0)
	ZEND_ARG_INFO(0, generator)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_generator_trace, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_generator_functions[] = {
	ZEND_ME(reflection_generator, __construct, arginfo_reflection_generator___construct, 0)
	ZEND_ME(reflection_generator, getExecutingLine, arginfo_reflection__void, 0)
	ZEND_ME(reflection_generator, getExecutingFile, arginfo_reflection__void, 0)
	ZEND_ME(reflection_generator, getTrace, arginfo_reflection_generator_trace, 0)
	ZEND_ME(reflection_generator, getFunction, arginfo_reflection__void, 0)
	ZEND_ME(reflection_generator, getThis, arginfo_reflection__void, 0)
	ZEND_ME(reflection_generator, getExecutingGenerator, arginfo_reflection__void, 0)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_method_export, 0, 0, 2)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_method___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, class_or_method)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_method_invoke, 0)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_method_invokeArgs, 0)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_ARRAY_INFO(0, args, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_method_setAccessible, 0)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_method_getClosure, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_method_functions[] = {
	ZEND_ME(reflection_method, export, arginfo_reflection_method_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_method, __construct, arginfo_reflection_method___construct, 0)
	ZEND_ME(reflection_method, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isPublic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isPrivate, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isProtected, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isAbstract, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isFinal, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isStatic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isConstructor, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, isDestructor, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, getClosure, arginfo_reflection_method_getClosure, 0)
	ZEND_ME(reflection_method, getModifiers, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, invoke, arginfo_reflection_method_invoke, 0)
	ZEND_ME(reflection_method, invokeArgs, arginfo_reflection_method_invokeArgs, 0)
	ZEND_ME(reflection_method, getDeclaringClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, getPrototype, arginfo_reflection__void, 0)
	ZEND_ME(reflection_method, setAccessible, arginfo_reflection_method_setAccessible, 0)
	PHP_FE_END
};


ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_class_export, 0, 0, 1)
	ZEND_ARG_INFO(0, argument)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class___construct, 0)
	ZEND_ARG_INFO(0, argument)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_class_getStaticPropertyValue, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, default)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_setStaticPropertyValue, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_hasMethod, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_getMethod, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_class_getMethods, 0, 0, 0)
	ZEND_ARG_INFO(0, filter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_hasProperty, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_getProperty, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_class_getProperties, 0, 0, 0)
	ZEND_ARG_INFO(0, filter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_hasConstant, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_getConstant, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_isInstance, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_newInstance, 0)
	ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_newInstanceWithoutConstructor, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_class_newInstanceArgs, 0, 0, 0)
	ZEND_ARG_ARRAY_INFO(0, args, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_isSubclassOf, 0)
	ZEND_ARG_INFO(0, class)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_class_implementsInterface, 0)
	ZEND_ARG_INFO(0, interface)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_class_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_class, export, arginfo_reflection_class_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_class, __construct, arginfo_reflection_class___construct, 0)
	ZEND_ME(reflection_class, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isInternal, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isUserDefined, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isAnonymous, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isInstantiable, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isCloneable, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getFileName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getStartLine, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getEndLine, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getDocComment, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getConstructor, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, hasMethod, arginfo_reflection_class_hasMethod, 0)
	ZEND_ME(reflection_class, getMethod, arginfo_reflection_class_getMethod, 0)
	ZEND_ME(reflection_class, getMethods, arginfo_reflection_class_getMethods, 0)
	ZEND_ME(reflection_class, hasProperty, arginfo_reflection_class_hasProperty, 0)
	ZEND_ME(reflection_class, getProperty, arginfo_reflection_class_getProperty, 0)
	ZEND_ME(reflection_class, getProperties, arginfo_reflection_class_getProperties, 0)
	ZEND_ME(reflection_class, hasConstant, arginfo_reflection_class_hasConstant, 0)
	ZEND_ME(reflection_class, getConstants, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getReflectionConstants, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getConstant, arginfo_reflection_class_getConstant, 0)
	ZEND_ME(reflection_class, getReflectionConstant, arginfo_reflection_class_getConstant, 0)
	ZEND_ME(reflection_class, getInterfaces, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getInterfaceNames, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isInterface, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getTraits, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getTraitNames, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getTraitAliases, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isTrait, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isAbstract, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isFinal, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getModifiers, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isInstance, arginfo_reflection_class_isInstance, 0)
	ZEND_ME(reflection_class, newInstance, arginfo_reflection_class_newInstance, 0)
	ZEND_ME(reflection_class, newInstanceWithoutConstructor, arginfo_reflection_class_newInstanceWithoutConstructor, 0)
	ZEND_ME(reflection_class, newInstanceArgs, arginfo_reflection_class_newInstanceArgs, 0)
	ZEND_ME(reflection_class, getParentClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isSubclassOf, arginfo_reflection_class_isSubclassOf, 0)
	ZEND_ME(reflection_class, getStaticProperties, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getStaticPropertyValue, arginfo_reflection_class_getStaticPropertyValue, 0)
	ZEND_ME(reflection_class, setStaticPropertyValue, arginfo_reflection_class_setStaticPropertyValue, 0)
	ZEND_ME(reflection_class, getDefaultProperties, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, isIterateable, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, implementsInterface, arginfo_reflection_class_implementsInterface, 0)
	ZEND_ME(reflection_class, getExtension, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getExtensionName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, inNamespace, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getNamespaceName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class, getShortName, arginfo_reflection__void, 0)
	PHP_FE_END
};


ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_object_export, 0, 0, 1)
	ZEND_ARG_INFO(0, argument)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_object___construct, 0)
	ZEND_ARG_INFO(0, argument)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_object_functions[] = {
	ZEND_ME(reflection_object, export, arginfo_reflection_object_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_object, __construct, arginfo_reflection_object___construct, 0)
	PHP_FE_END
};


ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_property_export, 0, 0, 2)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_property___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_property_getValue, 0, 0, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_property_setValue, 0, 0, 1)
	ZEND_ARG_INFO(0, object)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_property_setAccessible, 0)
	ZEND_ARG_INFO(0, visible)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_property_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_property, export, arginfo_reflection_property_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_property, __construct, arginfo_reflection_property___construct, 0)
	ZEND_ME(reflection_property, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getValue, arginfo_reflection_property_getValue, 0)
	ZEND_ME(reflection_property, setValue, arginfo_reflection_property_setValue, 0)
	ZEND_ME(reflection_property, isPublic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isPrivate, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isProtected, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isStatic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isDefault, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getModifiers, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getDeclaringClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getDocComment, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, setAccessible, arginfo_reflection_property_setAccessible, 0)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_class_constant_export, 0, 0, 2)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_class_constant___construct, 0, 0, 2)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_class_constant_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_class_constant, export, arginfo_reflection_class_constant_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_class_constant, __construct, arginfo_reflection_class_constant___construct, 0)
	ZEND_ME(reflection_class_constant, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, getValue, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, isPublic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, isPrivate, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, isProtected, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, getModifiers, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, getDeclaringClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_class_constant, getDocComment, arginfo_reflection__void, 0)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_parameter_export, 0, 0, 2)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_INFO(0, parameter)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_parameter___construct, 0)
	ZEND_ARG_INFO(0, function)
	ZEND_ARG_INFO(0, parameter)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_parameter_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_parameter, export, arginfo_reflection_parameter_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_parameter, __construct, arginfo_reflection_parameter___construct, 0)
	ZEND_ME(reflection_parameter, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, isPassedByReference, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, canBePassedByValue, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getDeclaringFunction, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getDeclaringClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, hasType, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getType, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, isArray, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, isCallable, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, allowsNull, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getPosition, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, isOptional, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, isDefaultValueAvailable, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getDefaultValue, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, isDefaultValueConstant, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, getDefaultValueConstantName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_parameter, isVariadic, arginfo_reflection__void, 0)
	PHP_FE_END
};

static const zend_function_entry reflection_type_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_type, allowsNull, arginfo_reflection__void, 0)
	ZEND_ME(reflection_type, isBuiltin, arginfo_reflection__void, 0)
	ZEND_ME(reflection_type, __toString, arginfo_reflection__void, 0)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_extension_export, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, return)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_extension___construct, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_extension_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_extension, export, arginfo_reflection_extension_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_extension, __construct, arginfo_reflection_extension___construct, 0)
	ZEND_ME(reflection_extension, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getVersion, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getFunctions, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getConstants, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getINIEntries, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getClasses, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getClassNames, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, getDependencies, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, info, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, isPersistent, arginfo_reflection__void, 0)
	ZEND_ME(reflection_extension, isTemporary, arginfo_reflection__void, 0)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO(arginfo_reflection_zend_extension___construct, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_zend_extension_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_ME(reflection_zend_extension, export, arginfo_reflection_extension_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_zend_extension, __construct, arginfo_reflection_zend_extension___construct, 0)
	ZEND_ME(reflection_zend_extension, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getVersion, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getAuthor, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getURL, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getCopyright, arginfo_reflection__void, 0)
	PHP_FE_END
};
/* }}} */

const zend_function_entry reflection_ext_functions[] = { /* {{{ */
	PHP_FE_END
}; /* }}} */

static zend_object_handlers *zend_std_obj_handlers;

/* {{{ _reflection_write_property */
static void _reflection_write_property(zval *object, zval *member, zval *value, void **cache_slot)
{
	if ((Z_TYPE_P(member) == IS_STRING)
		&& zend_hash_exists(&Z_OBJCE_P(object)->properties_info, Z_STR_P(member))
		&& ((Z_STRLEN_P(member) == sizeof("name") - 1  && !memcmp(Z_STRVAL_P(member), "name",  sizeof("name")))
			|| (Z_STRLEN_P(member) == sizeof("class") - 1 && !memcmp(Z_STRVAL_P(member), "class", sizeof("class")))))
	{
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot set read-only property %s::$%s", ZSTR_VAL(Z_OBJCE_P(object)->name), Z_STRVAL_P(member));
	}
	else
	{
		zend_std_obj_handlers->write_property(object, member, value, cache_slot);
	}
}
/* }}} */

PHP_MINIT_FUNCTION(reflection) /* {{{ */
{
	zend_class_entry _reflection_entry;

	zend_std_obj_handlers = zend_get_std_object_handlers();
	memcpy(&reflection_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	reflection_object_handlers.offset = XtOffsetOf(reflection_object, zo);
	reflection_object_handlers.free_obj = reflection_free_objects_storage;
	reflection_object_handlers.clone_obj = NULL;
	reflection_object_handlers.write_property = _reflection_write_property;

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionException", reflection_exception_functions);
	reflection_exception_ptr = zend_register_internal_class_ex(&_reflection_entry, zend_ce_exception);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflection", reflection_functions);
	reflection_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflector", reflector_functions);
	reflector_ptr = zend_register_internal_interface(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionFunctionAbstract", reflection_function_abstract_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_function_abstract_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_function_abstract_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_function_abstract_ptr, "name", sizeof("name")-1, "", ZEND_ACC_ABSTRACT);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionFunction", reflection_function_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_function_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_function_abstract_ptr);
	zend_declare_property_string(reflection_function_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(function, "IS_DEPRECATED", ZEND_ACC_DEPRECATED);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionGenerator", reflection_generator_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_generator_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionParameter", reflection_parameter_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_parameter_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_parameter_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_parameter_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionType", reflection_type_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_type_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionMethod", reflection_method_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_method_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_function_abstract_ptr);
	zend_declare_property_string(reflection_method_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(reflection_method_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(method, "IS_STATIC", ZEND_ACC_STATIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(method, "IS_PUBLIC", ZEND_ACC_PUBLIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(method, "IS_PROTECTED", ZEND_ACC_PROTECTED);
	REGISTER_REFLECTION_CLASS_CONST_LONG(method, "IS_PRIVATE", ZEND_ACC_PRIVATE);
	REGISTER_REFLECTION_CLASS_CONST_LONG(method, "IS_ABSTRACT", ZEND_ACC_ABSTRACT);
	REGISTER_REFLECTION_CLASS_CONST_LONG(method, "IS_FINAL", ZEND_ACC_FINAL);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionClass", reflection_class_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_class_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_class_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_class_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_IMPLICIT_ABSTRACT", ZEND_ACC_IMPLICIT_ABSTRACT_CLASS);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_EXPLICIT_ABSTRACT", ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_FINAL", ZEND_ACC_FINAL);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionObject", reflection_object_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_object_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_class_ptr);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionProperty", reflection_property_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_property_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_property_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_property_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(reflection_property_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionClassConstant", reflection_class_constant_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_class_constant_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_class_constant_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_class_constant_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(reflection_class_constant_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_STATIC", ZEND_ACC_STATIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PUBLIC", ZEND_ACC_PUBLIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PROTECTED", ZEND_ACC_PROTECTED);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PRIVATE", ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionExtension", reflection_extension_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_extension_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_extension_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_extension_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionZendExtension", reflection_zend_extension_functions);
	_reflection_entry.create_object = reflection_objects_new;
	reflection_zend_extension_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_zend_extension_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_zend_extension_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	return SUCCESS;
} /* }}} */

PHP_MINFO_FUNCTION(reflection) /* {{{ */
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Reflection", "enabled");

	php_info_print_table_row(2, "Version", "$Id$");

	php_info_print_table_end();
} /* }}} */

zend_module_entry reflection_module_entry = { /* {{{ */
	STANDARD_MODULE_HEADER,
	"Reflection",
	reflection_ext_functions,
	PHP_MINIT(reflection),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(reflection),
	PHP_REFLECTION_VERSION,
	STANDARD_MODULE_PROPERTIES
}; /* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 */
