/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_reflection.h"
#include "ext/standard/info.h"
#include "ext/standard/sha1.h"
#include "ext/standard/php_random.h"

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
#include "zend_smart_str.h"

/* Key used to avoid leaking addresses in ReflectionProperty::getId() */
#define REFLECTION_KEY_LEN 16
ZEND_BEGIN_MODULE_GLOBALS(reflection)
	zend_bool key_initialized;
	unsigned char key[REFLECTION_KEY_LEN];
ZEND_END_MODULE_GLOBALS(reflection)
ZEND_DECLARE_MODULE_GLOBALS(reflection)

#define REFLECTION_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(reflection, v)

static zend_always_inline zval *reflection_prop_name(zval *object) {
	/* $name is always in the first property slot. */
	ZEND_ASSERT(Z_OBJCE_P(object)->default_properties_count >= 1);
	return &Z_OBJ_P(object)->properties_table[0];
}

static zend_always_inline zval *reflection_prop_class(zval *object) {
	/* $class is always in the second property slot. */
	ZEND_ASSERT(Z_OBJCE_P(object)->default_properties_count >= 2);
	return &Z_OBJ_P(object)->properties_table[1];
}

/* Class entry pointers */
PHPAPI zend_class_entry *reflector_ptr;
PHPAPI zend_class_entry *reflection_exception_ptr;
PHPAPI zend_class_entry *reflection_ptr;
PHPAPI zend_class_entry *reflection_function_abstract_ptr;
PHPAPI zend_class_entry *reflection_function_ptr;
PHPAPI zend_class_entry *reflection_generator_ptr;
PHPAPI zend_class_entry *reflection_parameter_ptr;
PHPAPI zend_class_entry *reflection_type_ptr;
PHPAPI zend_class_entry *reflection_named_type_ptr;
PHPAPI zend_class_entry *reflection_class_ptr;
PHPAPI zend_class_entry *reflection_object_ptr;
PHPAPI zend_class_entry *reflection_method_ptr;
PHPAPI zend_class_entry *reflection_property_ptr;
PHPAPI zend_class_entry *reflection_class_constant_ptr;
PHPAPI zend_class_entry *reflection_extension_ptr;
PHPAPI zend_class_entry *reflection_zend_extension_ptr;
PHPAPI zend_class_entry *reflection_reference_ptr;

/* Exception throwing macro */
#define _DO_THROW(msg) \
	zend_throw_exception(reflection_exception_ptr, msg, 0);

#define GET_REFLECTION_OBJECT() do { \
	intern = Z_REFLECTION_P(ZEND_THIS); \
	if (intern->ptr == NULL) { \
		if (EG(exception) && EG(exception)->ce == reflection_exception_ptr) { \
			return; \
		} \
		zend_throw_error(NULL, "Internal error: Failed to retrieve the reflection object"); \
		return; \
	} \
} while (0)

#define GET_REFLECTION_OBJECT_PTR(target) do { \
	GET_REFLECTION_OBJECT(); \
	target = intern->ptr; \
} while (0)

/* Class constants */
#define REGISTER_REFLECTION_CLASS_CONST_LONG(class_name, const_name, value) \
	zend_declare_class_constant_long(reflection_ ## class_name ## _ptr, const_name, sizeof(const_name)-1, (zend_long)value);

/* {{{ Object structure */

/* Struct for properties */
typedef struct _property_reference {
	zend_property_info prop;
	zend_string *unmangled_name;
	zend_bool dynamic;
} property_reference;

/* Struct for parameters */
typedef struct _parameter_reference {
	uint32_t offset;
	zend_bool required;
	struct _zend_arg_info *arg_info;
	zend_function *fptr;
} parameter_reference;

/* Struct for type hints */
typedef struct _type_reference {
	zend_type type;
} type_reference;

typedef enum {
	REF_TYPE_OTHER,      /* Must be 0 */
	REF_TYPE_FUNCTION,
	REF_TYPE_GENERATOR,
	REF_TYPE_PARAMETER,
	REF_TYPE_TYPE,
	REF_TYPE_PROPERTY,
	REF_TYPE_CLASS_CONSTANT
} reflection_type_t;

/* Struct for reflection objects */
typedef struct {
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

static inline zend_bool is_closure_invoke(zend_class_entry *ce, zend_string *lcname) {
	return ce == zend_ce_closure
		&& zend_string_equals_literal(lcname, ZEND_INVOKE_FUNC_NAME);
}

static zval *_default_load_name(zval *object) /* {{{ */
{
	return zend_hash_find_ex_ind(Z_OBJPROP_P(object), ZSTR_KNOWN(ZEND_STR_NAME), 1);
}
/* }}} */

static void _default_get_name(zval *object, zval *return_value) /* {{{ */
{
	zval *value;

	if ((value = _default_load_name(object)) == NULL) {
		RETURN_FALSE;
	}
	ZVAL_COPY(return_value, value);
}
/* }}} */

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
		zend_string_release_ex(fptr->internal_function.function_name, 0);
		zend_free_trampoline(fptr);
	}
}
/* }}} */

static void reflection_free_objects_storage(zend_object *object) /* {{{ */
{
	reflection_object *intern = reflection_object_from_obj(object);
	parameter_reference *reference;
	property_reference *prop_reference;

	if (intern->ptr) {
		switch (intern->ref_type) {
		case REF_TYPE_PARAMETER:
			reference = (parameter_reference*)intern->ptr;
			_free_function(reference->fptr);
			efree(intern->ptr);
			break;
		case REF_TYPE_TYPE:
		{
			type_reference *type_ref = intern->ptr;
			if (ZEND_TYPE_IS_NAME(type_ref->type)) {
				zend_string_release(ZEND_TYPE_NAME(type_ref->type));
			}
			efree(type_ref);
			break;
		}
		case REF_TYPE_FUNCTION:
			_free_function(intern->ptr);
			break;
		case REF_TYPE_PROPERTY:
			prop_reference = (property_reference*)intern->ptr;
			zend_string_release_ex(prop_reference->unmangled_name, 0);

			if (ZEND_TYPE_IS_NAME(prop_reference->prop.type)) {
				zend_string_release(ZEND_TYPE_NAME(prop_reference->prop.type));
			}

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

static HashTable *reflection_get_gc(zval *obj, zval **gc_data, int *gc_data_count) /* {{{ */
{
	reflection_object *intern = Z_REFLECTION_P(obj);
	*gc_data = &intern->obj;
	*gc_data_count = 1;
	return zend_std_get_properties(obj);
}
/* }}} */

static zend_object *reflection_objects_new(zend_class_entry *class_type) /* {{{ */
{
	reflection_object *intern = zend_object_alloc(sizeof(reflection_object), class_type);

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

static void _const_string(smart_str *str, char *name, zval *value, char *indent);
static void _function_string(smart_str *str, zend_function *fptr, zend_class_entry *scope, char* indent);
static void _property_string(smart_str *str, zend_property_info *prop, const char *prop_name, char* indent, zend_bool dynamic);
static void _class_const_string(smart_str *str, char *name, zend_class_constant *c, char* indent);
static void _class_string(smart_str *str, zend_class_entry *ce, zval *obj, char *indent);
static void _extension_string(smart_str *str, zend_module_entry *module, char *indent);
static void _zend_extension_string(smart_str *str, zend_extension *extension, char *indent);

/* {{{ _class_string */
static void _class_string(smart_str *str, zend_class_entry *ce, zval *obj, char *indent)
{
	int count, count_static_props = 0, count_static_funcs = 0, count_shadow_props = 0;
	zend_string *sub_indent = strpprintf(0, "%s    ", indent);

	/* TBD: Repair indenting of doc comment (or is this to be done in the parser?) */
	if (ce->type == ZEND_USER_CLASS && ce->info.user.doc_comment) {
		smart_str_append_printf(str, "%s%s", indent, ZSTR_VAL(ce->info.user.doc_comment));
		smart_str_appendc(str, '\n');
	}

	if (obj && Z_TYPE_P(obj) == IS_OBJECT) {
		smart_str_append_printf(str, "%sObject of class [ ", indent);
	} else {
		char *kind = "Class";
		if (ce->ce_flags & ZEND_ACC_INTERFACE) {
			kind = "Interface";
		} else if (ce->ce_flags & ZEND_ACC_TRAIT) {
			kind = "Trait";
		}
		smart_str_append_printf(str, "%s%s [ ", indent, kind);
	}
	smart_str_append_printf(str, (ce->type == ZEND_USER_CLASS) ? "<user" : "<internal");
	if (ce->type == ZEND_INTERNAL_CLASS && ce->info.internal.module) {
		smart_str_append_printf(str, ":%s", ce->info.internal.module->name);
	}
	smart_str_append_printf(str, "> ");
	if (ce->get_iterator != NULL) {
		smart_str_append_printf(str, "<iterateable> ");
	}
	if (ce->ce_flags & ZEND_ACC_INTERFACE) {
		smart_str_append_printf(str, "interface ");
	} else if (ce->ce_flags & ZEND_ACC_TRAIT) {
		smart_str_append_printf(str, "trait ");
	} else {
		if (ce->ce_flags & (ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
			smart_str_append_printf(str, "abstract ");
		}
		if (ce->ce_flags & ZEND_ACC_FINAL) {
			smart_str_append_printf(str, "final ");
		}
		smart_str_append_printf(str, "class ");
	}
	smart_str_append_printf(str, "%s", ZSTR_VAL(ce->name));
	if (ce->parent) {
		smart_str_append_printf(str, " extends %s", ZSTR_VAL(ce->parent->name));
	}

	if (ce->num_interfaces) {
		uint32_t i;

		ZEND_ASSERT(ce->ce_flags & ZEND_ACC_LINKED);
		if (ce->ce_flags & ZEND_ACC_INTERFACE) {
			smart_str_append_printf(str, " extends %s", ZSTR_VAL(ce->interfaces[0]->name));
		} else {
			smart_str_append_printf(str, " implements %s", ZSTR_VAL(ce->interfaces[0]->name));
		}
		for (i = 1; i < ce->num_interfaces; ++i) {
			smart_str_append_printf(str, ", %s", ZSTR_VAL(ce->interfaces[i]->name));
		}
	}
	smart_str_append_printf(str, " ] {\n");

	/* The information where a class is declared is only available for user classes */
	if (ce->type == ZEND_USER_CLASS) {
		smart_str_append_printf(str, "%s  @@ %s %d-%d\n", indent, ZSTR_VAL(ce->info.user.filename),
						ce->info.user.line_start, ce->info.user.line_end);
	}

	/* Constants */
	smart_str_append_printf(str, "\n");
	count = zend_hash_num_elements(&ce->constants_table);
	smart_str_append_printf(str, "%s  - Constants [%d] {\n", indent, count);
	if (count > 0) {
		zend_string *key;
		zend_class_constant *c;

		ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, key, c) {
			_class_const_string(str, ZSTR_VAL(key), c, ZSTR_VAL(sub_indent));
			if (UNEXPECTED(EG(exception))) {
				zend_string_release(sub_indent);
				return;
			}
		} ZEND_HASH_FOREACH_END();
	}
	smart_str_append_printf(str, "%s  }\n", indent);

	/* Static properties */
	/* counting static properties */
	count = zend_hash_num_elements(&ce->properties_info);
	if (count > 0) {
		zend_property_info *prop;

		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			if ((prop->flags & ZEND_ACC_PRIVATE) && prop->ce != ce) {
				count_shadow_props++;
			} else if (prop->flags & ZEND_ACC_STATIC) {
				count_static_props++;
			}
		} ZEND_HASH_FOREACH_END();
	}

	/* static properties */
	smart_str_append_printf(str, "\n%s  - Static properties [%d] {\n", indent, count_static_props);
	if (count_static_props > 0) {
		zend_property_info *prop;

		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			if ((prop->flags & ZEND_ACC_STATIC) && (!(prop->flags & ZEND_ACC_PRIVATE) || prop->ce == ce)) {
				_property_string(str, prop, NULL, ZSTR_VAL(sub_indent), 0);
			}
		} ZEND_HASH_FOREACH_END();
	}
	smart_str_append_printf(str, "%s  }\n", indent);

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
	smart_str_append_printf(str, "\n%s  - Static methods [%d] {", indent, count_static_funcs);
	if (count_static_funcs > 0) {
		zend_function *mptr;

		ZEND_HASH_FOREACH_PTR(&ce->function_table, mptr) {
			if (mptr->common.fn_flags & ZEND_ACC_STATIC
				&& ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) == 0 || mptr->common.scope == ce))
			{
				smart_str_append_printf(str, "\n");
				_function_string(str, mptr, ce, ZSTR_VAL(sub_indent));
			}
		} ZEND_HASH_FOREACH_END();
	} else {
		smart_str_append_printf(str, "\n");
	}
	smart_str_append_printf(str, "%s  }\n", indent);

	/* Default/Implicit properties */
	count = zend_hash_num_elements(&ce->properties_info) - count_static_props - count_shadow_props;
	smart_str_append_printf(str, "\n%s  - Properties [%d] {\n", indent, count);
	if (count > 0) {
		zend_property_info *prop;

		ZEND_HASH_FOREACH_PTR(&ce->properties_info, prop) {
			if (!(prop->flags & ZEND_ACC_STATIC)
			 && (!(prop->flags & ZEND_ACC_PRIVATE) || prop->ce == ce)) {
				_property_string(str, prop, NULL, ZSTR_VAL(sub_indent), 0);
			}
		} ZEND_HASH_FOREACH_END();
	}
	smart_str_append_printf(str, "%s  }\n", indent);

	if (obj && Z_TYPE_P(obj) == IS_OBJECT) {
		HashTable    *properties = Z_OBJ_HT_P(obj)->get_properties(obj);
		zend_string  *prop_name;
		smart_str prop_str = {0};

		count = 0;
		if (properties && zend_hash_num_elements(properties)) {
			ZEND_HASH_FOREACH_STR_KEY(properties, prop_name) {
				if (prop_name && ZSTR_LEN(prop_name) && ZSTR_VAL(prop_name)[0]) { /* skip all private and protected properties */
					if (!zend_hash_exists(&ce->properties_info, prop_name)) {
						count++;
						_property_string(&prop_str, NULL, ZSTR_VAL(prop_name), ZSTR_VAL(sub_indent), 0);
					}
				}
			} ZEND_HASH_FOREACH_END();
		}

		smart_str_append_printf(str, "\n%s  - Dynamic properties [%d] {\n", indent, count);
		smart_str_append_smart_str(str, &prop_str);
		smart_str_append_printf(str, "%s  }\n", indent);
		smart_str_free(&prop_str);
	}

	/* Non static methods */
	count = zend_hash_num_elements(&ce->function_table) - count_static_funcs;
	if (count > 0) {
		zend_function *mptr;
		smart_str method_str = {0};

		count = 0;
		ZEND_HASH_FOREACH_PTR(&ce->function_table, mptr) {
			if ((mptr->common.fn_flags & ZEND_ACC_STATIC) == 0
				&& ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) == 0 || mptr->common.scope == ce))
			{
				zend_function *closure;
				/* see if this is a closure */
				if (obj && is_closure_invoke(ce, mptr->common.function_name)
					&& (closure = zend_get_closure_invoke_method(Z_OBJ_P(obj))) != NULL)
				{
					mptr = closure;
				} else {
					closure = NULL;
				}
				smart_str_appendc(&method_str, '\n');
				_function_string(&method_str, mptr, ce, ZSTR_VAL(sub_indent));
				count++;
				_free_function(closure);
			}
		} ZEND_HASH_FOREACH_END();
		smart_str_append_printf(str, "\n%s  - Methods [%d] {", indent, count);
		smart_str_append_smart_str(str, &method_str);
		if (!count) {
			smart_str_append_printf(str, "\n");
		}
		smart_str_free(&method_str);
	} else {
		smart_str_append_printf(str, "\n%s  - Methods [0] {\n", indent);
	}
	smart_str_append_printf(str, "%s  }\n", indent);

	smart_str_append_printf(str, "%s}\n", indent);
	zend_string_release_ex(sub_indent, 0);
}
/* }}} */

/* {{{ _const_string */
static void _const_string(smart_str *str, char *name, zval *value, char *indent)
{
	char *type = zend_zval_type_name(value);

	if (Z_TYPE_P(value) == IS_ARRAY) {
		smart_str_append_printf(str, "%s    Constant [ %s %s ] { Array }\n",
						indent, type, name);
	} else if (Z_TYPE_P(value) == IS_STRING) {
		smart_str_append_printf(str, "%s    Constant [ %s %s ] { %s }\n",
						indent, type, name, Z_STRVAL_P(value));
	} else {
		zend_string *tmp_value_str;
		zend_string *value_str = zval_get_tmp_string(value, &tmp_value_str);
		smart_str_append_printf(str, "%s    Constant [ %s %s ] { %s }\n",
						indent, type, name, ZSTR_VAL(value_str));
		zend_tmp_string_release(tmp_value_str);
	}
}
/* }}} */

/* {{{ _class_const_string */
static void _class_const_string(smart_str *str, char *name, zend_class_constant *c, char *indent)
{
	char *visibility = zend_visibility_string(Z_ACCESS_FLAGS(c->value));
	char *type;

	if (zval_update_constant_ex(&c->value, c->ce) == FAILURE) {
		return;
	}

	type = zend_zval_type_name(&c->value);

	if (Z_TYPE(c->value) == IS_ARRAY) {
		smart_str_append_printf(str, "%sConstant [ %s %s %s ] { Array }\n",
						indent, visibility, type, name);
	} else {
		zend_string *tmp_value_str;
		zend_string *value_str = zval_get_tmp_string(&c->value, &tmp_value_str);

		smart_str_append_printf(str, "%sConstant [ %s %s %s ] { %s }\n",
						indent, visibility, type, name, ZSTR_VAL(value_str));

		zend_tmp_string_release(tmp_value_str);
	}
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
		    || op->opcode == ZEND_RECV_VARIADIC) && op->op1.num == offset)
		{
			return op;
		}
		++op;
	}
	return NULL;
}
/* }}} */

/* {{{ _parameter_string */
static void _parameter_string(smart_str *str, zend_function *fptr, struct _zend_arg_info *arg_info, uint32_t offset, zend_bool required, char* indent)
{
	smart_str_append_printf(str, "Parameter #%d [ ", offset);
	if (!required) {
		smart_str_append_printf(str, "<optional> ");
	} else {
		smart_str_append_printf(str, "<required> ");
	}
	if (ZEND_TYPE_IS_CLASS(arg_info->type)) {
		smart_str_append_printf(str, "%s ",
			ZSTR_VAL(ZEND_TYPE_NAME(arg_info->type)));
		if (ZEND_TYPE_ALLOW_NULL(arg_info->type)) {
			smart_str_append_printf(str, "or NULL ");
		}
	} else if (ZEND_TYPE_IS_CODE(arg_info->type)) {
		smart_str_append_printf(str, "%s ", zend_get_type_by_const(ZEND_TYPE_CODE(arg_info->type)));
		if (ZEND_TYPE_ALLOW_NULL(arg_info->type)) {
			smart_str_append_printf(str, "or NULL ");
		}
	}
	if (arg_info->pass_by_reference) {
		smart_str_appendc(str, '&');
	}
	if (arg_info->is_variadic) {
		smart_str_appends(str, "...");
	}
	if (arg_info->name) {
		smart_str_append_printf(str, "$%s",
			(fptr->type == ZEND_INTERNAL_FUNCTION &&
			 !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) ?
			((zend_internal_arg_info*)arg_info)->name :
			ZSTR_VAL(arg_info->name));
	} else {
		smart_str_append_printf(str, "$param%d", offset);
	}
	if (fptr->type == ZEND_USER_FUNCTION && !required) {
		zend_op *precv = _get_recv_op((zend_op_array*)fptr, offset);
		if (precv && precv->opcode == ZEND_RECV_INIT && precv->op2_type != IS_UNUSED) {
			zval zv;

			smart_str_appends(str, " = ");
			ZVAL_COPY(&zv, RT_CONSTANT(precv, precv->op2));
			if (UNEXPECTED(zval_update_constant_ex(&zv, fptr->common.scope) == FAILURE)) {
				zval_ptr_dtor(&zv);
				return;
			}
			if (Z_TYPE(zv) == IS_TRUE) {
				smart_str_appends(str, "true");
			} else if (Z_TYPE(zv) == IS_FALSE) {
				smart_str_appends(str, "false");
			} else if (Z_TYPE(zv) == IS_NULL) {
				smart_str_appends(str, "NULL");
			} else if (Z_TYPE(zv) == IS_STRING) {
				smart_str_appendc(str, '\'');
				smart_str_appendl(str, Z_STRVAL(zv), MIN(Z_STRLEN(zv), 15));
				if (Z_STRLEN(zv) > 15) {
					smart_str_appends(str, "...");
				}
				smart_str_appendc(str, '\'');
			} else if (Z_TYPE(zv) == IS_ARRAY) {
				smart_str_appends(str, "Array");
			} else {
				zend_string *tmp_zv_str;
				zend_string *zv_str = zval_get_tmp_string(&zv, &tmp_zv_str);
				smart_str_append(str, zv_str);
				zend_tmp_string_release(tmp_zv_str);
			}
			zval_ptr_dtor(&zv);
		}
	}
	smart_str_appends(str, " ]");
}
/* }}} */

/* {{{ _function_parameter_string */
static void _function_parameter_string(smart_str *str, zend_function *fptr, char* indent)
{
	struct _zend_arg_info *arg_info = fptr->common.arg_info;
	uint32_t i, num_args, num_required = fptr->common.required_num_args;

	if (!arg_info) {
		return;
	}

	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}
	smart_str_appendc(str, '\n');
	smart_str_append_printf(str, "%s- Parameters [%d] {\n", indent, num_args);
	for (i = 0; i < num_args; i++) {
		smart_str_append_printf(str, "%s  ", indent);
		_parameter_string(str, fptr, arg_info, i, i < num_required, indent);
		smart_str_appendc(str, '\n');
		arg_info++;
	}
	smart_str_append_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _function_closure_string */
static void _function_closure_string(smart_str *str, zend_function *fptr, char* indent)
{
	uint32_t i, count;
	zend_string *key;
	HashTable *static_variables;

	if (fptr->type != ZEND_USER_FUNCTION || !fptr->op_array.static_variables) {
		return;
	}

	static_variables = ZEND_MAP_PTR_GET(fptr->op_array.static_variables_ptr);
	count = zend_hash_num_elements(static_variables);

	if (!count) {
		return;
	}

	smart_str_append_printf(str, "\n");
	smart_str_append_printf(str, "%s- Bound Variables [%d] {\n", indent, zend_hash_num_elements(static_variables));
	i = 0;
	ZEND_HASH_FOREACH_STR_KEY(static_variables, key) {
		smart_str_append_printf(str, "%s    Variable #%d [ $%s ]\n", indent, i++, ZSTR_VAL(key));
	} ZEND_HASH_FOREACH_END();
	smart_str_append_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _function_string */
static void _function_string(smart_str *str, zend_function *fptr, zend_class_entry *scope, char* indent)
{
	smart_str param_indent = {0};
	zend_function *overwrites;
	zend_string *lc_name;

	/* TBD: Repair indenting of doc comment (or is this to be done in the parser?)
	 * What's "wrong" is that any whitespace before the doc comment start is
	 * swallowed, leading to an unaligned comment.
	 */
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		smart_str_append_printf(str, "%s%s\n", indent, ZSTR_VAL(fptr->op_array.doc_comment));
	}

	smart_str_appendl(str, indent, strlen(indent));
	smart_str_append_printf(str, fptr->common.fn_flags & ZEND_ACC_CLOSURE ? "Closure [ " : (fptr->common.scope ? "Method [ " : "Function [ "));
	smart_str_append_printf(str, (fptr->type == ZEND_USER_FUNCTION) ? "<user" : "<internal");
	if (fptr->common.fn_flags & ZEND_ACC_DEPRECATED) {
		smart_str_appends(str, ", deprecated");
	}
	if (fptr->type == ZEND_INTERNAL_FUNCTION && ((zend_internal_function*)fptr)->module) {
		smart_str_append_printf(str, ":%s", ((zend_internal_function*)fptr)->module->name);
	}

	if (scope && fptr->common.scope) {
		if (fptr->common.scope != scope) {
			smart_str_append_printf(str, ", inherits %s", ZSTR_VAL(fptr->common.scope->name));
		} else if (fptr->common.scope->parent) {
			lc_name = zend_string_tolower(fptr->common.function_name);
			if ((overwrites = zend_hash_find_ptr(&fptr->common.scope->parent->function_table, lc_name)) != NULL) {
				if (fptr->common.scope != overwrites->common.scope) {
					smart_str_append_printf(str, ", overwrites %s", ZSTR_VAL(overwrites->common.scope->name));
				}
			}
			zend_string_release_ex(lc_name, 0);
		}
	}
	if (fptr->common.prototype && fptr->common.prototype->common.scope) {
		smart_str_append_printf(str, ", prototype %s", ZSTR_VAL(fptr->common.prototype->common.scope->name));
	}
	if (fptr->common.fn_flags & ZEND_ACC_CTOR) {
		smart_str_appends(str, ", ctor");
	}
	if (fptr->common.fn_flags & ZEND_ACC_DTOR) {
		smart_str_appends(str, ", dtor");
	}
	smart_str_appends(str, "> ");

	if (fptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
		smart_str_appends(str, "abstract ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_FINAL) {
		smart_str_appends(str, "final ");
	}
	if (fptr->common.fn_flags & ZEND_ACC_STATIC) {
		smart_str_appends(str, "static ");
	}

	if (fptr->common.scope) {
		/* These are mutually exclusive */
		switch (fptr->common.fn_flags & ZEND_ACC_PPP_MASK) {
			case ZEND_ACC_PUBLIC:
				smart_str_appends(str, "public ");
				break;
			case ZEND_ACC_PRIVATE:
				smart_str_appends(str, "private ");
				break;
			case ZEND_ACC_PROTECTED:
				smart_str_appends(str, "protected ");
				break;
			default:
				smart_str_appends(str, "<visibility error> ");
				break;
		}
		smart_str_appends(str, "method ");
	} else {
		smart_str_appends(str, "function ");
	}

	if (fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) {
		smart_str_appendc(str, '&');
	}
	smart_str_append_printf(str, "%s ] {\n", ZSTR_VAL(fptr->common.function_name));
	/* The information where a function is declared is only available for user classes */
	if (fptr->type == ZEND_USER_FUNCTION) {
		smart_str_append_printf(str, "%s  @@ %s %d - %d\n", indent,
						ZSTR_VAL(fptr->op_array.filename),
						fptr->op_array.line_start,
						fptr->op_array.line_end);
	}
	smart_str_append_printf(&param_indent, "%s  ", indent);
	smart_str_0(&param_indent);
	if (fptr->common.fn_flags & ZEND_ACC_CLOSURE) {
		_function_closure_string(str, fptr, ZSTR_VAL(param_indent.s));
	}
	_function_parameter_string(str, fptr, ZSTR_VAL(param_indent.s));
	smart_str_free(&param_indent);
	if (fptr->op_array.fn_flags & ZEND_ACC_HAS_RETURN_TYPE) {
		smart_str_append_printf(str, "  %s- Return [ ", indent);
		if (ZEND_TYPE_IS_CLASS(fptr->common.arg_info[-1].type)) {
			smart_str_append_printf(str, "%s ",
				ZSTR_VAL(ZEND_TYPE_NAME(fptr->common.arg_info[-1].type)));
			if (ZEND_TYPE_ALLOW_NULL(fptr->common.arg_info[-1].type)) {
				smart_str_appends(str, "or NULL ");
			}
		} else if (ZEND_TYPE_IS_CODE(fptr->common.arg_info[-1].type)) {
			smart_str_append_printf(str, "%s ", zend_get_type_by_const(ZEND_TYPE_CODE(fptr->common.arg_info[-1].type)));
			if (ZEND_TYPE_ALLOW_NULL(fptr->common.arg_info[-1].type)) {
				smart_str_appends(str, "or NULL ");
			}
		}
		smart_str_appends(str, "]\n");
	}
	smart_str_append_printf(str, "%s}\n", indent);
}
/* }}} */

/* {{{ _property_string */
static void _property_string(smart_str *str, zend_property_info *prop, const char *prop_name, char* indent, zend_bool dynamic)
{
	smart_str_append_printf(str, "%sProperty [ ", indent);
	if (!prop) {
		smart_str_append_printf(str, "<dynamic> public $%s", prop_name);
	} else {
		if (!(prop->flags & ZEND_ACC_STATIC)) {
			if (dynamic) {
				smart_str_appends(str, "<implicit> ");
			} else {
				smart_str_appends(str, "<default> ");
			}
		}

		/* These are mutually exclusive */
		switch (prop->flags & ZEND_ACC_PPP_MASK) {
			case ZEND_ACC_PUBLIC:
				smart_str_appends(str, "public ");
				break;
			case ZEND_ACC_PRIVATE:
				smart_str_appends(str, "private ");
				break;
			case ZEND_ACC_PROTECTED:
				smart_str_appends(str, "protected ");
				break;
		}
		if (prop->flags & ZEND_ACC_STATIC) {
			smart_str_appends(str, "static ");
		}
		if (!prop_name) {
			const char *class_name;
			zend_unmangle_property_name(prop->name, &class_name, &prop_name);
		}
		smart_str_append_printf(str, "$%s", prop_name);
	}

	smart_str_appends(str, " ]\n");
}
/* }}} */

static void _extension_ini_string(zend_ini_entry *ini_entry, smart_str *str, char *indent, int number) /* {{{ */
{
	char *comma = "";

	if (number == ini_entry->module_number) {
		smart_str_append_printf(str, "    %sEntry [ %s <", indent, ZSTR_VAL(ini_entry->name));
		if (ini_entry->modifiable == ZEND_INI_ALL) {
			smart_str_appends(str, "ALL");
		} else {
			if (ini_entry->modifiable & ZEND_INI_USER) {
				smart_str_appends(str, "USER");
				comma = ",";
			}
			if (ini_entry->modifiable & ZEND_INI_PERDIR) {
				smart_str_append_printf(str, "%sPERDIR", comma);
				comma = ",";
			}
			if (ini_entry->modifiable & ZEND_INI_SYSTEM) {
				smart_str_append_printf(str, "%sSYSTEM", comma);
			}
		}

		smart_str_appends(str, "> ]\n");
		smart_str_append_printf(str, "    %s  Current = '%s'\n", indent, ini_entry->value ? ZSTR_VAL(ini_entry->value) : "");
		if (ini_entry->modified) {
			smart_str_append_printf(str, "    %s  Default = '%s'\n", indent, ini_entry->orig_value ? ZSTR_VAL(ini_entry->orig_value) : "");
		}
		smart_str_append_printf(str, "    %s}\n", indent);
	}
}
/* }}} */

static void _extension_class_string(zend_class_entry *ce, zend_string *key, smart_str *str, char *indent, zend_module_entry *module, int *num_classes) /* {{{ */
{
	if (ce->type == ZEND_INTERNAL_CLASS && ce->info.internal.module && !strcasecmp(ce->info.internal.module->name, module->name)) {
		/* dump class if it is not an alias */
		if (zend_string_equals_ci(ce->name, key)) {
			smart_str_append_printf(str, "\n");
			_class_string(str, ce, NULL, indent);
			(*num_classes)++;
		}
	}
}
/* }}} */

static void _extension_string(smart_str *str, zend_module_entry *module, char *indent) /* {{{ */
{
	smart_str_append_printf(str, "%sExtension [ ", indent);
	if (module->type == MODULE_PERSISTENT) {
		smart_str_appends(str, "<persistent>");
	}
	if (module->type == MODULE_TEMPORARY) {
		smart_str_appends(str, "<temporary>" );
	}
	smart_str_append_printf(str, " extension #%d %s version %s ] {\n",
					module->module_number, module->name,
					(module->version == NO_VERSION_YET) ? "<no_version>" : module->version);

	if (module->deps) {
		const zend_module_dep* dep = module->deps;

		smart_str_appends(str, "\n  - Dependencies {\n");

		while(dep->name) {
			smart_str_append_printf(str, "%s    Dependency [ %s (", indent, dep->name);

			switch(dep->type) {
			case MODULE_DEP_REQUIRED:
				smart_str_appends(str, "Required");
				break;
			case MODULE_DEP_CONFLICTS:
				smart_str_appends(str, "Conflicts");
				break;
			case MODULE_DEP_OPTIONAL:
				smart_str_appends(str, "Optional");
				break;
			default:
				smart_str_appends(str, "Error"); /* shouldn't happen */
				break;
			}

			if (dep->rel) {
				smart_str_append_printf(str, " %s", dep->rel);
			}
			if (dep->version) {
				smart_str_append_printf(str, " %s", dep->version);
			}
			smart_str_appends(str, ") ]\n");
			dep++;
		}
		smart_str_append_printf(str, "%s  }\n", indent);
	}

	{
		smart_str str_ini = {0};
		zend_ini_entry *ini_entry;
		ZEND_HASH_FOREACH_PTR(EG(ini_directives), ini_entry) {
			_extension_ini_string(ini_entry, &str_ini, indent, module->module_number);
		} ZEND_HASH_FOREACH_END();
		if (smart_str_get_len(&str_ini) > 0) {
			smart_str_append_printf(str, "\n  - INI {\n");
			smart_str_append_smart_str(str, &str_ini);
			smart_str_append_printf(str, "%s  }\n", indent);
		}
		smart_str_free(&str_ini);
	}

	{
		smart_str str_constants = {0};
		zend_constant *constant;
		int num_constants = 0;

		ZEND_HASH_FOREACH_PTR(EG(zend_constants), constant) {
			if (ZEND_CONSTANT_MODULE_NUMBER(constant) == module->module_number) {
				_const_string(&str_constants, ZSTR_VAL(constant->name), &constant->value, indent);
				num_constants++;
			}
		} ZEND_HASH_FOREACH_END();

		if (num_constants) {
			smart_str_append_printf(str, "\n  - Constants [%d] {\n", num_constants);
			smart_str_append_smart_str(str, &str_constants);
			smart_str_append_printf(str, "%s  }\n", indent);
		}
		smart_str_free(&str_constants);
	}

	{
		zend_function *fptr;
		int first = 1;

		ZEND_HASH_FOREACH_PTR(CG(function_table), fptr) {
			if (fptr->common.type==ZEND_INTERNAL_FUNCTION
				&& fptr->internal_function.module == module) {
				if (first) {
					smart_str_append_printf(str, "\n  - Functions {\n");
					first = 0;
				}
				_function_string(str, fptr, NULL, "    ");
			}
		} ZEND_HASH_FOREACH_END();
		if (!first) {
			smart_str_append_printf(str, "%s  }\n", indent);
		}
	}

	{
		zend_string *sub_indent = strpprintf(0, "%s    ", indent);
		smart_str str_classes = {0};
		zend_string *key;
		zend_class_entry *ce;
		int num_classes = 0;

		ZEND_HASH_FOREACH_STR_KEY_PTR(EG(class_table), key, ce) {
			_extension_class_string(ce, key, &str_classes, ZSTR_VAL(sub_indent), module, &num_classes);
		} ZEND_HASH_FOREACH_END();
		if (num_classes) {
			smart_str_append_printf(str, "\n  - Classes [%d] {", num_classes);
			smart_str_append_smart_str(str, &str_classes);
			smart_str_append_printf(str, "%s  }\n", indent);
		}
		smart_str_free(&str_classes);
		zend_string_release_ex(sub_indent, 0);
	}

	smart_str_append_printf(str, "%s}\n", indent);
}
/* }}} */

static void _zend_extension_string(smart_str *str, zend_extension *extension, char *indent) /* {{{ */
{
	smart_str_append_printf(str, "%sZend Extension [ %s ", indent, extension->name);

	if (extension->version) {
		smart_str_append_printf(str, "%s ", extension->version);
	}
	if (extension->copyright) {
		smart_str_append_printf(str, "%s ", extension->copyright);
	}
	if (extension->author) {
		smart_str_append_printf(str, "by %s ", extension->author);
	}
	if (extension->URL) {
		smart_str_append_printf(str, "<%s> ", extension->URL);
	}

	smart_str_appends(str, "]\n");
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

	reflection_instantiate(reflection_class_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = ce;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = ce;
	ZVAL_STR_COPY(reflection_prop_name(object), ce->name);
}
/* }}} */

/* {{{ reflection_extension_factory */
static void reflection_extension_factory(zval *object, const char *name_str)
{
	reflection_object *intern;
	size_t name_len = strlen(name_str);
	zend_string *lcname;
	struct _zend_module_entry *module;

	lcname = zend_string_alloc(name_len, 0);
	zend_str_tolower_copy(ZSTR_VAL(lcname), name_str, name_len);
	module = zend_hash_find_ptr(&module_registry, lcname);
	zend_string_efree(lcname);
	if (!module) {
		return;
	}

	reflection_instantiate(reflection_extension_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = module;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = NULL;
	ZVAL_STRINGL(reflection_prop_name(object), module->name, name_len);
}
/* }}} */

/* {{{ reflection_parameter_factory */
static void reflection_parameter_factory(zend_function *fptr, zval *closure_object, struct _zend_arg_info *arg_info, uint32_t offset, zend_bool required, zval *object)
{
	reflection_object *intern;
	parameter_reference *reference;
	zval *prop_name;

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
		ZVAL_OBJ(&intern->obj, Z_OBJ_P(closure_object));
	}

	prop_name = reflection_prop_name(object);
	if (arg_info->name) {
		if (fptr->type == ZEND_INTERNAL_FUNCTION &&
		    !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
			ZVAL_STRING(prop_name, ((zend_internal_arg_info*)arg_info)->name);
		} else {
			ZVAL_STR_COPY(prop_name, arg_info->name);
		}
	} else {
		ZVAL_NULL(prop_name);
	}
}
/* }}} */

/* {{{ reflection_type_factory */
static void reflection_type_factory(zend_type type, zval *object)
{
	reflection_object *intern;
	type_reference *reference;

	reflection_instantiate(reflection_named_type_ptr, object);
	intern = Z_REFLECTION_P(object);
	reference = (type_reference*) emalloc(sizeof(type_reference));
	reference->type = type;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_TYPE;

	/* Property types may be resolved during the lifetime of the ReflectionType,
	 * so we need to make sure that the strings we reference are not released. */
	if (ZEND_TYPE_IS_NAME(type)) {
		zend_string_addref(ZEND_TYPE_NAME(type));
	}
}
/* }}} */

/* {{{ reflection_function_factory */
static void reflection_function_factory(zend_function *function, zval *closure_object, zval *object)
{
	reflection_object *intern;
	reflection_instantiate(reflection_function_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = function;
	intern->ref_type = REF_TYPE_FUNCTION;
	intern->ce = NULL;
	if (closure_object) {
		Z_ADDREF_P(closure_object);
		ZVAL_OBJ(&intern->obj, Z_OBJ_P(closure_object));
	}
	ZVAL_STR_COPY(reflection_prop_name(object), function->common.function_name);
}
/* }}} */

/* {{{ reflection_method_factory */
static void reflection_method_factory(zend_class_entry *ce, zend_function *method, zval *closure_object, zval *object)
{
	reflection_object *intern;

	reflection_instantiate(reflection_method_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = method;
	intern->ref_type = REF_TYPE_FUNCTION;
	intern->ce = ce;
	if (closure_object) {
		Z_ADDREF_P(closure_object);
		ZVAL_OBJ(&intern->obj, Z_OBJ_P(closure_object));
	}

	ZVAL_STR_COPY(reflection_prop_name(object),
		(method->common.scope && method->common.scope->trait_aliases)
			? zend_resolve_method_name(ce, method) : method->common.function_name);
	ZVAL_STR_COPY(reflection_prop_class(object), method->common.scope->name);
}
/* }}} */

/* {{{ reflection_property_factory */
static void reflection_property_factory(zend_class_entry *ce, zend_string *name, zend_property_info *prop, zval *object, zend_bool dynamic)
{
	reflection_object *intern;
	property_reference *reference;

	if (!(prop->flags & ZEND_ACC_PRIVATE)) {
		/* we have to search the class hierarchy for this (implicit) public or protected property */
		zend_class_entry *tmp_ce = ce, *store_ce = ce;
		zend_property_info *tmp_info = NULL;

		while (tmp_ce && (tmp_info = zend_hash_find_ptr(&tmp_ce->properties_info, name)) == NULL) {
			ce = tmp_ce;
			tmp_ce = tmp_ce->parent;
		}

		if (tmp_info && (!(tmp_info->flags & ZEND_ACC_PRIVATE) || tmp_info->ce == tmp_ce)) { /* found something and it's not a parent's private */
			prop = tmp_info;
		} else { /* not found, use initial value */
			ce = store_ce;
		}
	}

	reflection_instantiate(reflection_property_ptr, object);
	intern = Z_REFLECTION_P(object);
	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->prop = *prop;

	if (ZEND_TYPE_IS_NAME(reference->prop.type)) {
		zend_string_addref(ZEND_TYPE_NAME(reference->prop.type));
	}

	reference->unmangled_name = zend_string_copy(name);
	reference->dynamic = dynamic;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_PROPERTY;
	intern->ce = ce;
	intern->ignore_visibility = 0;
	ZVAL_STR_COPY(reflection_prop_name(object), name);
	ZVAL_STR_COPY(reflection_prop_class(object), prop->ce->name);
}
/* }}} */

static void reflection_property_factory_str(zend_class_entry *ce, const char *name_str, size_t name_len, zend_property_info *prop, zval *object)
{
	zend_string *name = zend_string_init(name_str, name_len, 0);
	reflection_property_factory(ce, name, prop, object, 0);
	zend_string_release(name);
}

/* {{{ reflection_class_constant_factory */
static void reflection_class_constant_factory(zend_class_entry *ce, zend_string *name_str, zend_class_constant *constant, zval *object)
{
	reflection_object *intern;

	reflection_instantiate(reflection_class_constant_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = constant;
	intern->ref_type = REF_TYPE_CLASS_CONSTANT;
	intern->ce = constant->ce;
	intern->ignore_visibility = 0;

	ZVAL_STR_COPY(reflection_prop_name(object), name_str);
	ZVAL_STR_COPY(reflection_prop_class(object), ce->name);
}
/* }}} */

static void reflection_export_impl(zval *return_value, zval *object, zend_bool return_output) {
	zval fname, retval;
	int result;

	/* Invoke the __toString() method */
	ZVAL_STRINGL(&fname, "__tostring", sizeof("__tostring") - 1);
	result = call_user_function(NULL, object, &fname, &retval, 0, NULL);
	zval_ptr_dtor_str(&fname);

	if (result == FAILURE) {
		_DO_THROW("Invocation of method __toString() failed");
		return;
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

/* {{{ _reflection_export */
static void _reflection_export(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *ce_ptr, int ctor_argc)
{
	zval reflector;
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
	if (object_init_ex(&reflector, ce_ptr) == FAILURE) {
		_DO_THROW("Could not create reflector");
		return;
	}

	/* Call __construct() */

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = Z_OBJ(reflector);
	fci.retval = &retval;
	fci.param_count = ctor_argc;
	fci.params = params;
	fci.no_separation = 1;

	fcc.function_handler = ce_ptr->constructor;
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
		return;
	}

	reflection_export_impl(return_value, &reflector, return_output);

	/* Destruct reflector which is no longer needed */
	zval_ptr_dtor(&reflector);
}
/* }}} */

/* {{{ _reflection_param_get_default_param */
static parameter_reference *_reflection_param_get_default_param(INTERNAL_FUNCTION_PARAMETERS)
{
	reflection_object *intern;
	parameter_reference *param;

	intern = Z_REFLECTION_P(ZEND_THIS);
	if (intern->ptr == NULL) {
		if (EG(exception) && EG(exception)->ce == reflection_exception_ptr) {
			return NULL;
		}
		zend_throw_error(NULL, "Internal error: Failed to retrieve the reflection object");
		return NULL;
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
	zval *object;
	zend_bool return_output = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJECT_OF_CLASS(object, reflector_ptr)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(return_output)
	ZEND_PARSE_PARAMETERS_END();

	reflection_export_impl(return_value, object, return_output);
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
	zval *object;
	zval *closure = NULL;
	reflection_object *intern;
	zend_function *fptr;
	zend_string *fname, *lcname;

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "O", &closure, zend_ce_closure) == SUCCESS) {
		fptr = (zend_function*)zend_get_closure_method_def(closure);
		Z_ADDREF_P(closure);
	} else {
		ALLOCA_FLAG(use_heap)

		if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &fname) == FAILURE) {
			return;
		}

		if (UNEXPECTED(ZSTR_VAL(fname)[0] == '\\')) {
			/* Ignore leading "\" */
			ZSTR_ALLOCA_ALLOC(lcname, ZSTR_LEN(fname) - 1, use_heap);
			zend_str_tolower_copy(ZSTR_VAL(lcname), ZSTR_VAL(fname) + 1, ZSTR_LEN(fname) - 1);
			fptr = zend_fetch_function(lcname);
			ZSTR_ALLOCA_FREE(lcname, use_heap);
		} else {
			lcname = zend_string_tolower(fname);
			fptr = zend_fetch_function(lcname);
			zend_string_release(lcname);
		}

		if (fptr == NULL) {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Function %s() does not exist", ZSTR_VAL(fname));
			return;
		}
	}

	if (intern->ptr) {
		zval_ptr_dtor(&intern->obj);
		zval_ptr_dtor(reflection_prop_name(object));
	}

	ZVAL_STR_COPY(reflection_prop_name(object), fptr->common.function_name);
	intern->ptr = fptr;
	intern->ref_type = REF_TYPE_FUNCTION;
	if (closure) {
		ZVAL_OBJ(&intern->obj, Z_OBJ_P(closure));
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
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	_function_string(&str, fptr, intern->ce, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ proto public string ReflectionFunction::getName()
   Returns this function's name */
ZEND_METHOD(reflection_function, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_name(ZEND_THIS, return_value);
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
			Z_ADDREF_P(closure_this);
			ZVAL_OBJ(return_value, Z_OBJ_P(closure_this));
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
		Z_ADDREF(intern->obj);
		ZVAL_OBJ(return_value, Z_OBJ(intern->obj));
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
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.static_variables != NULL) {
		HashTable *ht;

		array_init(return_value);
		ht = ZEND_MAP_PTR_GET(fptr->op_array.static_variables_ptr);
		if (!ht) {
			ZEND_ASSERT(fptr->op_array.fn_flags & ZEND_ACC_IMMUTABLE);
			ht = zend_array_dup(fptr->op_array.static_variables);
			ZEND_MAP_PTR_SET(fptr->op_array.static_variables_ptr, ht);
		}
		ZEND_HASH_FOREACH_VAL(ht, val) {
			if (UNEXPECTED(zval_update_constant_ex(val, fptr->common.scope) != SUCCESS)) {
				return;
			}
		} ZEND_HASH_FOREACH_END();
		zend_hash_copy(Z_ARRVAL_P(return_value), ht, zval_add_ref);
	} else {
		RETURN_EMPTY_ARRAY();
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

	fcc.function_handler = fptr;
	fcc.called_scope = NULL;
	fcc.object = NULL;

	if (!Z_ISUNDEF(intern->obj)) {
		Z_OBJ_HT(intern->obj)->get_closure(
			&intern->obj, &fcc.called_scope, &fcc.function_handler, &fcc.object);
	}

	result = zend_call_function(&fci, &fcc);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of function %s() failed", ZSTR_VAL(fptr->common.function_name));
		return;
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
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

	fcc.function_handler = fptr;
	fcc.called_scope = NULL;
	fcc.object = NULL;

	if (!Z_ISUNDEF(intern->obj)) {
		Z_OBJ_HT(intern->obj)->get_closure(
			&intern->obj, &fcc.called_scope, &fcc.function_handler, &fcc.object);
	}

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
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
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

	GET_REFLECTION_OBJECT_PTR(fptr);

	RETURN_BOOL((fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0);
}
/* }}} */

/* {{{ proto public bool ReflectionFunction::getNumberOfParameters()
   Gets the number of parameters */
ZEND_METHOD(reflection_function, getNumberOfParameters)
{
	reflection_object *intern;
	zend_function *fptr;
	uint32_t num_args;

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

	GET_REFLECTION_OBJECT_PTR(fptr);

	arg_info= fptr->common.arg_info;
	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}

	if (!num_args) {
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);
	for (i = 0; i < num_args; i++) {
		zval parameter;

		reflection_parameter_factory(
			_copy_function(fptr),
			Z_ISUNDEF(intern->obj) ? NULL : &intern->obj,
			arg_info,
			i,
			i < fptr->common.required_num_args,
			&parameter
		);
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

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "O", &generator, zend_ce_generator) == FAILURE) {
		return;
	}

	ex = ((zend_generator *) Z_OBJ_P(generator))->execute_data;
	if (!ex) {
		_DO_THROW("Cannot create ReflectionGenerator based on a terminated Generator");
		return;
	}

	if (intern->ce) {
		zval_ptr_dtor(&intern->obj);
	}

	intern->ref_type = REF_TYPE_GENERATOR;
	Z_ADDREF_P(generator);
	ZVAL_OBJ(&intern->obj, Z_OBJ_P(generator));
	intern->ce = zend_ce_generator;
}
/* }}} */

#define REFLECTION_CHECK_VALID_GENERATOR(ex) \
	if (!ex) { \
		_DO_THROW("Cannot fetch information from a terminated Generator"); \
		return; \
	}

/* {{{ proto public array ReflectionGenerator::getTrace($options = DEBUG_BACKTRACE_PROVIDE_OBJECT) */
ZEND_METHOD(reflection_generator, getTrace)
{
	zend_long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
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
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
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
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
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
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	if (ex->func->common.fn_flags & ZEND_ACC_CLOSURE) {
		zval closure;
		ZVAL_OBJ(&closure, ZEND_CLOSURE_OBJECT(ex->func));
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
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	if (Z_TYPE(ex->This) == IS_OBJECT) {
		Z_ADDREF(ex->This);
		ZVAL_OBJ(return_value, Z_OBJ(ex->This));
	} else {
		ZVAL_NULL(return_value);
	}
}
/* }}} */

/* {{{ proto public Generator ReflectionGenerator::getExecutingGenerator() */
ZEND_METHOD(reflection_generator, getExecutingGenerator)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;
	zend_generator *current;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	current = zend_generator_get_current(generator);
	GC_ADDREF(&current->std);

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
	zval *prop_name;
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

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	/* First, find the function */
	switch (Z_TYPE_P(reference)) {
		case IS_STRING:
			{
				zend_string *lcname = zend_string_tolower(Z_STR_P(reference));
				fptr = zend_hash_find_ptr(EG(function_table), lcname);
				zend_string_release(lcname);
				if (!fptr) {
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Function %s() does not exist", Z_STRVAL_P(reference));
					return;
				}
				ce = fptr->common.scope;
			}
			break;

		case IS_ARRAY: {
				zval *classref;
				zval *method;
				zend_string *name, *lcname;

				if (((classref = zend_hash_index_find(Z_ARRVAL_P(reference), 0)) == NULL)
					|| ((method = zend_hash_index_find(Z_ARRVAL_P(reference), 1)) == NULL))
				{
					_DO_THROW("Expected array($object, $method) or array($classname, $method)");
					return;
				}

				if (Z_TYPE_P(classref) == IS_OBJECT) {
					ce = Z_OBJCE_P(classref);
				} else {
					name = zval_try_get_string(classref);
					if (UNEXPECTED(!name)) {
						return;
					}
					if ((ce = zend_lookup_class(name)) == NULL) {
						zend_throw_exception_ex(reflection_exception_ptr, 0,
								"Class %s does not exist", ZSTR_VAL(name));
						zend_string_release(name);
						return;
					}
					zend_string_release(name);
				}

				name = zval_try_get_string(method);
				if (UNEXPECTED(!name)) {
					return;
				}

				lcname = zend_string_tolower(name);
				if (Z_TYPE_P(classref) == IS_OBJECT && is_closure_invoke(ce, lcname)
					&& (fptr = zend_get_closure_invoke_method(Z_OBJ_P(classref))) != NULL)
				{
					/* nothing to do. don't set is_closure since is the invoke handler,
					   not the closure itself */
				} else if ((fptr = zend_hash_find_ptr(&ce->function_table, lcname)) == NULL) {
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Method %s::%s() does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(name));
					zend_string_release(name);
					zend_string_release(lcname);
					return;
				}
				zend_string_release(name);
				zend_string_release(lcname);
			}
			break;

		case IS_OBJECT: {
				ce = Z_OBJCE_P(reference);

				if (instanceof_function(ce, zend_ce_closure)) {
					fptr = (zend_function *)zend_get_closure_method_def(reference);
					Z_ADDREF_P(reference);
					is_closure = 1;
				} else if ((fptr = zend_hash_find_ptr(&ce->function_table, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) == NULL) {
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Method %s::%s() does not exist", ZSTR_VAL(ce->name), ZEND_INVOKE_FUNC_NAME);
					return;
				}
			}
			break;

		default:
			_DO_THROW("The parameter class is expected to be either a string, an array(class, method) or a callable object");
			return;
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
			_DO_THROW("The parameter specified by its offset could not be found");
			goto failure;
		}
	} else {
		uint32_t i;

		position = -1;
		if (!try_convert_to_string(parameter)) {
			goto failure;
		}

		if (fptr->type == ZEND_INTERNAL_FUNCTION &&
		    !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
			for (i = 0; i < num_args; i++) {
				if (arg_info[i].name) {
					if (strcmp(((zend_internal_arg_info*)arg_info)[i].name, Z_STRVAL_P(parameter)) == 0) {
						position = i;
						break;
					}

				}
			}
		} else {
			for (i = 0; i < num_args; i++) {
				if (arg_info[i].name) {
					if (strcmp(ZSTR_VAL(arg_info[i].name), Z_STRVAL_P(parameter)) == 0) {
						position = i;
						break;
					}
				}
			}
		}
		if (position == -1) {
			_DO_THROW("The parameter specified by its name could not be found");
			goto failure;
		}
	}

	ref = (parameter_reference*) emalloc(sizeof(parameter_reference));
	ref->arg_info = &arg_info[position];
	ref->offset = (uint32_t)position;
	ref->required = (uint32_t)position < fptr->common.required_num_args;
	ref->fptr = fptr;
	/* TODO: copy fptr */
	intern->ptr = ref;
	intern->ref_type = REF_TYPE_PARAMETER;
	intern->ce = ce;
	if (reference && is_closure) {
		ZVAL_COPY_VALUE(&intern->obj, reference);
	}

	prop_name = reflection_prop_name(object);
	if (arg_info[position].name) {
		if (fptr->type == ZEND_INTERNAL_FUNCTION &&
		    !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO)) {
			ZVAL_STRING(prop_name, ((zend_internal_arg_info*)arg_info)[position].name);
		} else {
			ZVAL_STR_COPY(prop_name, arg_info[position].name);
		}
	} else {
		ZVAL_NULL(prop_name);
	}
	return;

failure:
	if (fptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		if (fptr->type != ZEND_OVERLOADED_FUNCTION) {
			zend_string_release_ex(fptr->common.function_name, 0);
		}
		zend_free_trampoline(fptr);
	}
	if (is_closure) {
		zval_ptr_dtor(reference);
	}
}
/* }}} */

/* {{{ proto public string ReflectionParameter::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_parameter, __toString)
{
	reflection_object *intern;
	parameter_reference *param;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);
	_parameter_string(&str, param->fptr, param->arg_info, param->offset, param->required, "");
	RETURN_STR(smart_str_extract(&str));
}

/* }}} */

/* {{{ proto public string ReflectionParameter::getName()
   Returns this parameters's name */
ZEND_METHOD(reflection_parameter, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_name(ZEND_THIS, return_value);
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

	if (ZEND_TYPE_IS_CLASS(param->arg_info->type)) {
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
		zend_string *class_name;

		class_name = ZEND_TYPE_NAME(param->arg_info->type);
		if (0 == zend_binary_strcasecmp(ZSTR_VAL(class_name), ZSTR_LEN(class_name), "self", sizeof("self")- 1)) {
			ce = param->fptr->common.scope;
			if (!ce) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter uses 'self' as type hint but function is not a class member!");
				return;
			}
		} else if (0 == zend_binary_strcasecmp(ZSTR_VAL(class_name), ZSTR_LEN(class_name), "parent", sizeof("parent")- 1)) {
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
			ce = zend_lookup_class(class_name);
			if (!ce) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Class %s does not exist", ZSTR_VAL(class_name));
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

	RETVAL_BOOL(ZEND_TYPE_IS_SET(param->arg_info->type));
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

	if (!ZEND_TYPE_IS_SET(param->arg_info->type)) {
		RETURN_NULL();
	}
	reflection_type_factory(param->arg_info->type, return_value);
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

	RETVAL_BOOL(ZEND_TYPE_CODE(param->arg_info->type) == IS_ARRAY);
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

	RETVAL_BOOL(ZEND_TYPE_CODE(param->arg_info->type) == IS_CALLABLE);
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

	RETVAL_BOOL(ZEND_TYPE_ALLOW_NULL(param->arg_info->type));
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

	RETVAL_BOOL(!param->required);
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

	ZVAL_COPY(return_value, RT_CONSTANT(precv, precv->op2));
	if (Z_TYPE_P(return_value) == IS_CONSTANT_AST) {
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
	if (precv && Z_TYPE_P(RT_CONSTANT(precv, precv->op2)) == IS_CONSTANT_AST) {
		zend_ast *ast = Z_ASTVAL_P(RT_CONSTANT(precv, precv->op2));

		if (ast->kind == ZEND_AST_CONSTANT
		 || ast->kind == ZEND_AST_CONSTANT_CLASS) {
			RETURN_TRUE;
		}
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
	if (precv && Z_TYPE_P(RT_CONSTANT(precv, precv->op2)) == IS_CONSTANT_AST) {
		zend_ast *ast = Z_ASTVAL_P(RT_CONSTANT(precv, precv->op2));

		if (ast->kind == ZEND_AST_CONSTANT) {
			RETURN_STR_COPY(zend_ast_get_constant_name(ast));
		} else if (ast->kind == ZEND_AST_CONSTANT_CLASS) {
			RETURN_STRINGL("__CLASS__", sizeof("__CLASS__")-1);
		}
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

	RETVAL_BOOL(ZEND_TYPE_ALLOW_NULL(param->type));
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

	RETVAL_BOOL(ZEND_TYPE_IS_CODE(param->type));
}
/* }}} */

/* {{{ reflection_type_name */
static zend_string *reflection_type_name(type_reference *param) {
	if (ZEND_TYPE_IS_NAME(param->type)) {
		return zend_string_copy(ZEND_TYPE_NAME(param->type));
	} else if (ZEND_TYPE_IS_CE(param->type)) {
		return zend_string_copy(ZEND_TYPE_CE(param->type)->name);
	} else {
		char *name = zend_get_type_by_const(ZEND_TYPE_CODE(param->type));
		return zend_string_init(name, strlen(name), 0);
	}
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

	RETURN_STR(reflection_type_name(param));
}
/* }}} */

/* {{{ proto public string ReflectionNamedType::getName()
 Return the text of the type hint */
ZEND_METHOD(reflection_named_type, getName)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETURN_STR(reflection_type_name(param));
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
	zval *classname;
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

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	/* Find the class entry */
	switch (Z_TYPE_P(classname)) {
		case IS_STRING:
			if ((ce = zend_lookup_class(Z_STR_P(classname))) == NULL) {
				if (!EG(exception)) {
					zend_throw_exception_ex(reflection_exception_ptr, 0,
							"Class %s does not exist", Z_STRVAL_P(classname));
				}
				if (classname == &ztmp) {
					zval_ptr_dtor_str(&ztmp);
				}
				return;
			}
			break;

		case IS_OBJECT:
			ce = Z_OBJCE_P(classname);
			break;

		default:
			if (classname == &ztmp) {
				zval_ptr_dtor_str(&ztmp);
			}
			_DO_THROW("The parameter class is expected to be either a string or an object");
			return;
	}

	if (classname == &ztmp) {
		zval_ptr_dtor_str(&ztmp);
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

	ZVAL_STR_COPY(reflection_prop_name(object), mptr->common.function_name);
	ZVAL_STR_COPY(reflection_prop_class(object), mptr->common.scope->name);
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
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	_function_string(&str, mptr, intern->ce, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ proto public mixed ReflectionMethod::getClosure([mixed object])
   Invokes the function */
ZEND_METHOD(reflection_method, getClosure)
{
	reflection_object *intern;
	zval *obj;
	zend_function *mptr;

	GET_REFLECTION_OBJECT_PTR(mptr);

	if (mptr->common.fn_flags & ZEND_ACC_STATIC)  {
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, mptr->common.scope, NULL);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &obj) == FAILURE) {
			return;
		}

		if (!instanceof_function(Z_OBJCE_P(obj), mptr->common.scope)) {
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			return;
		}

		/* This is an original closure object and __invoke is to be called. */
		if (Z_OBJCE_P(obj) == zend_ce_closure &&
			(mptr->internal_function.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))
		{
			Z_ADDREF_P(obj);
			ZVAL_OBJ(return_value, Z_OBJ_P(obj));
		} else {
			zend_create_fake_closure(return_value, mptr, mptr->common.scope, Z_OBJCE_P(obj), obj);
		}
	}
}
/* }}} */

/* {{{ reflection_method_invoke */
static void reflection_method_invoke(INTERNAL_FUNCTION_PARAMETERS, int variadic)
{
	zval retval;
	zval *params = NULL, *val, *object;
	reflection_object *intern;
	zend_function *mptr;
	int i, argc = 0, result;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_class_entry *obj_ce;
	zval *param_array;

	GET_REFLECTION_OBJECT_PTR(mptr);

	if (mptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Trying to invoke abstract method %s::%s()",
			ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		return;
	}

	if (!(mptr->common.fn_flags & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Trying to invoke %s method %s::%s() from scope %s",
			mptr->common.fn_flags & ZEND_ACC_PROTECTED ? "protected" : "private",
			ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name),
			ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		return;
	}

	if (variadic) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o!*", &object, &params, &argc) == FAILURE) {
			return;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o!a", &object, &param_array) == FAILURE) {
			return;
		}

		argc = zend_hash_num_elements(Z_ARRVAL_P(param_array));

		params = safe_emalloc(sizeof(zval), argc, 0);
		argc = 0;
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(param_array), val) {
			ZVAL_COPY(&params[argc], val);
			argc++;
		} ZEND_HASH_FOREACH_END();
	}

	/* In case this is a static method, we shouldn't pass an object_ptr
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
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Trying to invoke non static method %s::%s() without an object",
				ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
			return;
		}

		obj_ce = Z_OBJCE_P(object);

		if (!instanceof_function(obj_ce, mptr->common.scope)) {
			if (!variadic) {
				efree(params);
			}
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			return;
		}
	}

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = object ? Z_OBJ_P(object) : NULL;
	fci.retval = &retval;
	fci.param_count = argc;
	fci.params = params;
	fci.no_separation = 1;

	fcc.function_handler = mptr;
	fcc.called_scope = intern->ce;
	fcc.object = object ? Z_OBJ_P(object) : NULL;

	/*
	 * Copy the zend_function when calling via handler (e.g. Closure::__invoke())
	 */
	if ((mptr->internal_function.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
		fcc.function_handler = _copy_function(mptr);
	}

	result = zend_call_function(&fci, &fcc);

	if (!variadic) {
		for (i = 0; i < argc; i++) {
			zval_ptr_dtor(&params[i]);
		}
		efree(params);
	}

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of method %s::%s() failed", ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		return;
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionMethod::invoke(mixed object, mixed* args)
   Invokes the method. */
ZEND_METHOD(reflection_method, invoke)
{
	reflection_method_invoke(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto public mixed ReflectionMethod::invokeArgs(mixed object, array args)
   Invokes the function and pass its arguments as array. */
ZEND_METHOD(reflection_method, invokeArgs)
{
	reflection_method_invoke(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
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
	if ((name = _default_load_name(ZEND_THIS)) == NULL) {
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
	if ((name = _default_load_name(ZEND_THIS)) == NULL) {
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
	if ((name = _default_load_name(ZEND_THIS)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_STRINGL(backslash + 1, Z_STRLEN_P(name) - (backslash - Z_STRVAL_P(name) + 1));
	}
	ZVAL_COPY_DEREF(return_value, name);
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

	reflection_type_factory(fptr->common.arg_info[-1].type, return_value);
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
	uint32_t keep_flags = ZEND_ACC_PPP_MASK
		| ZEND_ACC_STATIC | ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(mptr);

	RETURN_LONG((mptr->common.fn_flags & keep_flags));
}
/* }}} */

/* {{{ proto public ReflectionClass ReflectionMethod::getDeclaringClass()
   Get the declaring class */
ZEND_METHOD(reflection_method, getDeclaringClass)
{
	reflection_object *intern;
	zend_function *mptr;

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

	intern = Z_REFLECTION_P(ZEND_THIS);

	intern->ignore_visibility = visible;
}
/* }}} */

/* {{{ proto public void ReflectionClassConstant::__construct(mixed class, string name)
   Constructor. Throws an Exception in case the given class constant does not exist */
ZEND_METHOD(reflection_class_constant, __construct)
{
	zval *classname, *object;
	zend_string *constname;
	reflection_object *intern;
	zend_class_entry *ce;
	zend_class_constant *constant = NULL;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zS", &classname, &constname) == FAILURE) {
		return;
	}

	object = ZEND_THIS;
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
			return;
	}

	if ((constant = zend_hash_find_ptr(&ce->constants_table, constname)) == NULL) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class Constant %s::%s does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(constname));
		return;
	}

	intern->ptr = constant;
	intern->ref_type = REF_TYPE_CLASS_CONSTANT;
	intern->ce = constant->ce;
	intern->ignore_visibility = 0;
	ZVAL_STR_COPY(reflection_prop_name(object), constname);
	ZVAL_STR_COPY(reflection_prop_class(object), ce->name);
}
/* }}} */

/* {{{ proto public string ReflectionClassConstant::__toString()
   Returns a string representation */
ZEND_METHOD(reflection_class_constant, __toString)
{
	reflection_object *intern;
	zend_class_constant *ref;
	smart_str str = {0};
	zval name;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	_default_get_name(ZEND_THIS, &name);
	_class_const_string(&str, Z_STRVAL(name), ref, "");
	zval_ptr_dtor(&name);
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ proto public string ReflectionClassConstant::getName()
   Returns the constant' name */
ZEND_METHOD(reflection_class_constant, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_name(ZEND_THIS, return_value);
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
	_class_constant_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
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

	ZVAL_COPY_OR_DUP(return_value, &ref->value);
	if (Z_TYPE_P(return_value) == IS_CONSTANT_AST) {
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
	reflection_object *intern;
	zend_class_entry *ce;

	if (is_object) {
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_OBJECT(argument)
		ZEND_PARSE_PARAMETERS_END();
	} else {
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_ZVAL(argument)
		ZEND_PARSE_PARAMETERS_END();
	}

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	if (Z_TYPE_P(argument) == IS_OBJECT) {
		ZVAL_STR_COPY(reflection_prop_name(object), Z_OBJCE_P(argument)->name);
		intern->ptr = Z_OBJCE_P(argument);
		if (is_object) {
			ZVAL_COPY(&intern->obj, argument);
		}
	} else {
		if (!try_convert_to_string(argument)) {
			return;
		}

		if ((ce = zend_lookup_class(Z_STR_P(argument))) == NULL) {
			if (!EG(exception)) {
				zend_throw_exception_ex(reflection_exception_ptr, -1, "Class %s does not exist", Z_STRVAL_P(argument));
			}
			return;
		}

		ZVAL_STR_COPY(reflection_prop_name(object), ce->name);
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
		if (((prop_info->flags & ZEND_ACC_PRIVATE) &&
		     prop_info->ce != ce)) {
			continue;
		}
		prop = NULL;
		if (statics && (prop_info->flags & ZEND_ACC_STATIC) != 0) {
			prop = &ce->default_static_members_table[prop_info->offset];
			ZVAL_DEINDIRECT(prop);
		} else if (!statics && (prop_info->flags & ZEND_ACC_STATIC) == 0) {
			prop = &ce->default_properties_table[OBJ_PROP_TO_NUM(prop_info->offset)];
		}
		if (!prop || (prop_info->type && Z_ISUNDEF_P(prop))) {
			continue;
		}

		/* copy: enforce read only access */
		ZVAL_DEREF(prop);
		ZVAL_COPY_OR_DUP(&prop_copy, prop);

		/* this is necessary to make it able to work with default array
		* properties, returned to user */
		if (Z_TYPE(prop_copy) == IS_CONSTANT_AST) {
			if (UNEXPECTED(zval_update_constant_ex(&prop_copy, ce) != SUCCESS)) {
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
	zend_property_info *prop_info;
	zval *prop;
	zend_string *key;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		return;
	}

	if (ce->default_static_members_count && !CE_STATIC_MEMBERS(ce)) {
		zend_class_init_statics(ce);
	}

	array_init(return_value);

	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->properties_info, key, prop_info) {
		if (((prop_info->flags & ZEND_ACC_PRIVATE) &&
		     prop_info->ce != ce)) {
			continue;
		}
		if ((prop_info->flags & ZEND_ACC_STATIC) == 0) {
			continue;
		}

		prop = &CE_STATIC_MEMBERS(ce)[prop_info->offset];
		ZVAL_DEINDIRECT(prop);

		if (prop_info->type && Z_ISUNDEF_P(prop)) {
			continue;
		}

		/* enforce read only access */
		ZVAL_DEREF(prop);
		Z_TRY_ADDREF_P(prop);

		zend_hash_update(Z_ARRVAL_P(return_value), key, prop);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto public mixed ReflectionClass::getStaticPropertyValue(string name [, mixed default])
   Returns the value of a static property */
ZEND_METHOD(reflection_class, getStaticPropertyValue)
{
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	zend_string *name;
	zval *prop, *def_value = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def_value) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		return;
	}

	old_scope = EG(fake_scope);
	EG(fake_scope) = ce;
	prop = zend_std_get_static_property(ce, name, BP_VAR_IS);
	EG(fake_scope) = old_scope;

	if (!prop) {
		if (def_value) {
			ZVAL_COPY(return_value, def_value);
		} else {
			zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Class %s does not have a property named %s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}
		return;
	} else {
		ZVAL_COPY_DEREF(return_value, prop);
	}
}
/* }}} */

/* {{{ proto public void ReflectionClass::setStaticPropertyValue(string $name, mixed $value)
   Sets the value of a static property */
ZEND_METHOD(reflection_class, setStaticPropertyValue)
{
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	zend_property_info *prop_info;
	zend_string *name;
	zval *variable_ptr, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		return;
	}
	old_scope = EG(fake_scope);
	EG(fake_scope) = ce;
	variable_ptr =  zend_std_get_static_property_with_info(ce, name, BP_VAR_W, &prop_info);
	EG(fake_scope) = old_scope;
	if (!variable_ptr) {
		zend_clear_exception();
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Class %s does not have a property named %s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		return;
	}

	if (Z_ISREF_P(variable_ptr)) {
		zend_reference *ref = Z_REF_P(variable_ptr);
		variable_ptr = Z_REFVAL_P(variable_ptr);

		if (!zend_verify_ref_assignable_zval(ref, value, 0)) {
			return;
		}
	}

	if (prop_info->type && !zend_verify_property_type(prop_info, value, 0)) {
		return;
	}

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
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	_class_string(&str, ce, &intern->obj, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ proto public string ReflectionClass::getName()
   Returns the class' name */
ZEND_METHOD(reflection_class, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_name(ZEND_THIS, return_value);
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
	if (ce->type == ZEND_USER_CLASS) {
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
	zend_string *name, *lc_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	lc_name = zend_string_tolower(name);
	RETVAL_BOOL(zend_hash_exists(&ce->function_table, lc_name) || is_closure_invoke(ce, lc_name));
	zend_string_release(lc_name);
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
	zend_string *name, *lc_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	lc_name = zend_string_tolower(name);
	if (!Z_ISUNDEF(intern->obj) && is_closure_invoke(ce, lc_name)
		&& (mptr = zend_get_closure_invoke_method(Z_OBJ(intern->obj))) != NULL)
	{
		/* don't assign closure_object since we only reflect the invoke handler
		   method and not the closure definition itself */
		reflection_method_factory(ce, mptr, NULL, return_value);
	} else if (Z_ISUNDEF(intern->obj) && is_closure_invoke(ce, lc_name)
		&& object_init_ex(&obj_tmp, ce) == SUCCESS && (mptr = zend_get_closure_invoke_method(Z_OBJ(obj_tmp))) != NULL) {
		/* don't assign closure_object since we only reflect the invoke handler
		   method and not the closure definition itself */
		reflection_method_factory(ce, mptr, NULL, return_value);
		zval_ptr_dtor(&obj_tmp);
	} else if ((mptr = zend_hash_find_ptr(&ce->function_table, lc_name)) != NULL) {
		reflection_method_factory(ce, mptr, NULL, return_value);
	} else {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Method %s does not exist", ZSTR_VAL(name));
	}
	zend_string_release(lc_name);
}
/* }}} */

/* {{{ _addmethod */
static void _addmethod(zend_function *mptr, zend_class_entry *ce, zval *retval, zend_long filter)
{
	if (mptr->common.fn_flags & filter) {
		zval method;
		reflection_method_factory(ce, mptr, NULL, &method);
		add_next_index_zval(retval, &method);
	}
}
/* }}} */

/* {{{ proto public ReflectionMethod[] ReflectionClass::getMethods([long $filter])
   Returns an array of this class' methods */
ZEND_METHOD(reflection_class, getMethods)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_function *mptr;
	zend_long filter = 0;
	zend_bool filter_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &filter, &filter_is_null) == FAILURE) {
		return;
	}

	if (filter_is_null) {
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	ZEND_HASH_FOREACH_PTR(&ce->function_table, mptr) {
		_addmethod(mptr, ce, return_value, filter);
	} ZEND_HASH_FOREACH_END();

	if (instanceof_function(ce, zend_ce_closure)) {
		zend_bool has_obj = Z_TYPE(intern->obj) != IS_UNDEF;
		zval obj_tmp;
		zend_object *obj;
		if (!has_obj) {
			object_init_ex(&obj_tmp, ce);
			obj = Z_OBJ(obj_tmp);
		} else {
			obj = Z_OBJ(intern->obj);
		}
		zend_function *closure = zend_get_closure_invoke_method(obj);
		if (closure) {
			_addmethod(closure, ce, return_value, filter);
		}
		if (!has_obj) {
			zval_ptr_dtor(&obj_tmp);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if ((property_info = zend_hash_find_ptr(&ce->properties_info, name)) != NULL) {
		if ((property_info->flags & ZEND_ACC_PRIVATE) && property_info->ce != ce) {
			RETURN_FALSE;
		}
		RETURN_TRUE;
	} else {
		if (Z_TYPE(intern->obj) != IS_UNDEF) {
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if ((property_info = zend_hash_find_ptr(&ce->properties_info, name)) != NULL) {
		if (!(property_info->flags & ZEND_ACC_PRIVATE) || property_info->ce == ce) {
			reflection_property_factory(ce, name, property_info, return_value, 0);
			return;
		}
	} else if (Z_TYPE(intern->obj) != IS_UNDEF) {
		/* Check for dynamic properties */
		if (zend_hash_exists(Z_OBJ_HT(intern->obj)->get_properties(&intern->obj), name)) {
			zend_property_info property_info_tmp;
			property_info_tmp.flags = ZEND_ACC_PUBLIC;
			property_info_tmp.name = name;
			property_info_tmp.doc_comment = NULL;
			property_info_tmp.ce = ce;
			property_info_tmp.type = 0;

			reflection_property_factory(ce, name, &property_info_tmp, return_value, 1);
			return;
		}
	}
	str_name = ZSTR_VAL(name);
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
			zend_string_release_ex(classname, 0);
			return;
		}
		zend_string_release_ex(classname, 0);

		if (!instanceof_function(ce, ce2)) {
			zend_throw_exception_ex(reflection_exception_ptr, -1, "Fully qualified property name %s::%s does not specify a base class of %s", ZSTR_VAL(ce2->name), str_name, ZSTR_VAL(ce->name));
			return;
		}
		ce = ce2;

		property_info = zend_hash_str_find_ptr(&ce->properties_info, str_name, str_name_len);
		if (property_info != NULL
		 && (!(property_info->flags & ZEND_ACC_PRIVATE)
		  || property_info->ce == ce)) {
			reflection_property_factory_str(ce, str_name, str_name_len, property_info, return_value);
			return;
		}
	}
	zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Property %s does not exist", str_name);
}
/* }}} */

/* {{{ _addproperty */
static void _addproperty(zend_property_info *pptr, zend_string *key, zend_class_entry *ce, zval *retval, long filter)
{
	if ((pptr->flags & ZEND_ACC_PRIVATE) && pptr->ce != ce) {
		return;
	}

	if (pptr->flags	& filter) {
		zval property;
		reflection_property_factory(ce, key, pptr, &property, 0);
		add_next_index_zval(retval, &property);
	}
}
/* }}} */

/* {{{ _adddynproperty */
static void _adddynproperty(zval *ptr, zend_string *key, zend_class_entry *ce, zval *retval)
{
	zend_property_info property_info;
	zval property;

	/* under some circumstances, the properties hash table may contain numeric
	 * properties (e.g. when casting from array). This is a WON'T FIX bug, at
	 * least for the moment. Ignore these */
	if (key == NULL) {
		return;
	}

	/* Not a dynamic property */
	if (Z_TYPE_P(ptr) == IS_INDIRECT) {
		return;
	}

	property_info.doc_comment = NULL;
	property_info.flags = ZEND_ACC_PUBLIC;
	property_info.name = key;
	property_info.ce = ce;
	property_info.offset = -1;
	property_info.type = 0;
	reflection_property_factory(ce, key, &property_info, &property, 1);
	add_next_index_zval(retval, &property);
}
/* }}} */

/* {{{ proto public ReflectionProperty[] ReflectionClass::getProperties([long $filter])
   Returns an array of this class' properties */
ZEND_METHOD(reflection_class, getProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *key;
	zend_property_info *prop_info;
	zend_long filter = 0;
	zend_bool filter_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &filter, &filter_is_null) == FAILURE) {
		return;
	}

	if (filter_is_null) {
		filter = ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->properties_info, key, prop_info) {
		_addproperty(prop_info, key, ce, return_value, filter);
	} ZEND_HASH_FOREACH_END();

	if (Z_TYPE(intern->obj) != IS_UNDEF && (filter & ZEND_ACC_PUBLIC) != 0) {
		HashTable *properties = Z_OBJ_HT(intern->obj)->get_properties(&intern->obj);
		zval *prop;
		ZEND_HASH_FOREACH_STR_KEY_VAL(properties, key, prop) {
			_adddynproperty(prop, key, ce, return_value);
		} ZEND_HASH_FOREACH_END();
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
	zval val;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, key, c) {
		if (UNEXPECTED(zval_update_constant_ex(&c->value, ce) != SUCCESS)) {
			zend_array_destroy(Z_ARRVAL_P(return_value));
			RETURN_NULL();
		}
		ZVAL_COPY_OR_DUP(&val, &c->value);
		zend_hash_add_new(Z_ARRVAL_P(return_value), key, &val);
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
	ZVAL_COPY_OR_DUP(return_value, &c->value);
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
			/* We're not calling the constructor, so don't call the destructor either. */
			zend_object_store_ctor_failed(Z_OBJ(obj));
			RETVAL_BOOL(Z_OBJ_HANDLER(obj, clone_obj) != NULL);
			zval_ptr_dtor(&obj);
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
	uint32_t keep_flags = ZEND_ACC_FINAL
		| ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_LONG((ce->ce_flags & keep_flags));
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isInstance(stdclass object)
   Returns whether the given object is an instance of this class */
ZEND_METHOD(reflection_class, isInstance)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval *object;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &object) == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(instanceof_function(Z_OBJCE_P(object), ce));
}
/* }}} */

/* {{{ proto public object ReflectionClass::newInstance(mixed* args, ...)
   Returns an instance of this class */
ZEND_METHOD(reflection_class, newInstance)
{
	zval retval;
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	zend_function *constructor;

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
			zval_ptr_dtor(return_value);
			RETURN_NULL();
		}

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &params, &num_args) == FAILURE) {
			zval_ptr_dtor(return_value);
			RETURN_FALSE;
		}

		for (i = 0; i < num_args; i++) {
			Z_TRY_ADDREF(params[i]);
		}

		fci.size = sizeof(fci);
		ZVAL_UNDEF(&fci.function_name);
		fci.object = Z_OBJ_P(return_value);
		fci.retval = &retval;
		fci.param_count = num_args;
		fci.params = params;
		fci.no_separation = 1;

		fcc.function_handler = constructor;
		fcc.called_scope = Z_OBJCE_P(return_value);
		fcc.object = Z_OBJ_P(return_value);

		ret = zend_call_function(&fci, &fcc);
		zval_ptr_dtor(&retval);
		for (i = 0; i < num_args; i++) {
			zval_ptr_dtor(&params[i]);
		}

		if (EG(exception)) {
			zend_object_store_ctor_failed(Z_OBJ_P(return_value));
		}
		if (ret == FAILURE) {
			php_error_docref(NULL, E_WARNING, "Invocation of %s's constructor failed", ZSTR_VAL(ce->name));
			zval_ptr_dtor(return_value);
			RETURN_NULL();
		}
	} else if (ZEND_NUM_ARGS()) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s does not have a constructor, so you cannot pass any constructor arguments", ZSTR_VAL(ce->name));
	}
}
/* }}} */

/* {{{ proto public object ReflectionClass::newInstanceWithoutConstructor()
   Returns an instance of this class without invoking its constructor */
ZEND_METHOD(reflection_class, newInstanceWithoutConstructor)
{
	reflection_object *intern;
	zend_class_entry *ce;

	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->type == ZEND_INTERNAL_CLASS
			&& ce->create_object != NULL && (ce->ce_flags & ZEND_ACC_FINAL)) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s is an internal class marked as final that cannot be instantiated without invoking its constructor", ZSTR_VAL(ce->name));
		return;
	}

	object_init_ex(return_value, ce);
}
/* }}} */

/* {{{ proto public object ReflectionClass::newInstanceArgs([array args])
   Returns an instance of this class */
ZEND_METHOD(reflection_class, newInstanceArgs)
{
	zval retval, *val;
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	int ret, i, argc = 0;
	HashTable *args;
	zend_function *constructor;

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
			zval_ptr_dtor(return_value);
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

		fcc.function_handler = constructor;
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

		if (EG(exception)) {
			zend_object_store_ctor_failed(Z_OBJ_P(return_value));
		}
		if (ret == FAILURE) {
			zval_ptr_dtor(&retval);
			php_error_docref(NULL, E_WARNING, "Invocation of %s's constructor failed", ZSTR_VAL(ce->name));
			zval_ptr_dtor(return_value);
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

	if (ce->num_interfaces) {
		uint32_t i;

		ZEND_ASSERT(ce->ce_flags & ZEND_ACC_LINKED);
		array_init(return_value);
		for (i=0; i < ce->num_interfaces; i++) {
			zval interface;
			zend_reflection_class_factory(ce->interfaces[i], &interface);
			zend_hash_update(Z_ARRVAL_P(return_value), ce->interfaces[i]->name, &interface);
		}
	} else {
		RETURN_EMPTY_ARRAY();
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

	if (!ce->num_interfaces) {
		/* Return an empty array if this class implements no interfaces */
		RETURN_EMPTY_ARRAY();
	}

	ZEND_ASSERT(ce->ce_flags & ZEND_ACC_LINKED);
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

	if (!ce->num_traits) {
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);

	for (i=0; i < ce->num_traits; i++) {
		zval trait;
		zend_class_entry *trait_ce;

		trait_ce = zend_fetch_class_by_name(ce->trait_names[i].name,
			ce->trait_names[i].lc_name, ZEND_FETCH_CLASS_TRAIT);
		ZEND_ASSERT(trait_ce);
		zend_reflection_class_factory(trait_ce, &trait);
		zend_hash_update(Z_ARRVAL_P(return_value), ce->trait_names[i].name, &trait);
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

	if (!ce->num_traits) {
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);

	for (i=0; i < ce->num_traits; i++) {
		add_next_index_str(return_value, zend_string_copy(ce->trait_names[i].name));
	}
}
/* }}} */

/* {{{ proto public array ReflectionClass::getTraitAliases()
   Returns an array of trait aliases */
ZEND_METHOD(reflection_class, getTraitAliases)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ce);


	if (ce->trait_aliases) {
		uint32_t i = 0;

		array_init(return_value);
		while (ce->trait_aliases[i]) {
			zend_string *mname;
			zend_trait_method_reference *cur_ref = &ce->trait_aliases[i]->trait_method;

			if (ce->trait_aliases[i]->alias) {

				mname = zend_string_alloc(ZSTR_LEN(cur_ref->class_name) + ZSTR_LEN(cur_ref->method_name) + 2, 0);
				snprintf(ZSTR_VAL(mname), ZSTR_LEN(mname) + 1, "%s::%s", ZSTR_VAL(cur_ref->class_name), ZSTR_VAL(cur_ref->method_name));
				add_assoc_str_ex(return_value, ZSTR_VAL(ce->trait_aliases[i]->alias), ZSTR_LEN(ce->trait_aliases[i]->alias), mname);
			}
			i++;
		}
	} else {
		RETURN_EMPTY_ARRAY();
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
					zend_throw_error(NULL, "Internal error: Failed to retrieve the argument's reflection object");
					return;
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
					zend_throw_error(NULL, "Internal error: Failed to retrieve the argument's reflection object");
					return;
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
				"%s is not an interface", ZSTR_VAL(interface_ce->name));
		return;
	}
	RETURN_BOOL(instanceof_function(ce, interface_ce));
}
/* }}} */

/* {{{ proto public bool ReflectionClass::isIterable()
   Returns whether this class is iterable (can be used inside foreach) */
ZEND_METHOD(reflection_class, isIterable)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_IMPLICIT_ABSTRACT_CLASS |
	                    ZEND_ACC_TRAIT     | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		RETURN_FALSE;
	}

	RETURN_BOOL(ce->get_iterator || instanceof_function(ce, zend_ce_traversable));
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
	if ((name = _default_load_name(ZEND_THIS)) == NULL) {
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
	if ((name = _default_load_name(ZEND_THIS)) == NULL) {
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
	if ((name = _default_load_name(ZEND_THIS)) == NULL) {
		RETURN_FALSE;
	}
	if (Z_TYPE_P(name) == IS_STRING
		&& (backslash = zend_memrchr(Z_STRVAL_P(name), '\\', Z_STRLEN_P(name)))
		&& backslash > Z_STRVAL_P(name))
	{
		RETURN_STRINGL(backslash + 1, Z_STRLEN_P(name) - (backslash - Z_STRVAL_P(name) + 1));
	}
	ZVAL_COPY_DEREF(return_value, name);
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
	zval *classname;
	zend_string *name;
	int dynam_prop = 0;
	zval *object;
	reflection_object *intern;
	zend_class_entry *ce;
	zend_property_info *property_info = NULL;
	property_reference *reference;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zS", &classname, &name) == FAILURE) {
		return;
	}

	object = ZEND_THIS;
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
			return;
	}

	property_info = zend_hash_find_ptr(&ce->properties_info, name);
	if (property_info == NULL
	 || ((property_info->flags & ZEND_ACC_PRIVATE)
	  && property_info->ce != ce)) {
		/* Check for dynamic properties */
		if (property_info == NULL && Z_TYPE_P(classname) == IS_OBJECT) {
			if (zend_hash_exists(Z_OBJ_HT_P(classname)->get_properties(classname), name)) {
				dynam_prop = 1;
			}
		}
		if (dynam_prop == 0) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Property %s::$%s does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(name));
			return;
		}
	}

	if (dynam_prop == 0 && (property_info->flags & ZEND_ACC_PRIVATE) == 0) {
		/* we have to search the class hierarchy for this (implicit) public or protected property */
		zend_class_entry *tmp_ce = ce;
		zend_property_info *tmp_info;

		while (tmp_ce && (tmp_info = zend_hash_find_ptr(&tmp_ce->properties_info, name)) == NULL) {
			ce = tmp_ce;
			property_info = tmp_info;
			tmp_ce = tmp_ce->parent;
		}
	}

	ZVAL_STR_COPY(reflection_prop_name(object), name);
	if (dynam_prop == 0) {
		ZVAL_STR_COPY(reflection_prop_class(object), property_info->ce->name);
	} else {
		ZVAL_STR_COPY(reflection_prop_class(object), ce->name);
	}

	reference = (property_reference*) emalloc(sizeof(property_reference));
	if (dynam_prop) {
		reference->prop.flags = ZEND_ACC_PUBLIC;
		reference->prop.name = name;
		reference->prop.doc_comment = NULL;
		reference->prop.ce = ce;
		reference->prop.type = 0;
		reference->dynamic = 1;
	} else {
		reference->prop = *property_info;
		reference->dynamic = 0;

		if (ZEND_TYPE_IS_NAME(reference->prop.type)) {
			zend_string_addref(ZEND_TYPE_NAME(reference->prop.type));
		}
	}
	reference->unmangled_name = zend_string_copy(name);
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
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	_property_string(&str, &ref->prop, ZSTR_VAL(ref->unmangled_name), "", ref->dynamic);
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ proto public string ReflectionProperty::getName()
   Returns the class' name */
ZEND_METHOD(reflection_property, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_name(ZEND_THIS, return_value);
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
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
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
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(!ref->dynamic);
}
/* }}} */

/* {{{ proto public int ReflectionProperty::getModifiers()
   Returns a bitfield of the access modifiers for this property */
ZEND_METHOD(reflection_property, getModifiers)
{
	reflection_object *intern;
	property_reference *ref;
	uint32_t keep_flags = ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	RETURN_LONG((ref->prop.flags & keep_flags));
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

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(ref->prop.flags & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		name = _default_load_name(ZEND_THIS);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public member %s::$%s", ZSTR_VAL(intern->ce->name), Z_STRVAL_P(name));
		return;
	}

	if (ref->prop.flags & ZEND_ACC_STATIC) {
		member_p = zend_read_static_property_ex(intern->ce, ref->unmangled_name, 0);
		if (member_p) {
			ZVAL_COPY_DEREF(return_value, member_p);
		}
	} else {
		zval rv;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &object) == FAILURE) {
			return;
		}

		if (!instanceof_function(Z_OBJCE_P(object), ref->prop.ce)) {
			_DO_THROW("Given object is not an instance of the class this property was declared in");
			return;
		}

		member_p = zend_read_property_ex(intern->ce, object, ref->unmangled_name, 0, &rv);
		if (member_p != &rv) {
			ZVAL_COPY_DEREF(return_value, member_p);
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
	zval *object, *name;
	zval *value;
	zval *tmp;

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(ref->prop.flags & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		name = _default_load_name(ZEND_THIS);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public member %s::$%s", ZSTR_VAL(intern->ce->name), Z_STRVAL_P(name));
		return;
	}

	if (ref->prop.flags & ZEND_ACC_STATIC) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &tmp, &value) == FAILURE) {
				return;
			}
		}

		zend_update_static_property_ex(intern->ce, ref->unmangled_name, value);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "oz", &object, &value) == FAILURE) {
			return;
		}

		zend_update_property_ex(intern->ce, object, ref->unmangled_name, value);
	}
}
/* }}} */

/* {{{ proto public mixed ReflectionProperty::isInitialized([stdclass object])
   Returns this property's value */
ZEND_METHOD(reflection_property, isInitialized)
{
	reflection_object *intern;
	property_reference *ref;
	zval *object, *name;
	zval *member_p = NULL;

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(ref->prop.flags & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		name = _default_load_name(getThis());
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public member %s::$%s", ZSTR_VAL(intern->ce->name), Z_STRVAL_P(name));
		return;
	}

	if (ref->prop.flags & ZEND_ACC_STATIC) {
		member_p = zend_read_static_property_ex(intern->ce, ref->unmangled_name, 1);
		if (member_p) {
			RETURN_BOOL(!Z_ISUNDEF_P(member_p))
		}
		RETURN_FALSE;
	} else {
		zval name_zv;
		zend_class_entry *old_scope;
		int retval;

		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &object) == FAILURE) {
			return;
		}

		if (!instanceof_function(Z_OBJCE_P(object), ref->prop.ce)) {
			_DO_THROW("Given object is not an instance of the class this property was declared in");
			return;
		}

		old_scope = EG(fake_scope);
		EG(fake_scope) = intern->ce;
		ZVAL_STR(&name_zv, ref->unmangled_name);
		retval = Z_OBJ_HT_P(object)->has_property(object, &name_zv, ZEND_PROPERTY_EXISTS, NULL);
		EG(fake_scope) = old_scope;

		RETVAL_BOOL(retval);
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

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	ce = tmp_ce = intern->ce;
	while (tmp_ce && (tmp_info = zend_hash_find_ptr(&tmp_ce->properties_info, ref->unmangled_name)) != NULL) {
		if (tmp_info->flags & ZEND_ACC_PRIVATE) {
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

	intern = Z_REFLECTION_P(ZEND_THIS);

	intern->ignore_visibility = visible;
}
/* }}} */

/* {{{ proto public ReflectionType ReflectionProperty::getType()
   Returns the type associated with the property */
ZEND_METHOD(reflection_property, getType)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!ZEND_TYPE_IS_SET(ref->prop.type)) {
		RETURN_NULL();
	}

	reflection_type_factory(ref->prop.type, return_value);
}
/* }}} */

/* {{{ proto public bool ReflectionProperty::hasType()
   Returns whether property has a type */
ZEND_METHOD(reflection_property, hasType)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	RETVAL_BOOL(ZEND_TYPE_IS_SET(ref->prop.type));
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

	object = ZEND_THIS;
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
	ZVAL_STRING(reflection_prop_name(object), module->name);
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
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);
	_extension_string(&str, module, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ proto public string ReflectionExtension::getName()
   Returns this extension's name */
ZEND_METHOD(reflection_extension, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	_default_get_name(ZEND_THIS, return_value);
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

/* {{{ proto public array ReflectionExtension::getConstants()
   Returns an associative array containing this extension's constants and their values */
ZEND_METHOD(reflection_extension, getConstants)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_constant *constant;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	ZEND_HASH_FOREACH_PTR(EG(zend_constants), constant) {
		if (module->module_number == ZEND_CONSTANT_MODULE_NUMBER(constant)) {
			zval const_val;
			ZVAL_COPY_OR_DUP(&const_val, &constant->value);
			zend_hash_update(Z_ARRVAL_P(return_value), constant->name, &const_val);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ _addinientry */
static void _addinientry(zend_ini_entry *ini_entry, zval *retval, int number)
{
	if (number == ini_entry->module_number) {
		zval zv;
		if (ini_entry->value) {
			ZVAL_STR_COPY(&zv, ini_entry->value);
		} else {
			ZVAL_NULL(&zv);
		}
		zend_symtable_update(Z_ARRVAL_P(retval), ini_entry->name, &zv);
	}
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getINIEntries()
   Returns an associative array containing this extension's INI entries and their values */
ZEND_METHOD(reflection_extension, getINIEntries)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_ini_entry *ini_entry;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	ZEND_HASH_FOREACH_PTR(EG(ini_directives), ini_entry) {
		_addinientry(ini_entry, return_value, module->module_number);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ add_extension_class */
static void add_extension_class(zend_class_entry *ce, zend_string *key, zval *class_array, zend_module_entry *module, zend_bool add_reflection_class)
{
	if (ce->type == ZEND_INTERNAL_CLASS && ce->info.internal.module && !strcasecmp(ce->info.internal.module->name, module->name)) {
		zend_string *name;

		if (!zend_string_equals_ci(ce->name, key)) {
			/* This is a class alias, use alias name */
			name = key;
		} else {
			/* Use class name */
			name = ce->name;
		}
		if (add_reflection_class) {
			zval zclass;
			zend_reflection_class_factory(ce, &zclass);
			zend_hash_update(Z_ARRVAL_P(class_array), name, &zclass);
		} else {
			add_next_index_str(class_array, zend_string_copy(name));
		}
	}
}
/* }}} */

/* {{{ proto public ReflectionClass[] ReflectionExtension::getClasses()
   Returns an array containing ReflectionClass objects for all classes of this extension */
ZEND_METHOD(reflection_extension, getClasses)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_string *key;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(EG(class_table), key, ce) {
		add_extension_class(ce, key, return_value, module, 1);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ proto public array ReflectionExtension::getClassNames()
   Returns an array containing all names of all classes of this extension */
ZEND_METHOD(reflection_extension, getClassNames)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_string *key;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(EG(class_table), key, ce) {
		add_extension_class(ce, key, return_value, module, 0);
	} ZEND_HASH_FOREACH_END();
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

	dep = module->deps;

	if (!dep)
	{
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);
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
	zval *object;
	reflection_object *intern;
	zend_extension *extension;
	char *name_str;
	size_t name_len;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "s", &name_str, &name_len) == FAILURE) {
		return;
	}

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	extension = zend_get_extension(name_str);
	if (!extension) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Zend Extension %s does not exist", name_str);
		return;
	}
	ZVAL_STRING(reflection_prop_name(object), extension->name);
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
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	GET_REFLECTION_OBJECT_PTR(extension);
	_zend_extension_string(&str, extension, "");
	RETURN_STR(smart_str_extract(&str));
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

/* {{{ proto public ReflectionReference::__construct()
 *     Dummy constructor -- always throws ReflectionExceptions. */
ZEND_METHOD(reflection_reference, __construct)
{
	_DO_THROW(
		"Cannot directly instantiate ReflectionReference. "
		"Use ReflectionReference::fromArrayElement() instead"
	);
}
/* }}} */

static zend_bool is_ignorable_reference(HashTable *ht, zval *ref) {
	if (Z_REFCOUNT_P(ref) != 1) {
		return 0;
	}

	/* Directly self-referential arrays are treated as proper references
	 * in zend_array_dup() despite rc=1. */
	return Z_TYPE_P(Z_REFVAL_P(ref)) != IS_ARRAY || Z_ARRVAL_P(Z_REFVAL_P(ref)) != ht;
}

/* {{{ proto public ReflectionReference|null ReflectionReference::fromArrayElement(array array, mixed key)
 *     Create ReflectionReference for array item. Returns null if not a reference. */
ZEND_METHOD(reflection_reference, fromArrayElement)
{
	HashTable *ht;
	zval *key, *item;
	reflection_object *intern;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "hz", &ht, &key) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(key) == IS_LONG) {
		item = zend_hash_index_find(ht, Z_LVAL_P(key));
	} else if (Z_TYPE_P(key) == IS_STRING) {
		item = zend_symtable_find(ht, Z_STR_P(key));
	} else {
		zend_type_error("Key must be array or string");
		return;
	}

	if (!item) {
		_DO_THROW("Array key not found");
		return;
	}

	if (Z_TYPE_P(item) != IS_REFERENCE || is_ignorable_reference(ht, item)) {
		RETURN_NULL();
	}

	object_init_ex(return_value, reflection_reference_ptr);
	intern = Z_REFLECTION_P(return_value);
	ZVAL_COPY(&intern->obj, item);
	intern->ref_type = REF_TYPE_OTHER;
}
/* }}} */

/* {{{ proto public int|string ReflectionReference::getId()
 *     Returns a unique identifier for the reference.
 *     The format of the return value is unspecified and may change. */
ZEND_METHOD(reflection_reference, getId)
{
	reflection_object *intern;
	unsigned char digest[20];
	PHP_SHA1_CTX context;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = Z_REFLECTION_P(getThis());
	if (Z_TYPE(intern->obj) != IS_REFERENCE) {
		_DO_THROW("Corrupted ReflectionReference object");
		return;
	}

	if (!REFLECTION_G(key_initialized)) {
		if (php_random_bytes_throw(&REFLECTION_G(key_initialized), 16) == FAILURE) {
			return;
		}

		REFLECTION_G(key_initialized) = 1;
	}

	/* SHA1(ref || key) to avoid directly exposing memory addresses. */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, (unsigned char *) &Z_REF(intern->obj), sizeof(zend_reference *));
	PHP_SHA1Update(&context, REFLECTION_G(key), REFLECTION_KEY_LEN);
	PHP_SHA1Final(digest, &context);

	RETURN_STRINGL((char *) digest, sizeof(digest));
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
	ZEND_DEP_ME(reflection, export, arginfo_reflection_export, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_FE_END
};

static const zend_function_entry reflector_functions[] = {
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
	ZEND_DEP_ME(reflection_function, export, arginfo_reflection_function_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_method_getClosure, 0, 0, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_method_functions[] = {
	ZEND_DEP_ME(reflection_method, export, arginfo_reflection_method_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
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
	ZEND_DEP_ME(reflection_class, export, arginfo_reflection_class_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
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
	ZEND_ME(reflection_class, isIterable, arginfo_reflection__void, 0)
	ZEND_MALIAS(reflection_class, isIterateable, isIterable, arginfo_reflection__void, 0)
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
	ZEND_DEP_ME(reflection_object, export, arginfo_reflection_object_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_property_isInitialized, 0, 0, 0)
	ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_reflection_property_setAccessible, 0)
	ZEND_ARG_INFO(0, visible)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_property_functions[] = {
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE|ZEND_ACC_FINAL)
	ZEND_DEP_ME(reflection_property, export, arginfo_reflection_property_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_property, __construct, arginfo_reflection_property___construct, 0)
	ZEND_ME(reflection_property, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getValue, arginfo_reflection_property_getValue, 0)
	ZEND_ME(reflection_property, setValue, arginfo_reflection_property_setValue, 0)
	ZEND_ME(reflection_property, isInitialized, arginfo_reflection_property_isInitialized, 0)
	ZEND_ME(reflection_property, isPublic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isPrivate, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isProtected, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isStatic, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, isDefault, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getModifiers, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getDeclaringClass, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, getDocComment, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, setAccessible, arginfo_reflection_property_setAccessible, 0)
	ZEND_ME(reflection_property, getType, arginfo_reflection__void, 0)
	ZEND_ME(reflection_property, hasType, arginfo_reflection__void, 0)
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
	ZEND_DEP_ME(reflection_class_constant, export, arginfo_reflection_class_constant_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
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
	ZEND_DEP_ME(reflection_parameter, export, arginfo_reflection_parameter_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
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
	ZEND_ME(reflection_type, __toString, arginfo_reflection__void, ZEND_ACC_DEPRECATED)
	PHP_FE_END
};

static const zend_function_entry reflection_named_type_functions[] = {
	ZEND_ME(reflection_named_type, getName, arginfo_reflection__void, 0)
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
	ZEND_DEP_ME(reflection_extension, export, arginfo_reflection_extension_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
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
	ZEND_DEP_ME(reflection_zend_extension, export, arginfo_reflection_extension_export, ZEND_ACC_STATIC|ZEND_ACC_PUBLIC)
	ZEND_ME(reflection_zend_extension, __construct, arginfo_reflection_zend_extension___construct, 0)
	ZEND_ME(reflection_zend_extension, __toString, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getName, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getVersion, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getAuthor, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getURL, arginfo_reflection__void, 0)
	ZEND_ME(reflection_zend_extension, getCopyright, arginfo_reflection__void, 0)
	PHP_FE_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_reflection_reference_fromArrayElement, 0, 0, 2)
	ZEND_ARG_INFO(0, array)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

static const zend_function_entry reflection_reference_functions[] = {
	ZEND_ME(reflection_reference, fromArrayElement, arginfo_reflection_reference_fromArrayElement, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(reflection_reference, getId, arginfo_reflection__void, ZEND_ACC_PUBLIC)

	/* Always throwing dummy methods */
	ZEND_ME(reflection, __clone, arginfo_reflection__void, ZEND_ACC_PRIVATE)
	ZEND_ME(reflection_reference, __construct, arginfo_reflection__void, ZEND_ACC_PRIVATE)
	PHP_FE_END
};
/* }}} */

static const zend_function_entry reflection_ext_functions[] = { /* {{{ */
	PHP_FE_END
}; /* }}} */

/* {{{ _reflection_write_property */
static zval *_reflection_write_property(zval *object, zval *member, zval *value, void **cache_slot)
{
	if ((Z_TYPE_P(member) == IS_STRING)
		&& zend_hash_exists(&Z_OBJCE_P(object)->properties_info, Z_STR_P(member))
		&& ((Z_STRLEN_P(member) == sizeof("name") - 1  && !memcmp(Z_STRVAL_P(member), "name",  sizeof("name")))
			|| (Z_STRLEN_P(member) == sizeof("class") - 1 && !memcmp(Z_STRVAL_P(member), "class", sizeof("class")))))
	{
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot set read-only property %s::$%s", ZSTR_VAL(Z_OBJCE_P(object)->name), Z_STRVAL_P(member));
		return &EG(uninitialized_zval);
	}
	else
	{
		return zend_std_write_property(object, member, value, cache_slot);
	}
}
/* }}} */

static void reflection_init_class_handlers(zend_class_entry *ce) {
	ce->create_object = reflection_objects_new;
	ce->serialize = zend_class_serialize_deny;
	ce->unserialize = zend_class_unserialize_deny;
}

PHP_MINIT_FUNCTION(reflection) /* {{{ */
{
	zend_class_entry _reflection_entry;

	memcpy(&reflection_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	reflection_object_handlers.offset = XtOffsetOf(reflection_object, zo);
	reflection_object_handlers.free_obj = reflection_free_objects_storage;
	reflection_object_handlers.clone_obj = NULL;
	reflection_object_handlers.write_property = _reflection_write_property;
	reflection_object_handlers.get_gc = reflection_get_gc;

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionException", reflection_exception_functions);
	reflection_exception_ptr = zend_register_internal_class_ex(&_reflection_entry, zend_ce_exception);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflection", reflection_functions);
	reflection_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflector", reflector_functions);
	reflector_ptr = zend_register_internal_interface(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionFunctionAbstract", reflection_function_abstract_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_function_abstract_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_function_abstract_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_function_abstract_ptr, "name", sizeof("name")-1, "", ZEND_ACC_ABSTRACT);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionFunction", reflection_function_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_function_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_function_abstract_ptr);
	zend_declare_property_string(reflection_function_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(function, "IS_DEPRECATED", ZEND_ACC_DEPRECATED);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionGenerator", reflection_generator_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_generator_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionParameter", reflection_parameter_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_parameter_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_parameter_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_parameter_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionType", reflection_type_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_type_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionNamedType", reflection_named_type_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_named_type_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_type_ptr);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionMethod", reflection_method_functions);
	reflection_init_class_handlers(&_reflection_entry);
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
	reflection_init_class_handlers(&_reflection_entry);
	reflection_class_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_class_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_class_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	/* IS_IMPLICIT_ABSTRACT is not longer used */
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_IMPLICIT_ABSTRACT", ZEND_ACC_IMPLICIT_ABSTRACT_CLASS);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_EXPLICIT_ABSTRACT", ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_FINAL", ZEND_ACC_FINAL);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionObject", reflection_object_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_object_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_class_ptr);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionProperty", reflection_property_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_property_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_property_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_property_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(reflection_property_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionClassConstant", reflection_class_constant_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_class_constant_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_class_constant_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_class_constant_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(reflection_class_constant_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_STATIC", ZEND_ACC_STATIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PUBLIC", ZEND_ACC_PUBLIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PROTECTED", ZEND_ACC_PROTECTED);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PRIVATE", ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionExtension", reflection_extension_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_extension_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_extension_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_extension_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionZendExtension", reflection_zend_extension_functions);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_zend_extension_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_zend_extension_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_zend_extension_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionReference", reflection_reference_functions);
	reflection_init_class_handlers(&_reflection_entry);
	_reflection_entry.ce_flags |= ZEND_ACC_FINAL;
	reflection_reference_ptr = zend_register_internal_class(&_reflection_entry);

	REFLECTION_G(key_initialized) = 0;

	return SUCCESS;
} /* }}} */

PHP_MINFO_FUNCTION(reflection) /* {{{ */
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Reflection", "enabled");
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
	ZEND_MODULE_GLOBALS(reflection),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
}; /* }}} */
