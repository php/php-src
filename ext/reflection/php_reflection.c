/*
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
#include "zend_ast.h"
#include "zend_attributes.h"
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
#include "php_reflection_arginfo.h"

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
PHPAPI zend_class_entry *reflection_union_type_ptr;
PHPAPI zend_class_entry *reflection_class_ptr;
PHPAPI zend_class_entry *reflection_object_ptr;
PHPAPI zend_class_entry *reflection_method_ptr;
PHPAPI zend_class_entry *reflection_property_ptr;
PHPAPI zend_class_entry *reflection_class_constant_ptr;
PHPAPI zend_class_entry *reflection_extension_ptr;
PHPAPI zend_class_entry *reflection_zend_extension_ptr;
PHPAPI zend_class_entry *reflection_reference_ptr;
PHPAPI zend_class_entry *reflection_attribute_ptr;

/* Exception throwing macro */
#define _DO_THROW(msg) \
	zend_throw_exception(reflection_exception_ptr, msg, 0);

#define GET_REFLECTION_OBJECT() do { \
	intern = Z_REFLECTION_P(ZEND_THIS); \
	if (intern->ptr == NULL) { \
		if (EG(exception) && EG(exception)->ce == reflection_exception_ptr) { \
			RETURN_THROWS(); \
		} \
		zend_throw_error(NULL, "Internal error: Failed to retrieve the reflection object"); \
		RETURN_THROWS(); \
	} \
} while (0)

#define GET_REFLECTION_OBJECT_PTR(target) do { \
	GET_REFLECTION_OBJECT(); \
	target = intern->ptr; \
} while (0)

/* Class constants */
#define REGISTER_REFLECTION_CLASS_CONST_LONG(class_name, const_name, value) \
	zend_declare_class_constant_long(reflection_ ## class_name ## _ptr, const_name, sizeof(const_name)-1, (zend_long)value);

#define REFLECTION_ATTRIBUTE_IS_INSTANCEOF (1 << 1)

/* {{{ Object structure */

/* Struct for properties */
typedef struct _property_reference {
	zend_property_info *prop;
	zend_string *unmangled_name;
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
	/* Whether to use backwards compatible null representation */
	zend_bool legacy_behavior;
} type_reference;

/* Struct for attributes */
typedef struct _attribute_reference {
	HashTable *attributes;
	zend_attribute *data;
	zend_class_entry *scope;
	zend_string *filename;
	uint32_t target;
} attribute_reference;

typedef enum {
	REF_TYPE_OTHER,      /* Must be 0 */
	REF_TYPE_FUNCTION,
	REF_TYPE_GENERATOR,
	REF_TYPE_PARAMETER,
	REF_TYPE_TYPE,
	REF_TYPE_PROPERTY,
	REF_TYPE_CLASS_CONSTANT,
	REF_TYPE_ATTRIBUTE
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

static zend_always_inline uint32_t prop_get_flags(property_reference *ref) {
	return ref->prop ? ref->prop->flags : ZEND_ACC_PUBLIC;
}

static inline zend_bool is_closure_invoke(zend_class_entry *ce, zend_string *lcname) {
	return ce == zend_ce_closure
		&& zend_string_equals_literal(lcname, ZEND_INVOKE_FUNC_NAME);
}

static void _default_get_name(zval *object, zval *return_value) /* {{{ */
{
	zval *name = reflection_prop_name(object);
	if (Z_ISUNDEF_P(name)) {
		RETURN_FALSE;
	}
	ZVAL_COPY(return_value, name);
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
			if (ZEND_TYPE_HAS_NAME(type_ref->type)) {
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
			efree(intern->ptr);
			break;
		case REF_TYPE_ATTRIBUTE: {
			attribute_reference *attr_ref = intern->ptr;
			if (attr_ref->filename) {
				zend_string_release(attr_ref->filename);
			}
			efree(intern->ptr);
			break;
		}
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

static HashTable *reflection_get_gc(zend_object *obj, zval **gc_data, int *gc_data_count) /* {{{ */
{
	reflection_object *intern = reflection_object_from_obj(obj);
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
static void _property_string(smart_str *str, zend_property_info *prop, const char *prop_name, char* indent);
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
				_property_string(str, prop, NULL, ZSTR_VAL(sub_indent));
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
			if ((mptr->common.fn_flags & ZEND_ACC_STATIC)
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
			if ((mptr->common.fn_flags & ZEND_ACC_STATIC)
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
				_property_string(str, prop, NULL, ZSTR_VAL(sub_indent));
			}
		} ZEND_HASH_FOREACH_END();
	}
	smart_str_append_printf(str, "%s  }\n", indent);

	if (obj && Z_TYPE_P(obj) == IS_OBJECT) {
		HashTable    *properties = Z_OBJ_HT_P(obj)->get_properties(Z_OBJ_P(obj));
		zend_string  *prop_name;
		smart_str prop_str = {0};

		count = 0;
		if (properties && zend_hash_num_elements(properties)) {
			ZEND_HASH_FOREACH_STR_KEY(properties, prop_name) {
				if (prop_name && ZSTR_LEN(prop_name) && ZSTR_VAL(prop_name)[0]) { /* skip all private and protected properties */
					if (!zend_hash_exists(&ce->properties_info, prop_name)) {
						count++;
						_property_string(&prop_str, NULL, ZSTR_VAL(prop_name), ZSTR_VAL(sub_indent));
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
	const char *type = zend_zval_type_name(value);

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
	const char *type;

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

static zend_op *get_recv_op(zend_op_array *op_array, uint32_t offset)
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
	ZEND_ASSERT(0 && "Failed to find op");
	return NULL;
}

static zval *get_default_from_recv(zend_op_array *op_array, uint32_t offset) {
	zend_op *recv = get_recv_op(op_array, offset);
	if (!recv || recv->opcode != ZEND_RECV_INIT) {
		return NULL;
	}

	return RT_CONSTANT(recv, recv->op2);
}

static int format_default_value(smart_str *str, zval *value, zend_class_entry *scope) {
	zval zv;
	ZVAL_COPY(&zv, value);
	if (UNEXPECTED(zval_update_constant_ex(&zv, scope) == FAILURE)) {
		zval_ptr_dtor(&zv);
		return FAILURE;
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
	return SUCCESS;
}

static inline zend_bool has_internal_arg_info(const zend_function *fptr) {
	return fptr->type == ZEND_INTERNAL_FUNCTION
		&& !(fptr->common.fn_flags & ZEND_ACC_USER_ARG_INFO);
}

/* {{{ _parameter_string */
static void _parameter_string(smart_str *str, zend_function *fptr, struct _zend_arg_info *arg_info, uint32_t offset, zend_bool required, char* indent)
{
	smart_str_append_printf(str, "Parameter #%d [ ", offset);
	if (!required) {
		smart_str_append_printf(str, "<optional> ");
	} else {
		smart_str_append_printf(str, "<required> ");
	}
	if (ZEND_TYPE_IS_SET(arg_info->type)) {
		zend_string *type_str = zend_type_to_string(arg_info->type);
		smart_str_append(str, type_str);
		smart_str_appendc(str, ' ');
		zend_string_release(type_str);
	}
	if (ZEND_ARG_SEND_MODE(arg_info)) {
		smart_str_appendc(str, '&');
	}
	if (ZEND_ARG_IS_VARIADIC(arg_info)) {
		smart_str_appends(str, "...");
	}
	smart_str_append_printf(str, "$%s", has_internal_arg_info(fptr)
		? ((zend_internal_arg_info*)arg_info)->name : ZSTR_VAL(arg_info->name));

	if (!required && !ZEND_ARG_IS_VARIADIC(arg_info)) {
		if (fptr->type == ZEND_INTERNAL_FUNCTION) {
			smart_str_appends(str, " = ");
			/* TODO: We don't have a way to fetch the default value for an internal function
			 * with userland arg info. */
			if (has_internal_arg_info(fptr)
					&& ((zend_internal_arg_info*)arg_info)->default_value) {
				smart_str_appends(str, ((zend_internal_arg_info*)arg_info)->default_value);
			} else {
				smart_str_appends(str, "<default>");
			}
		} else {
			zval *default_value = get_default_from_recv((zend_op_array*)fptr, offset);
			if (default_value) {
				smart_str_appends(str, " = ");
				if (format_default_value(str, default_value, fptr->common.scope) == FAILURE) {
					return;
				}
			}
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
		if (ZEND_TYPE_IS_SET(fptr->common.arg_info[-1].type)) {
			zend_string *type_str = zend_type_to_string(fptr->common.arg_info[-1].type);
			smart_str_append_printf(str, "%s ", ZSTR_VAL(type_str));
			zend_string_release(type_str);
		}
		smart_str_appends(str, "]\n");
	}
	smart_str_append_printf(str, "%s}\n", indent);
}
/* }}} */

static zval *property_get_default(zend_property_info *prop_info) {
	zend_class_entry *ce = prop_info->ce;
	if (prop_info->flags & ZEND_ACC_STATIC) {
		zval *prop = &ce->default_static_members_table[prop_info->offset];
		ZVAL_DEINDIRECT(prop);
		return prop;
	} else {
		return &ce->default_properties_table[OBJ_PROP_TO_NUM(prop_info->offset)];
	}
}

/* {{{ _property_string */
static void _property_string(smart_str *str, zend_property_info *prop, const char *prop_name, char* indent)
{
	smart_str_append_printf(str, "%sProperty [ ", indent);
	if (!prop) {
		smart_str_append_printf(str, "<dynamic> public $%s", prop_name);
	} else {
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
		if (ZEND_TYPE_IS_SET(prop->type)) {
			zend_string *type_str = zend_type_to_string(prop->type);
			smart_str_append(str, type_str);
			smart_str_appendc(str, ' ');
			zend_string_release(type_str);
		}
		if (!prop_name) {
			const char *class_name;
			zend_unmangle_property_name(prop->name, &class_name, &prop_name);
		}
		smart_str_append_printf(str, "$%s", prop_name);

		zval *default_value = property_get_default(prop);
		if (!Z_ISUNDEF_P(default_value)) {
			smart_str_appends(str, " = ");
			if (format_default_value(str, default_value, prop->ce) == FAILURE) {
				return;
			}
		}
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

/* {{{ reflection_attribute_factory */
static void reflection_attribute_factory(zval *object, HashTable *attributes, zend_attribute *data,
		zend_class_entry *scope, uint32_t target, zend_string *filename)
{
	reflection_object *intern;
	attribute_reference *reference;

	reflection_instantiate(reflection_attribute_ptr, object);
	intern  = Z_REFLECTION_P(object);
	reference = (attribute_reference*) emalloc(sizeof(attribute_reference));
	reference->attributes = attributes;
	reference->data = data;
	reference->scope = scope;
	reference->filename = filename ? zend_string_copy(filename) : NULL;
	reference->target = target;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_ATTRIBUTE;
}
/* }}} */

static int read_attributes(zval *ret, HashTable *attributes, zend_class_entry *scope,
		uint32_t offset, uint32_t target, zend_string *name, zend_class_entry *base, zend_string *filename) /* {{{ */
{
	ZEND_ASSERT(attributes != NULL);

	zend_attribute *attr;
	zval tmp;

	if (name) {
		// Name based filtering using lowercased key.
		zend_string *filter = zend_string_tolower(name);

		ZEND_HASH_FOREACH_PTR(attributes, attr) {
			if (attr->offset == offset && zend_string_equals(attr->lcname, filter)) {
				reflection_attribute_factory(&tmp, attributes, attr, scope, target, filename);
				add_next_index_zval(ret, &tmp);
			}
		} ZEND_HASH_FOREACH_END();

		zend_string_release(filter);
		return SUCCESS;
	}

	ZEND_HASH_FOREACH_PTR(attributes, attr) {
		if (attr->offset != offset) {
			continue;
		}

		if (base) {
			// Base type filtering.
			zend_class_entry *ce = zend_lookup_class_ex(attr->name, attr->lcname, 0);

			if (ce == NULL) {
				// Bailout on error, otherwise ignore unavailable class.
				if (EG(exception)) {
					return FAILURE;
				}

				continue;
			}

			if (!instanceof_function(ce, base)) {
				continue;
			}
		}

		reflection_attribute_factory(&tmp, attributes, attr, scope, target, filename);
		add_next_index_zval(ret, &tmp);
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}
/* }}} */

static void reflect_attributes(INTERNAL_FUNCTION_PARAMETERS, HashTable *attributes,
		uint32_t offset, zend_class_entry *scope, uint32_t target, zend_string *filename) /* {{{ */
{
	zend_string *name = NULL;
	zend_long flags = 0;
	zend_class_entry *base = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S!l", &name, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (flags & ~REFLECTION_ATTRIBUTE_IS_INSTANCEOF) {
		zend_argument_value_error(2, "must be a valid attribute filter flag");
		RETURN_THROWS();
	}

	if (name && (flags & REFLECTION_ATTRIBUTE_IS_INSTANCEOF)) {
		if (NULL == (base = zend_lookup_class(name))) {
			if (!EG(exception)) {
				zend_throw_error(NULL, "Class \"%s\" not found", ZSTR_VAL(name));
			}

			RETURN_THROWS();
		}

		name = NULL;
	}

	if (!attributes) {
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);

	if (FAILURE == read_attributes(return_value, attributes, scope, offset, target, name, base, filename)) {
		RETURN_THROWS();
	}
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
		RETURN_THROWS();
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
		ZVAL_OBJ_COPY(&intern->obj, Z_OBJ_P(closure_object));
	}

	prop_name = reflection_prop_name(object);
	if (has_internal_arg_info(fptr)) {
		ZVAL_STRING(prop_name, ((zend_internal_arg_info*)arg_info)->name);
	} else {
		ZVAL_STR_COPY(prop_name, arg_info->name);
	}
}
/* }}} */

/* For backwards compatibility reasons, we need to return T|null style unions
 * as a ReflectionNamedType. Here we determine what counts as a union type and
 * what doesn't. */
static zend_bool is_union_type(zend_type type) {
	if (ZEND_TYPE_HAS_LIST(type)) {
		return 1;
	}
	uint32_t type_mask_without_null = ZEND_TYPE_PURE_MASK_WITHOUT_NULL(type);
	if (ZEND_TYPE_HAS_CLASS(type)) {
		return type_mask_without_null != 0;
	}
	if (type_mask_without_null == MAY_BE_BOOL) {
		return 0;
	}
	/* Check that only one bit is set. */
	return (type_mask_without_null & (type_mask_without_null - 1)) != 0;
}

/* {{{ reflection_type_factory */
static void reflection_type_factory(zend_type type, zval *object, zend_bool legacy_behavior)
{
	reflection_object *intern;
	type_reference *reference;
	zend_bool is_union = is_union_type(type);
	zend_bool is_mixed = ZEND_TYPE_PURE_MASK(type) == MAY_BE_ANY;

	reflection_instantiate(is_union && !is_mixed ? reflection_union_type_ptr : reflection_named_type_ptr, object);
	intern = Z_REFLECTION_P(object);
	reference = (type_reference*) emalloc(sizeof(type_reference));
	reference->type = type;
	reference->legacy_behavior = legacy_behavior && !is_union && !is_mixed;
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_TYPE;

	/* Property types may be resolved during the lifetime of the ReflectionType.
	 * If we reference a string, make sure it doesn't get released. However, only
	 * do this for the top-level type, as resolutions inside type lists will be
	 * fully visible to us (we'd have to do a fully copy of the type if we wanted
	 * to prevent that). */
	if (ZEND_TYPE_HAS_NAME(type)) {
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
		ZVAL_OBJ_COPY(&intern->obj, Z_OBJ_P(closure_object));
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
		ZVAL_OBJ_COPY(&intern->obj, Z_OBJ_P(closure_object));
	}

	ZVAL_STR_COPY(reflection_prop_name(object), method->common.function_name);
	ZVAL_STR_COPY(reflection_prop_class(object), method->common.scope->name);
}
/* }}} */

/* {{{ reflection_property_factory */
static void reflection_property_factory(zend_class_entry *ce, zend_string *name, zend_property_info *prop, zval *object)
{
	reflection_object *intern;
	property_reference *reference;

	reflection_instantiate(reflection_property_ptr, object);
	intern = Z_REFLECTION_P(object);
	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->prop = prop;
	reference->unmangled_name = zend_string_copy(name);
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_PROPERTY;
	intern->ce = ce;
	intern->ignore_visibility = 0;
	ZVAL_STR_COPY(reflection_prop_name(object), name);
	ZVAL_STR_COPY(reflection_prop_class(object), prop ? prop->ce->name : ce->name);
}
/* }}} */

static void reflection_property_factory_str(zend_class_entry *ce, const char *name_str, size_t name_len, zend_property_info *prop, zval *object)
{
	zend_string *name = zend_string_init(name_str, name_len, 0);
	reflection_property_factory(ce, name, prop, object);
	zend_string_release(name);
}

/* {{{ reflection_class_constant_factory */
static void reflection_class_constant_factory(zend_string *name_str, zend_class_constant *constant, zval *object)
{
	reflection_object *intern;

	reflection_instantiate(reflection_class_constant_ptr, object);
	intern = Z_REFLECTION_P(object);
	intern->ptr = constant;
	intern->ref_type = REF_TYPE_CLASS_CONSTANT;
	intern->ce = constant->ce;
	intern->ignore_visibility = 0;

	ZVAL_STR_COPY(reflection_prop_name(object), name_str);
	ZVAL_STR_COPY(reflection_prop_class(object), constant->ce->name);
}
/* }}} */

static int get_parameter_default(zval *result, parameter_reference *param) {
	if (param->fptr->type == ZEND_INTERNAL_FUNCTION) {
		return zend_get_default_from_internal_arg_info(
			result, (zend_internal_arg_info *) param->arg_info);
	} else {
		zval *default_value = get_default_from_recv((zend_op_array *) param->fptr, param->offset);
		if (!default_value) {
			return FAILURE;
		}

		ZVAL_COPY(result, default_value);
		return SUCCESS;
	}
}

/* {{{ Preventing __clone from being called */
ZEND_METHOD(ReflectionClass, __clone)
{
	/* Should never be executable */
	_DO_THROW("Cannot clone object using __clone()");
}
/* }}} */

/* {{{ Returns an array of modifier names */
ZEND_METHOD(Reflection, getModifierNames)
{
	zend_long modifiers;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &modifiers) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Constructor. Throws an Exception in case the given function does not exist */
ZEND_METHOD(ReflectionFunction, __construct)
{
	zval *object;
	zend_object *closure_obj = NULL;
	reflection_object *intern;
	zend_function *fptr;
	zend_string *fname, *lcname;

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(closure_obj, zend_ce_closure, fname)
	ZEND_PARSE_PARAMETERS_END();

	if (closure_obj) {
		fptr = (zend_function*)zend_get_closure_method_def(closure_obj);
	} else {
		if (UNEXPECTED(ZSTR_VAL(fname)[0] == '\\')) {
			/* Ignore leading "\" */
			ALLOCA_FLAG(use_heap)
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
			RETURN_THROWS();
		}
	}

	ZVAL_STR_COPY(reflection_prop_name(object), fptr->common.function_name);
	intern->ptr = fptr;
	intern->ref_type = REF_TYPE_FUNCTION;
	if (closure_obj) {
		ZVAL_OBJ_COPY(&intern->obj, closure_obj);
	} else {
		ZVAL_UNDEF(&intern->obj);
	}
	intern->ce = NULL;
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionFunction, __toString)
{
	reflection_object *intern;
	zend_function *fptr;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	_function_string(&str, fptr, intern->ce, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ Returns this function's name */
ZEND_METHOD(ReflectionFunctionAbstract, getName)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_STR_COPY(fptr->common.function_name);
}
/* }}} */

/* {{{ Returns whether this is a closure */
ZEND_METHOD(ReflectionFunctionAbstract, isClosure)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->common.fn_flags & ZEND_ACC_CLOSURE);
}
/* }}} */

/* {{{ Returns this pointer bound to closure */
ZEND_METHOD(ReflectionFunctionAbstract, getClosureThis)
{
	reflection_object *intern;
	zval* closure_this;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT();
	if (!Z_ISUNDEF(intern->obj)) {
		closure_this = zend_get_closure_this_ptr(&intern->obj);
		if (!Z_ISUNDEF_P(closure_this)) {
			RETURN_OBJ_COPY(Z_OBJ_P(closure_this));
		}
	}
}
/* }}} */

/* {{{ Returns the scope associated to the closure */
ZEND_METHOD(ReflectionFunctionAbstract, getClosureScopeClass)
{
	reflection_object *intern;
	const zend_function *closure_func;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT();
	if (!Z_ISUNDEF(intern->obj)) {
		closure_func = zend_get_closure_method_def(Z_OBJ(intern->obj));
		if (closure_func && closure_func->common.scope) {
			zend_reflection_class_factory(closure_func->common.scope, return_value);
		}
	}
}
/* }}} */

/* {{{ Returns a dynamically created closure for the function */
ZEND_METHOD(ReflectionFunction, getClosure)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);

	if (!Z_ISUNDEF(intern->obj)) {
		/* Closures are immutable objects */
		RETURN_OBJ_COPY(Z_OBJ(intern->obj));
	} else {
		zend_create_fake_closure(return_value, fptr, NULL, NULL, NULL);
	}
}
/* }}} */

/* {{{ Returns whether this is an internal function */
ZEND_METHOD(ReflectionFunctionAbstract, isInternal)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_INTERNAL_FUNCTION);
}
/* }}} */

/* {{{ Returns whether this is a user-defined function */
ZEND_METHOD(ReflectionFunctionAbstract, isUserDefined)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	RETURN_BOOL(fptr->type == ZEND_USER_FUNCTION);
}
/* }}} */

/* {{{ Returns whether this function has been disabled or not */
ZEND_METHOD(ReflectionFunction, isDisabled)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	/* A disabled function cannot be queried using Reflection. */
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the filename of the file this function was declared in */
ZEND_METHOD(ReflectionFunctionAbstract, getFileName)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_STR_COPY(fptr->op_array.filename);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the line this function's declaration starts at */
ZEND_METHOD(ReflectionFunctionAbstract, getStartLine)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the line this function's declaration ends at */
ZEND_METHOD(ReflectionFunctionAbstract, getEndLine)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION) {
		RETURN_LONG(fptr->op_array.line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the doc comment for this function */
ZEND_METHOD(ReflectionFunctionAbstract, getDocComment)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(fptr);
	if (fptr->type == ZEND_USER_FUNCTION && fptr->op_array.doc_comment) {
		RETURN_STR_COPY(fptr->op_array.doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the attributes of this function */
ZEND_METHOD(ReflectionFunctionAbstract, getAttributes)
{
	reflection_object *intern;
	zend_function *fptr;
	uint32_t target;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (fptr->common.scope) {
		target = ZEND_ATTRIBUTE_TARGET_METHOD;
	} else {
		target = ZEND_ATTRIBUTE_TARGET_FUNCTION;
	}

	reflect_attributes(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		fptr->common.attributes, 0, fptr->common.scope, target,
		fptr->type == ZEND_USER_FUNCTION ? fptr->op_array.filename : NULL);
}
/* }}} */

/* {{{ Returns an associative array containing this function's static variables and their values */
ZEND_METHOD(ReflectionFunctionAbstract, getStaticVariables)
{
	reflection_object *intern;
	zend_function *fptr;
	zval *val;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Invokes the function */
ZEND_METHOD(ReflectionFunction, invoke)
{
	zval retval;
	zval *params;
	int result, num_args;
	HashTable *named_params;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	reflection_object *intern;
	zend_function *fptr;

	ZEND_PARSE_PARAMETERS_START(0, -1)
		Z_PARAM_VARIADIC_WITH_NAMED(params, num_args, named_params)
	ZEND_PARSE_PARAMETERS_END();

	GET_REFLECTION_OBJECT_PTR(fptr);

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = num_args;
	fci.params = params;
	fci.named_params = named_params;

	fcc.function_handler = fptr;
	fcc.called_scope = NULL;
	fcc.object = NULL;

	if (!Z_ISUNDEF(intern->obj)) {
		Z_OBJ_HT(intern->obj)->get_closure(
			Z_OBJ(intern->obj), &fcc.called_scope, &fcc.function_handler, &fcc.object, 0);
	}

	result = zend_call_function(&fci, &fcc);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of function %s() failed", ZSTR_VAL(fptr->common.function_name));
		RETURN_THROWS();
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ Invokes the function and pass its arguments as array. */
ZEND_METHOD(ReflectionFunction, invokeArgs)
{
	zval retval;
	int result;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	reflection_object *intern;
	zend_function *fptr;
	HashTable *params;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &params) == FAILURE) {
		RETURN_THROWS();
	}

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = NULL;
	fci.retval = &retval;
	fci.param_count = 0;
	fci.params = NULL;
	fci.named_params = params;

	fcc.function_handler = fptr;
	fcc.called_scope = NULL;
	fcc.object = NULL;

	if (!Z_ISUNDEF(intern->obj)) {
		Z_OBJ_HT(intern->obj)->get_closure(
			Z_OBJ(intern->obj), &fcc.called_scope, &fcc.function_handler, &fcc.object, 0);
	}

	result = zend_call_function(&fci, &fcc);

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of function %s() failed", ZSTR_VAL(fptr->common.function_name));
		RETURN_THROWS();
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ Gets whether this function returns a reference */
ZEND_METHOD(ReflectionFunctionAbstract, returnsReference)
{
	reflection_object *intern;
	zend_function *fptr;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL((fptr->op_array.fn_flags & ZEND_ACC_RETURN_REFERENCE) != 0);
}
/* }}} */

/* {{{ Gets the number of parameters */
ZEND_METHOD(ReflectionFunctionAbstract, getNumberOfParameters)
{
	reflection_object *intern;
	zend_function *fptr;
	uint32_t num_args;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}

	RETURN_LONG(num_args);
}
/* }}} */

/* {{{ Gets the number of required parameters */
ZEND_METHOD(ReflectionFunctionAbstract, getNumberOfRequiredParameters)
{
	reflection_object *intern;
	zend_function *fptr;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_LONG(fptr->common.required_num_args);
}
/* }}} */

/* {{{ Returns an array of parameter objects for this function */
ZEND_METHOD(ReflectionFunctionAbstract, getParameters)
{
	reflection_object *intern;
	zend_function *fptr;
	uint32_t i, num_args;
	struct _zend_arg_info *arg_info;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

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

/* {{{ Returns NULL or the extension the function belongs to */
ZEND_METHOD(ReflectionFunctionAbstract, getExtension)
{
	reflection_object *intern;
	zend_function *fptr;
	zend_internal_function *internal;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

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

/* {{{ Returns false or the name of the extension the function belongs to */
ZEND_METHOD(ReflectionFunctionAbstract, getExtensionName)
{
	reflection_object *intern;
	zend_function *fptr;
	zend_internal_function *internal;

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

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

/* {{{ */
ZEND_METHOD(ReflectionGenerator, __construct)
{
	zval *generator, *object;
	reflection_object *intern;
	zend_execute_data *ex;

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &generator, zend_ce_generator) == FAILURE) {
		RETURN_THROWS();
	}

	ex = ((zend_generator *) Z_OBJ_P(generator))->execute_data;
	if (!ex) {
		_DO_THROW("Cannot create ReflectionGenerator based on a terminated Generator");
		RETURN_THROWS();
	}

	intern->ref_type = REF_TYPE_GENERATOR;
	ZVAL_OBJ_COPY(&intern->obj, Z_OBJ_P(generator));
	intern->ce = zend_ce_generator;
}
/* }}} */

#define REFLECTION_CHECK_VALID_GENERATOR(ex) \
	if (!ex) { \
		_DO_THROW("Cannot fetch information from a terminated Generator"); \
		RETURN_THROWS(); \
	}

/* {{{ */
ZEND_METHOD(ReflectionGenerator, getTrace)
{
	zend_long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_generator *root_generator;
	zend_execute_data *ex_backup = EG(current_execute_data);
	zend_execute_data *ex = generator->execute_data;
	zend_execute_data *root_prev = NULL, *cur_prev;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &options) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ */
ZEND_METHOD(ReflectionGenerator, getExecutingLine)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	ZVAL_LONG(return_value, ex->opline->lineno);
}
/* }}} */

/* {{{ */
ZEND_METHOD(ReflectionGenerator, getExecutingFile)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	ZVAL_STR_COPY(return_value, ex->func->op_array.filename);
}
/* }}} */

/* {{{ */
ZEND_METHOD(ReflectionGenerator, getFunction)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ */
ZEND_METHOD(ReflectionGenerator, getThis)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	if (Z_TYPE(ex->This) == IS_OBJECT) {
		RETURN_OBJ_COPY(Z_OBJ(ex->This));
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ */
ZEND_METHOD(ReflectionGenerator, getExecutingGenerator)
{
	zend_generator *generator = (zend_generator *) Z_OBJ(Z_REFLECTION_P(ZEND_THIS)->obj);
	zend_execute_data *ex = generator->execute_data;
	zend_generator *current;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	REFLECTION_CHECK_VALID_GENERATOR(ex)

	current = zend_generator_get_current(generator);
	RETURN_OBJ_COPY(&current->std);
}
/* }}} */

/* {{{ Constructor. Throws an Exception in case the given method does not exist */
ZEND_METHOD(ReflectionParameter, __construct)
{
	parameter_reference *ref;
	zval *reference;
	zend_string *arg_name = NULL;
	zend_long position;
	zval *object;
	zval *prop_name;
	reflection_object *intern;
	zend_function *fptr;
	struct _zend_arg_info *arg_info;
	uint32_t num_args;
	zend_class_entry *ce = NULL;
	zend_bool is_closure = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(reference)
		Z_PARAM_STR_OR_LONG(arg_name, position)
	ZEND_PARSE_PARAMETERS_END();

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
					RETURN_THROWS();
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
					RETURN_THROWS();
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
								"Class \"%s\" does not exist", ZSTR_VAL(name));
						zend_string_release(name);
						RETURN_THROWS();
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
					zend_string_release(name);
					zend_string_release(lcname);
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Method %s::%s() does not exist", ZSTR_VAL(ce->name), Z_STRVAL_P(method));
					RETURN_THROWS();
				}
				zend_string_release(name);
				zend_string_release(lcname);
			}
			break;

		case IS_OBJECT: {
				ce = Z_OBJCE_P(reference);

				if (instanceof_function(ce, zend_ce_closure)) {
					fptr = (zend_function *)zend_get_closure_method_def(Z_OBJ_P(reference));
					Z_ADDREF_P(reference);
					is_closure = 1;
				} else if ((fptr = zend_hash_find_ptr(&ce->function_table, ZSTR_KNOWN(ZEND_STR_MAGIC_INVOKE))) == NULL) {
					zend_throw_exception_ex(reflection_exception_ptr, 0,
						"Method %s::%s() does not exist", ZSTR_VAL(ce->name), ZEND_INVOKE_FUNC_NAME);
					RETURN_THROWS();
				}
			}
			break;

		default:
			zend_argument_error(reflection_exception_ptr, 1, "must be a string, an array(class, method), or a callable object, %s given", zend_zval_type_name(reference));
			RETURN_THROWS();
	}

	/* Now, search for the parameter */
	arg_info = fptr->common.arg_info;
	num_args = fptr->common.num_args;
	if (fptr->common.fn_flags & ZEND_ACC_VARIADIC) {
		num_args++;
	}
	if (arg_name != NULL) {
		uint32_t i;
		position = -1;

		if (has_internal_arg_info(fptr)) {
			for (i = 0; i < num_args; i++) {
				if (arg_info[i].name) {
					if (strcmp(((zend_internal_arg_info*)arg_info)[i].name, ZSTR_VAL(arg_name)) == 0) {
						position = i;
						break;
					}
				}
			}
		} else {
			for (i = 0; i < num_args; i++) {
				if (arg_info[i].name) {
					if (zend_string_equals(arg_name, arg_info[i].name)) {
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
	} else {
		if (position < 0) {
			zend_argument_value_error(2, "must be greater than or equal to 0");
			goto failure;
		}
		if (position >= num_args) {
			_DO_THROW("The parameter specified by its offset could not be found");
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
	if (has_internal_arg_info(fptr)) {
		ZVAL_STRING(prop_name, ((zend_internal_arg_info*)arg_info)[position].name);
	} else {
		ZVAL_STR_COPY(prop_name, arg_info[position].name);
	}
	return;

failure:
	if (fptr->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) {
		zend_string_release_ex(fptr->common.function_name, 0);
		zend_free_trampoline(fptr);
	}
	if (is_closure) {
		zval_ptr_dtor(reference);
	}
	RETURN_THROWS();
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionParameter, __toString)
{
	reflection_object *intern;
	parameter_reference *param;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);
	_parameter_string(&str, param->fptr, param->arg_info, param->offset, param->required, "");
	RETURN_STR(smart_str_extract(&str));
}

/* }}} */

/* {{{ Returns this parameters's name */
ZEND_METHOD(ReflectionParameter, getName)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(param);
	if (has_internal_arg_info(param->fptr)) {
		RETURN_STRING(((zend_internal_arg_info *) param->arg_info)->name);
	} else {
		RETURN_STR_COPY(param->arg_info->name);
	}
}
/* }}} */

/* {{{ Returns the ReflectionFunction for the function of this parameter */
ZEND_METHOD(ReflectionParameter, getDeclaringFunction)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (!param->fptr->common.scope) {
		reflection_function_factory(_copy_function(param->fptr), Z_ISUNDEF(intern->obj)? NULL : &intern->obj, return_value);
	} else {
		reflection_method_factory(param->fptr->common.scope, _copy_function(param->fptr), Z_ISUNDEF(intern->obj)? NULL : &intern->obj, return_value);
	}
}
/* }}} */

/* {{{ Returns in which class this parameter is defined (not the type of the parameter) */
ZEND_METHOD(ReflectionParameter, getDeclaringClass)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (param->fptr->common.scope) {
		zend_reflection_class_factory(param->fptr->common.scope, return_value);
	}
}
/* }}} */

/* {{{ Returns this parameters's class hint or NULL if there is none */
ZEND_METHOD(ReflectionParameter, getClass)
{
	reflection_object *intern;
	parameter_reference *param;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	// TODO: This is going to return null for union types, which is rather odd.
	if (ZEND_TYPE_HAS_NAME(param->arg_info->type)) {
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
					"Parameter uses \"self\" as type but function is not a class member");
				RETURN_THROWS();
			}
		} else if (0 == zend_binary_strcasecmp(ZSTR_VAL(class_name), ZSTR_LEN(class_name), "parent", sizeof("parent")- 1)) {
			ce = param->fptr->common.scope;
			if (!ce) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter uses \"parent\" as type but function is not a class member");
				RETURN_THROWS();
			}
			if (!ce->parent) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Parameter uses \"parent\" as type although class does not have a parent");
				RETURN_THROWS();
			}
			ce = ce->parent;
		} else {
			ce = zend_lookup_class(class_name);
			if (!ce) {
				zend_throw_exception_ex(reflection_exception_ptr, 0,
					"Class \"%s\" does not exist", ZSTR_VAL(class_name));
				RETURN_THROWS();
			}
		}
		zend_reflection_class_factory(ce, return_value);
	}
}
/* }}} */

/* {{{ Returns whether parameter has a type */
ZEND_METHOD(ReflectionParameter, hasType)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(ZEND_TYPE_IS_SET(param->arg_info->type));
}
/* }}} */

/* {{{ Returns the type associated with the parameter */
ZEND_METHOD(ReflectionParameter, getType)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (!ZEND_TYPE_IS_SET(param->arg_info->type)) {
		RETURN_NULL();
	}
	reflection_type_factory(param->arg_info->type, return_value, 1);
}
/* }}} */

/* {{{ Returns whether parameter MUST be an array */
ZEND_METHOD(ReflectionParameter, isArray)
{
	reflection_object *intern;
	parameter_reference *param;
	uint32_t type_mask;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	type_mask = ZEND_TYPE_PURE_MASK_WITHOUT_NULL(param->arg_info->type);
	RETVAL_BOOL(type_mask == MAY_BE_ARRAY);
}
/* }}} */

/* {{{ Returns whether parameter MUST be callable */
ZEND_METHOD(ReflectionParameter, isCallable)
{
	reflection_object *intern;
	parameter_reference *param;
	uint32_t type_mask;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	type_mask = ZEND_TYPE_PURE_MASK_WITHOUT_NULL(param->arg_info->type);
	RETVAL_BOOL(type_mask == MAY_BE_CALLABLE);
}
/* }}} */

/* {{{ Returns whether NULL is allowed as this parameters's value */
ZEND_METHOD(ReflectionParameter, allowsNull)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(!ZEND_TYPE_IS_SET(param->arg_info->type)
		|| ZEND_TYPE_ALLOW_NULL(param->arg_info->type));
}
/* }}} */

/* {{{ Returns whether this parameters is passed to by reference */
ZEND_METHOD(ReflectionParameter, isPassedByReference)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(ZEND_ARG_SEND_MODE(param->arg_info));
}
/* }}} */

/* {{{ Returns whether this parameter can be passed by value */
ZEND_METHOD(ReflectionParameter, canBePassedByValue)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	/* true if it's ZEND_SEND_BY_VAL or ZEND_SEND_PREFER_REF */
	RETVAL_BOOL(ZEND_ARG_SEND_MODE(param->arg_info) != ZEND_SEND_BY_REF);
}
/* }}} */

/* {{{ Get parameter attributes. */
ZEND_METHOD(ReflectionParameter, getAttributes)
{
	reflection_object *intern;
	parameter_reference *param;

	GET_REFLECTION_OBJECT_PTR(param);

	HashTable *attributes = param->fptr->common.attributes;
	zend_class_entry *scope = param->fptr->common.scope;

	reflect_attributes(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		attributes, param->offset + 1, scope, ZEND_ATTRIBUTE_TARGET_PARAMETER,
		param->fptr->type == ZEND_USER_FUNCTION ? param->fptr->op_array.filename : NULL);
}

/* {{{ Returns whether this parameter is an optional parameter */
ZEND_METHOD(ReflectionParameter, getPosition)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_LONG(param->offset);
}
/* }}} */

/* {{{ Returns whether this parameter is an optional parameter */
ZEND_METHOD(ReflectionParameter, isOptional)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(!param->required);
}
/* }}} */

/* {{{ Returns whether the default value of this parameter is available */
ZEND_METHOD(ReflectionParameter, isDefaultValueAvailable)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(param);

	if (param->fptr->type == ZEND_INTERNAL_FUNCTION) {
		RETURN_BOOL(((zend_internal_arg_info*) (param->arg_info))->default_value);
	} else {
		zval *default_value = get_default_from_recv((zend_op_array *)param->fptr, param->offset);
		RETURN_BOOL(default_value != NULL);
	}
}
/* }}} */

/* {{{ Returns the default value of this parameter or throws an exception */
ZEND_METHOD(ReflectionParameter, getDefaultValue)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(param);

	if (get_parameter_default(return_value, param) == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Internal error: Failed to retrieve the default value");
		RETURN_THROWS();
	}

	if (Z_TYPE_P(return_value) == IS_CONSTANT_AST) {
		zval_update_constant_ex(return_value, param->fptr->common.scope);
	}
}
/* }}} */

/* {{{ Returns whether the default value of this parameter is constant */
ZEND_METHOD(ReflectionParameter, isDefaultValueConstant)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(param);

	zval default_value;
	if (get_parameter_default(&default_value, param) == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Internal error: Failed to retrieve the default value");
		RETURN_THROWS();
	}

	if (Z_TYPE(default_value) == IS_CONSTANT_AST) {
		zend_ast *ast = Z_ASTVAL(default_value);
		RETVAL_BOOL(ast->kind == ZEND_AST_CONSTANT || ast->kind == ZEND_AST_CONSTANT_CLASS);
	} else {
		RETVAL_FALSE;
	}

	zval_ptr_dtor_nogc(&default_value);
}
/* }}} */

/* {{{ Returns the default value's constant name if default value is constant or null */
ZEND_METHOD(ReflectionParameter, getDefaultValueConstantName)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(param);

	zval default_value;
	if (get_parameter_default(&default_value, param) == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Internal error: Failed to retrieve the default value");
		RETURN_THROWS();
	}

	if (Z_TYPE(default_value) != IS_CONSTANT_AST) {
		zval_ptr_dtor_nogc(&default_value);
		RETURN_NULL();
	}

	zend_ast *ast = Z_ASTVAL(default_value);
	if (ast->kind == ZEND_AST_CONSTANT) {
		RETVAL_STR_COPY(zend_ast_get_constant_name(ast));
	} else if (ast->kind == ZEND_AST_CONSTANT_CLASS) {
		RETVAL_STRINGL("__CLASS__", sizeof("__CLASS__")-1);
	} else {
		RETVAL_NULL();
	}
	zval_ptr_dtor_nogc(&default_value);
}

/* {{{ Returns whether this parameter is a variadic parameter */
ZEND_METHOD(ReflectionParameter, isVariadic)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(ZEND_ARG_IS_VARIADIC(param->arg_info));
}
/* }}} */

/* {{{ Returns this constructor parameter has been promoted to a property */
ZEND_METHOD(ReflectionParameter, isPromoted)
{
	reflection_object *intern;
	parameter_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(ZEND_ARG_IS_PROMOTED(param->arg_info));
}
/* }}} */

/* {{{ Returns whether parameter MAY be null */
ZEND_METHOD(ReflectionType, allowsNull)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETVAL_BOOL(ZEND_TYPE_ALLOW_NULL(param->type));
}
/* }}} */

static zend_string *zend_type_to_string_without_null(zend_type type) {
	ZEND_TYPE_FULL_MASK(type) &= ~MAY_BE_NULL;
	return zend_type_to_string(type);
}

/* {{{ Return the text of the type hint */
ZEND_METHOD(ReflectionType, __toString)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	RETURN_STR(zend_type_to_string(param->type));
}
/* }}} */

/* {{{ Return the name of the type */
ZEND_METHOD(ReflectionNamedType, getName)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	if (param->legacy_behavior) {
		RETURN_STR(zend_type_to_string_without_null(param->type));
	}
	RETURN_STR(zend_type_to_string(param->type));
}
/* }}} */

/* {{{ Returns whether type is a builtin type */
ZEND_METHOD(ReflectionNamedType, isBuiltin)
{
	reflection_object *intern;
	type_reference *param;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	/* Treat "static" as a class type for the purposes of reflection. */
	RETVAL_BOOL(ZEND_TYPE_IS_ONLY_MASK(param->type)
		&& !(ZEND_TYPE_FULL_MASK(param->type) & MAY_BE_STATIC));
}
/* }}} */

static void append_type(zval *return_value, zend_type type) {
	zval reflection_type;
	reflection_type_factory(type, &reflection_type, 0);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &reflection_type);
}

static void append_type_mask(zval *return_value, uint32_t type_mask) {
	append_type(return_value, (zend_type) ZEND_TYPE_INIT_MASK(type_mask));
}

/* {{{ Returns the types that are part of this union type */
ZEND_METHOD(ReflectionUnionType, getTypes)
{
	reflection_object *intern;
	type_reference *param;
	uint32_t type_mask;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(param);

	array_init(return_value);
	if (ZEND_TYPE_HAS_LIST(param->type)) {
		zend_type *list_type;
		ZEND_TYPE_LIST_FOREACH(ZEND_TYPE_LIST(param->type), list_type) {
			append_type(return_value, *list_type);
		} ZEND_TYPE_LIST_FOREACH_END();
	} else if (ZEND_TYPE_HAS_NAME(param->type)) {
		append_type(return_value,
			(zend_type) ZEND_TYPE_INIT_CLASS(ZEND_TYPE_NAME(param->type), 0, 0));
	} else if (ZEND_TYPE_HAS_CE(param->type)) {
		append_type(return_value,
			(zend_type) ZEND_TYPE_INIT_CE(ZEND_TYPE_CE(param->type), 0, 0));
	}

	type_mask = ZEND_TYPE_PURE_MASK(param->type);
	ZEND_ASSERT(!(type_mask & MAY_BE_VOID));
	if (type_mask & MAY_BE_STATIC) {
		append_type_mask(return_value, MAY_BE_STATIC);
	}
	if (type_mask & MAY_BE_CALLABLE) {
		append_type_mask(return_value, MAY_BE_CALLABLE);
	}
	if (type_mask & MAY_BE_ITERABLE) {
		append_type_mask(return_value, MAY_BE_ITERABLE);
	}
	if (type_mask & MAY_BE_OBJECT) {
		append_type_mask(return_value, MAY_BE_OBJECT);
	}
	if (type_mask & MAY_BE_ARRAY) {
		append_type_mask(return_value, MAY_BE_ARRAY);
	}
	if (type_mask & MAY_BE_STRING) {
		append_type_mask(return_value, MAY_BE_STRING);
	}
	if (type_mask & MAY_BE_LONG) {
		append_type_mask(return_value, MAY_BE_LONG);
	}
	if (type_mask & MAY_BE_DOUBLE) {
		append_type_mask(return_value, MAY_BE_DOUBLE);
	}
	if ((type_mask & MAY_BE_BOOL) == MAY_BE_BOOL) {
		append_type_mask(return_value, MAY_BE_BOOL);
	} else if (type_mask & MAY_BE_FALSE) {
		append_type_mask(return_value, MAY_BE_FALSE);
	}
	if (type_mask & MAY_BE_NULL) {
		append_type_mask(return_value, MAY_BE_NULL);
	}
}
/* }}} */

/* {{{ Constructor. Throws an Exception in case the given method does not exist */
ZEND_METHOD(ReflectionMethod, __construct)
{
	zend_object *arg1_obj;
	zend_string *arg1_str;
	zend_string *arg2_str = NULL;

	zend_object *orig_obj = NULL;
	zend_class_entry *ce = NULL;
	zend_string *class_name = NULL;
	char *method_name;
	size_t method_name_len;
	char *lcname;

	zval *object;
	reflection_object *intern;
	zend_function *mptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_OBJ_OR_STR(arg1_obj, arg1_str)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(arg2_str)
	ZEND_PARSE_PARAMETERS_END();

	if (arg1_obj) {
		if (!arg2_str) {
			zend_argument_value_error(2, "cannot be null when argument #1 ($objectOrMethod) is an object");
			RETURN_THROWS();
		}

		orig_obj = arg1_obj;
		ce = arg1_obj->ce;
		method_name = ZSTR_VAL(arg2_str);
		method_name_len = ZSTR_LEN(arg2_str);
	} else if (arg2_str) {
		class_name = zend_string_copy(arg1_str);
		method_name = ZSTR_VAL(arg2_str);
		method_name_len = ZSTR_LEN(arg2_str);
	} else {
		char *tmp;
		size_t tmp_len;
		char *name = ZSTR_VAL(arg1_str);

		if ((tmp = strstr(name, "::")) == NULL) {
			zend_argument_error(reflection_exception_ptr, 1, "must be a valid method name");
			RETURN_THROWS();
		}
		tmp_len = tmp - name;

		class_name = zend_string_init(name, tmp_len, 0);
		method_name = tmp + 2;
		method_name_len = ZSTR_LEN(arg1_str) - tmp_len - 2;
	}

	if (class_name) {
		if ((ce = zend_lookup_class(class_name)) == NULL) {
			if (!EG(exception)) {
				zend_throw_exception_ex(reflection_exception_ptr, 0, "Class \"%s\" does not exist", ZSTR_VAL(class_name));
			}
			zend_string_release(class_name);
			RETURN_THROWS();
		}

		zend_string_release(class_name);
	}

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	lcname = zend_str_tolower_dup(method_name, method_name_len);

	if (ce == zend_ce_closure && orig_obj && (method_name_len == sizeof(ZEND_INVOKE_FUNC_NAME)-1)
		&& memcmp(lcname, ZEND_INVOKE_FUNC_NAME, sizeof(ZEND_INVOKE_FUNC_NAME)-1) == 0
		&& (mptr = zend_get_closure_invoke_method(orig_obj)) != NULL)
	{
		/* do nothing, mptr already set */
	} else if ((mptr = zend_hash_str_find_ptr(&ce->function_table, lcname, method_name_len)) == NULL) {
		efree(lcname);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Method %s::%s() does not exist", ZSTR_VAL(ce->name), method_name);
		RETURN_THROWS();
	}
	efree(lcname);

	ZVAL_STR_COPY(reflection_prop_name(object), mptr->common.function_name);
	ZVAL_STR_COPY(reflection_prop_class(object), mptr->common.scope->name);
	intern->ptr = mptr;
	intern->ref_type = REF_TYPE_FUNCTION;
	intern->ce = ce;
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionMethod, __toString)
{
	reflection_object *intern;
	zend_function *mptr;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	_function_string(&str, mptr, intern->ce, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ Invokes the function */
ZEND_METHOD(ReflectionMethod, getClosure)
{
	reflection_object *intern;
	zval *obj = NULL;
	zend_function *mptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|o!", &obj) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(mptr);

	if (mptr->common.fn_flags & ZEND_ACC_STATIC)  {
		zend_create_fake_closure(return_value, mptr, mptr->common.scope, mptr->common.scope, NULL);
	} else {
		if (!obj) {
			zend_argument_value_error(1, "cannot be null for non-static methods");
			RETURN_THROWS();
		}

		if (!instanceof_function(Z_OBJCE_P(obj), mptr->common.scope)) {
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			RETURN_THROWS();
		}

		/* This is an original closure object and __invoke is to be called. */
		if (Z_OBJCE_P(obj) == zend_ce_closure &&
			(mptr->internal_function.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE))
		{
			RETURN_OBJ_COPY(Z_OBJ_P(obj));
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
	zval *params = NULL, *object;
	HashTable *named_params = NULL;
	reflection_object *intern;
	zend_function *mptr;
	int argc = 0, result;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zend_class_entry *obj_ce;

	GET_REFLECTION_OBJECT_PTR(mptr);

	if (mptr->common.fn_flags & ZEND_ACC_ABSTRACT) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Trying to invoke abstract method %s::%s()",
			ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		RETURN_THROWS();
	}

	if (!(mptr->common.fn_flags & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Trying to invoke %s method %s::%s() from scope %s",
			mptr->common.fn_flags & ZEND_ACC_PROTECTED ? "protected" : "private",
			ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name),
			ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
		RETURN_THROWS();
	}

	if (variadic) {
		ZEND_PARSE_PARAMETERS_START(1, -1)
			Z_PARAM_OBJECT_OR_NULL(object)
			Z_PARAM_VARIADIC_WITH_NAMED(params, argc, named_params)
		ZEND_PARSE_PARAMETERS_END();
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "o!h", &object, &named_params) == FAILURE) {
			RETURN_THROWS();
		}
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
			RETURN_THROWS();
		}

		obj_ce = Z_OBJCE_P(object);

		if (!instanceof_function(obj_ce, mptr->common.scope)) {
			if (!variadic) {
				efree(params);
			}
			_DO_THROW("Given object is not an instance of the class this method was declared in");
			RETURN_THROWS();
		}
	}

	fci.size = sizeof(fci);
	ZVAL_UNDEF(&fci.function_name);
	fci.object = object ? Z_OBJ_P(object) : NULL;
	fci.retval = &retval;
	fci.param_count = argc;
	fci.params = params;
	fci.named_params = named_params;

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

	if (result == FAILURE) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Invocation of method %s::%s() failed", ZSTR_VAL(mptr->common.scope->name), ZSTR_VAL(mptr->common.function_name));
		RETURN_THROWS();
	}

	if (Z_TYPE(retval) != IS_UNDEF) {
		if (Z_ISREF(retval)) {
			zend_unwrap_reference(&retval);
		}
		ZVAL_COPY_VALUE(return_value, &retval);
	}
}
/* }}} */

/* {{{ Invokes the method. */
ZEND_METHOD(ReflectionMethod, invoke)
{
	reflection_method_invoke(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Invokes the function and pass its arguments as array. */
ZEND_METHOD(ReflectionMethod, invokeArgs)
{
	reflection_method_invoke(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Returns whether this method is final */
ZEND_METHOD(ReflectionMethod, isFinal)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL);
}
/* }}} */

/* {{{ Returns whether this method is abstract */
ZEND_METHOD(ReflectionMethod, isAbstract)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_ABSTRACT);
}
/* }}} */

/* {{{ Returns whether this method is public */
ZEND_METHOD(ReflectionMethod, isPublic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ Returns whether this method is private */
ZEND_METHOD(ReflectionMethod, isPrivate)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ Returns whether this method is protected */
ZEND_METHOD(ReflectionMethod, isProtected)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ Returns whether this method is static */
ZEND_METHOD(ReflectionMethod, isStatic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ Returns whether this function is deprecated */
ZEND_METHOD(ReflectionFunctionAbstract, isDeprecated)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_DEPRECATED);
}
/* }}} */

/* {{{ Returns whether this function is a generator */
ZEND_METHOD(ReflectionFunctionAbstract, isGenerator)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_GENERATOR);
}
/* }}} */

/* {{{ Returns whether this function is variadic */
ZEND_METHOD(ReflectionFunctionAbstract, isVariadic)
{
	_function_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_VARIADIC);
}
/* }}} */

/* {{{ Returns whether this function is defined in namespace */
ZEND_METHOD(ReflectionFunctionAbstract, inNamespace)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(fptr);

	zend_string *name = fptr->common.function_name;
	const char *backslash = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	RETURN_BOOL(backslash && backslash > ZSTR_VAL(name));
}
/* }}} */

/* {{{ Returns the name of namespace where this function is defined */
ZEND_METHOD(ReflectionFunctionAbstract, getNamespaceName)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(fptr);

	zend_string *name = fptr->common.function_name;
	const char *backslash = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	if (backslash && backslash > ZSTR_VAL(name)) {
		RETURN_STRINGL(ZSTR_VAL(name), backslash - ZSTR_VAL(name));
	}
	RETURN_EMPTY_STRING();
}
/* }}} */

/* {{{ Returns the short name of the function (without namespace part) */
ZEND_METHOD(ReflectionFunctionAbstract, getShortName)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(fptr);

	zend_string *name = fptr->common.function_name;
	const char *backslash = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	if (backslash && backslash > ZSTR_VAL(name)) {
		RETURN_STRINGL(backslash + 1, ZSTR_LEN(name) - (backslash - ZSTR_VAL(name) + 1));
	}
	RETURN_STR_COPY(name);
}
/* }}} */

/* {{{ Return whether the function has a return type */
ZEND_METHOD(ReflectionFunctionAbstract, hasReturnType)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(fptr);

	RETVAL_BOOL(fptr->op_array.fn_flags & ZEND_ACC_HAS_RETURN_TYPE);
}
/* }}} */

/* {{{ Returns the return type associated with the function */
ZEND_METHOD(ReflectionFunctionAbstract, getReturnType)
{
	reflection_object *intern;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(fptr);

	if (!(fptr->op_array.fn_flags & ZEND_ACC_HAS_RETURN_TYPE)) {
		RETURN_NULL();
	}

	reflection_type_factory(fptr->common.arg_info[-1].type, return_value, 1);
}
/* }}} */

/* {{{ Returns whether this method is the constructor */
ZEND_METHOD(ReflectionMethod, isConstructor)
{
	reflection_object *intern;
	zend_function *mptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	/* we need to check if the ctor is the ctor of the class level we we
	 * looking at since we might be looking at an inherited old style ctor
	 * defined in base class. */
	RETURN_BOOL((mptr->common.fn_flags & ZEND_ACC_CTOR) && intern->ce->constructor && intern->ce->constructor->common.scope == mptr->common.scope);
}
/* }}} */

/* {{{ Returns whether this method is a destructor */
ZEND_METHOD(ReflectionMethod, isDestructor)
{
	reflection_object *intern;
	zend_function *mptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(mptr);
	RETURN_BOOL(zend_string_equals_literal_ci(
		mptr->common.function_name, ZEND_DESTRUCTOR_FUNC_NAME));
}
/* }}} */

/* {{{ Returns a bitfield of the access modifiers for this method */
ZEND_METHOD(ReflectionMethod, getModifiers)
{
	reflection_object *intern;
	zend_function *mptr;
	uint32_t keep_flags = ZEND_ACC_PPP_MASK
		| ZEND_ACC_STATIC | ZEND_ACC_ABSTRACT | ZEND_ACC_FINAL;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(mptr);

	RETURN_LONG((mptr->common.fn_flags & keep_flags));
}
/* }}} */

/* {{{ Get the declaring class */
ZEND_METHOD(ReflectionMethod, getDeclaringClass)
{
	reflection_object *intern;
	zend_function *mptr;

	GET_REFLECTION_OBJECT_PTR(mptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	zend_reflection_class_factory(mptr->common.scope, return_value);
}
/* }}} */

/* {{{ Get the prototype */
ZEND_METHOD(ReflectionMethod, getPrototype)
{
	reflection_object *intern;
	zend_function *mptr;

	GET_REFLECTION_OBJECT_PTR(mptr);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (!mptr->common.prototype) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Method %s::%s does not have a prototype", ZSTR_VAL(intern->ce->name), ZSTR_VAL(mptr->common.function_name));
		RETURN_THROWS();
	}

	reflection_method_factory(mptr->common.prototype->common.scope, mptr->common.prototype, NULL, return_value);
}
/* }}} */

/* {{{ Sets whether non-public methods can be invoked */
ZEND_METHOD(ReflectionMethod, setAccessible)
{
	reflection_object *intern;
	zend_bool visible;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &visible) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_REFLECTION_P(ZEND_THIS);

	intern->ignore_visibility = visible;
}
/* }}} */

/* {{{ Constructor. Throws an Exception in case the given class constant does not exist */
ZEND_METHOD(ReflectionClassConstant, __construct)
{
	zval *object;
	zend_string *classname_str;
	zend_object *classname_obj;
	zend_string *constname;
	reflection_object *intern;
	zend_class_entry *ce;
	zend_class_constant *constant = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJ_OR_STR(classname_obj, classname_str)
		Z_PARAM_STR(constname)
	ZEND_PARSE_PARAMETERS_END();

	if (classname_obj) {
		ce = classname_obj->ce;
	} else {
		if ((ce = zend_lookup_class(classname_str)) == NULL) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Class \"%s\" does not exist", ZSTR_VAL(classname_str));
			RETURN_THROWS();
		}
	}

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	if ((constant = zend_hash_find_ptr(&ce->constants_table, constname)) == NULL) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Constant %s::%s does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(constname));
		RETURN_THROWS();
	}

	intern->ptr = constant;
	intern->ref_type = REF_TYPE_CLASS_CONSTANT;
	intern->ce = constant->ce;
	intern->ignore_visibility = 0;
	ZVAL_STR_COPY(reflection_prop_name(object), constname);
	ZVAL_STR_COPY(reflection_prop_class(object), constant->ce->name);
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionClassConstant, __toString)
{
	reflection_object *intern;
	zend_class_constant *ref;
	smart_str str = {0};
	zval name;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	_default_get_name(ZEND_THIS, &name);
	_class_const_string(&str, Z_STRVAL(name), ref, "");
	zval_ptr_dtor(&name);
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ Returns the constant' name */
ZEND_METHOD(ReflectionClassConstant, getName)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	_default_get_name(ZEND_THIS, return_value);
}
/* }}} */

static void _class_constant_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask) /* {{{ */
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(Z_ACCESS_FLAGS(ref->value) & mask);
}
/* }}} */

/* {{{ Returns whether this constant is public */
ZEND_METHOD(ReflectionClassConstant, isPublic)
{
	_class_constant_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ Returns whether this constant is private */
ZEND_METHOD(ReflectionClassConstant, isPrivate)
{
	_class_constant_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ Returns whether this constant is protected */
ZEND_METHOD(ReflectionClassConstant, isProtected)
{
	_class_constant_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ Returns a bitfield of the access modifiers for this constant */
ZEND_METHOD(ReflectionClassConstant, getModifiers)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	RETURN_LONG(Z_ACCESS_FLAGS(ref->value));
}
/* }}} */

/* {{{ Returns this constant's value */
ZEND_METHOD(ReflectionClassConstant, getValue)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	ZVAL_COPY_OR_DUP(return_value, &ref->value);
	if (Z_TYPE_P(return_value) == IS_CONSTANT_AST) {
		zval_update_constant_ex(return_value, ref->ce);
	}
}
/* }}} */

/* {{{ Get the declaring class */
ZEND_METHOD(ReflectionClassConstant, getDeclaringClass)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	zend_reflection_class_factory(ref->ce, return_value);
}
/* }}} */

/* {{{ Returns the doc comment for this constant */
ZEND_METHOD(ReflectionClassConstant, getDocComment)
{
	reflection_object *intern;
	zend_class_constant *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	if (ref->doc_comment) {
		RETURN_STR_COPY(ref->doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the attributes of this constant */
ZEND_METHOD(ReflectionClassConstant, getAttributes)
{
	reflection_object *intern;
	zend_class_constant *ref;

	GET_REFLECTION_OBJECT_PTR(ref);

	reflect_attributes(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		ref->attributes, 0, ref->ce, ZEND_ATTRIBUTE_TARGET_CLASS_CONST,
		ref->ce->type == ZEND_USER_CLASS ? ref->ce->info.user.filename : NULL);
}
/* }}} */

/* {{{ reflection_class_object_ctor */
static void reflection_class_object_ctor(INTERNAL_FUNCTION_PARAMETERS, int is_object)
{
	zval *object;
	zend_string *arg_class = NULL;
	zend_object *arg_obj;
	reflection_object *intern;
	zend_class_entry *ce;

	if (is_object) {
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_OBJ(arg_obj)
		ZEND_PARSE_PARAMETERS_END();
	} else {
		ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_OBJ_OR_STR(arg_obj, arg_class)
		ZEND_PARSE_PARAMETERS_END();
	}

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	if (arg_obj) {
		ZVAL_STR_COPY(reflection_prop_name(object), arg_obj->ce->name);
		intern->ptr = arg_obj->ce;
		if (is_object) {
			ZVAL_OBJ_COPY(&intern->obj, arg_obj);
		}
	} else {
		if ((ce = zend_lookup_class(arg_class)) == NULL) {
			if (!EG(exception)) {
				zend_throw_exception_ex(reflection_exception_ptr, -1, "Class \"%s\" does not exist", ZSTR_VAL(arg_class));
			}
			RETURN_THROWS();
		}

		ZVAL_STR_COPY(reflection_prop_name(object), ce->name);
		intern->ptr = ce;
	}
	intern->ref_type = REF_TYPE_OTHER;
}
/* }}} */

/* {{{ Constructor. Takes a string or an instance as an argument */
ZEND_METHOD(ReflectionClass, __construct)
{
	reflection_class_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ add_class_vars */
static void add_class_vars(zend_class_entry *ce, zend_bool statics, zval *return_value)
{
	zend_property_info *prop_info;
	zval *prop, prop_copy;
	zend_string *key;

	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->properties_info, key, prop_info) {
		if (((prop_info->flags & ZEND_ACC_PRIVATE) &&
		     prop_info->ce != ce)) {
			continue;
		}

		zend_bool is_static = (prop_info->flags & ZEND_ACC_STATIC) != 0;
		if (statics != is_static) {
			continue;
		}

		prop = property_get_default(prop_info);
		if (Z_ISUNDEF_P(prop)) {
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

/* {{{ Returns an associative array containing all static property values of the class */
ZEND_METHOD(ReflectionClass, getStaticProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_property_info *prop_info;
	zval *prop;
	zend_string *key;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		RETURN_THROWS();
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

		if (ZEND_TYPE_IS_SET(prop_info->type) && Z_ISUNDEF_P(prop)) {
			continue;
		}

		/* enforce read only access */
		ZVAL_DEREF(prop);
		Z_TRY_ADDREF_P(prop);

		zend_hash_update(Z_ARRVAL_P(return_value), key, prop);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns the value of a static property */
ZEND_METHOD(ReflectionClass, getStaticPropertyValue)
{
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	zend_string *name;
	zval *prop, *def_value = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|z", &name, &def_value) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		RETURN_THROWS();
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
				"Property %s::$%s does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		}
		return;
	} else {
		ZVAL_COPY_DEREF(return_value, prop);
	}
}
/* }}} */

/* {{{ Sets the value of a static property */
ZEND_METHOD(ReflectionClass, setStaticPropertyValue)
{
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	zend_property_info *prop_info;
	zend_string *name;
	zval *variable_ptr, *value;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		RETURN_THROWS();
	}
	old_scope = EG(fake_scope);
	EG(fake_scope) = ce;
	variable_ptr =  zend_std_get_static_property_with_info(ce, name, BP_VAR_W, &prop_info);
	EG(fake_scope) = old_scope;
	if (!variable_ptr) {
		zend_clear_exception();
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Class %s does not have a property named %s", ZSTR_VAL(ce->name), ZSTR_VAL(name));
		RETURN_THROWS();
	}

	if (Z_ISREF_P(variable_ptr)) {
		zend_reference *ref = Z_REF_P(variable_ptr);
		variable_ptr = Z_REFVAL_P(variable_ptr);

		if (!zend_verify_ref_assignable_zval(ref, value, 0)) {
			return;
		}
	}

	if (ZEND_TYPE_IS_SET(prop_info->type) && !zend_verify_property_type(prop_info, value, 0)) {
		return;
	}

	zval_ptr_dtor(variable_ptr);
	ZVAL_COPY(variable_ptr, value);

}
/* }}} */

/* {{{ Returns an associative array containing copies of all default property values of the class */
ZEND_METHOD(ReflectionClass, getDefaultProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	array_init(return_value);
	if (UNEXPECTED(zend_update_class_constants(ce) != SUCCESS)) {
		RETURN_THROWS();
	}
	add_class_vars(ce, 1, return_value);
	add_class_vars(ce, 0, return_value);
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionClass, __toString)
{
	reflection_object *intern;
	zend_class_entry *ce;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	_class_string(&str, ce, &intern->obj, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ Returns the class' name */
ZEND_METHOD(ReflectionClass, getName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_STR_COPY(ce->name);
}
/* }}} */

/* {{{ Returns whether this class is an internal class */
ZEND_METHOD(ReflectionClass, isInternal)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_INTERNAL_CLASS);
}
/* }}} */

/* {{{ Returns whether this class is user-defined */
ZEND_METHOD(ReflectionClass, isUserDefined)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->type == ZEND_USER_CLASS);
}
/* }}} */

/* {{{ Returns whether this class is anonymous */
ZEND_METHOD(ReflectionClass, isAnonymous)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(ce->ce_flags & ZEND_ACC_ANON_CLASS);
}
/* }}} */

/* {{{ Returns the filename of the file this class was declared in */
ZEND_METHOD(ReflectionClass, getFileName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_STR_COPY(ce->info.user.filename);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the line this class' declaration starts at */
ZEND_METHOD(ReflectionClass, getStartLine)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_LONG(ce->info.user.line_start);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the line this class' declaration ends at */
ZEND_METHOD(ReflectionClass, getEndLine)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS) {
		RETURN_LONG(ce->info.user.line_end);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the doc comment for this class */
ZEND_METHOD(ReflectionClass, getDocComment)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	if (ce->type == ZEND_USER_CLASS && ce->info.user.doc_comment) {
		RETURN_STR_COPY(ce->info.user.doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the attributes for this class */
ZEND_METHOD(ReflectionClass, getAttributes)
{
	reflection_object *intern;
	zend_class_entry *ce;

	GET_REFLECTION_OBJECT_PTR(ce);

	reflect_attributes(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		ce->attributes, 0, ce, ZEND_ATTRIBUTE_TARGET_CLASS,
		ce->type == ZEND_USER_CLASS ? ce->info.user.filename : NULL);
}
/* }}} */

/* {{{ Returns the class' constructor if there is one, NULL otherwise */
ZEND_METHOD(ReflectionClass, getConstructor)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->constructor) {
		reflection_method_factory(ce, ce->constructor, NULL, return_value);
	} else {
		RETURN_NULL();
	}
}
/* }}} */

/* {{{ Returns whether a method exists or not */
ZEND_METHOD(ReflectionClass, hasMethod)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *name, *lc_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	lc_name = zend_string_tolower(name);
	RETVAL_BOOL(zend_hash_exists(&ce->function_table, lc_name) || is_closure_invoke(ce, lc_name));
	zend_string_release(lc_name);
}
/* }}} */

/* {{{ Returns the class' method specified by its name */
ZEND_METHOD(ReflectionClass, getMethod)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_function *mptr;
	zval obj_tmp;
	zend_string *name, *lc_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		RETURN_THROWS();
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
				"Method %s::%s() does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(name));
	}
	zend_string_release(lc_name);
}
/* }}} */

/* {{{ _addmethod */
static void _addmethod(zend_function *mptr, zend_class_entry *ce, zval *retval, zend_long filter)
{
	if ((mptr->common.fn_flags & ZEND_ACC_PRIVATE) && mptr->common.scope != ce) {
		return;
	}

	if (mptr->common.fn_flags & filter) {
		zval method;
		reflection_method_factory(ce, mptr, NULL, &method);
		add_next_index_zval(retval, &method);
	}
}
/* }}} */

/* {{{ Returns an array of this class' methods */
ZEND_METHOD(ReflectionClass, getMethods)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_function *mptr;
	zend_long filter;
	zend_bool filter_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &filter, &filter_is_null) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns whether a property exists or not */
ZEND_METHOD(ReflectionClass, hasProperty)
{
	reflection_object *intern;
	zend_property_info *property_info;
	zend_class_entry *ce;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if ((property_info = zend_hash_find_ptr(&ce->properties_info, name)) != NULL) {
		if ((property_info->flags & ZEND_ACC_PRIVATE) && property_info->ce != ce) {
			RETURN_FALSE;
		}
		RETURN_TRUE;
	} else {
		if (Z_TYPE(intern->obj) != IS_UNDEF) {
			if (Z_OBJ_HANDLER(intern->obj, has_property)(Z_OBJ(intern->obj), name, 2, NULL)) {
				RETURN_TRUE;
			}
		}
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns the class' property specified by its name */
ZEND_METHOD(ReflectionClass, getProperty)
{
	reflection_object *intern;
	zend_class_entry *ce, *ce2;
	zend_property_info *property_info;
	zend_string *name, *classname;
	char *tmp, *str_name;
	size_t classname_len, str_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if ((property_info = zend_hash_find_ptr(&ce->properties_info, name)) != NULL) {
		if (!(property_info->flags & ZEND_ACC_PRIVATE) || property_info->ce == ce) {
			reflection_property_factory(ce, name, property_info, return_value);
			return;
		}
	} else if (Z_TYPE(intern->obj) != IS_UNDEF) {
		/* Check for dynamic properties */
		if (zend_hash_exists(Z_OBJ_HT(intern->obj)->get_properties(Z_OBJ(intern->obj)), name)) {
			reflection_property_factory(ce, name, NULL, return_value);
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
				zend_throw_exception_ex(reflection_exception_ptr, -1, "Class \"%s\" does not exist", ZSTR_VAL(classname));
			}
			zend_string_release_ex(classname, 0);
			RETURN_THROWS();
		}
		zend_string_release_ex(classname, 0);

		if (!instanceof_function(ce, ce2)) {
			zend_throw_exception_ex(reflection_exception_ptr, -1, "Fully qualified property name %s::$%s does not specify a base class of %s", ZSTR_VAL(ce2->name), str_name, ZSTR_VAL(ce->name));
			RETURN_THROWS();
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
	zend_throw_exception_ex(reflection_exception_ptr, 0, "Property %s::$%s does not exist", ZSTR_VAL(ce->name), str_name);
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
		reflection_property_factory(ce, key, pptr, &property);
		add_next_index_zval(retval, &property);
	}
}
/* }}} */

/* {{{ _adddynproperty */
static void _adddynproperty(zval *ptr, zend_string *key, zend_class_entry *ce, zval *retval)
{
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

	reflection_property_factory(ce, key, NULL, &property);
	add_next_index_zval(retval, &property);
}
/* }}} */

/* {{{ Returns an array of this class' properties */
ZEND_METHOD(ReflectionClass, getProperties)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *key;
	zend_property_info *prop_info;
	zend_long filter;
	zend_bool filter_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &filter, &filter_is_null) == FAILURE) {
		RETURN_THROWS();
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
		HashTable *properties = Z_OBJ_HT(intern->obj)->get_properties(Z_OBJ(intern->obj));
		zval *prop;
		ZEND_HASH_FOREACH_STR_KEY_VAL(properties, key, prop) {
			_adddynproperty(prop, key, ce, return_value);
		} ZEND_HASH_FOREACH_END();
	}
}
/* }}} */

/* {{{ Returns whether a constant exists or not */
ZEND_METHOD(ReflectionClass, hasConstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);
	if (zend_hash_exists(&ce->constants_table, name)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns an associative array containing this class' constants and their values */
ZEND_METHOD(ReflectionClass, getConstants)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *key;
	zend_class_constant *constant;
	zval val;
	zend_long filter;
	zend_bool filter_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &filter, &filter_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (filter_is_null) {
		filter = ZEND_ACC_PPP_MASK;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, key, constant) {
		if (UNEXPECTED(zval_update_constant_ex(&constant->value, ce) != SUCCESS)) {
			zend_array_destroy(Z_ARRVAL_P(return_value));
			RETURN_NULL();
		}

		if (Z_ACCESS_FLAGS(constant->value) & filter) {
			ZVAL_COPY_OR_DUP(&val, &constant->value);
			zend_hash_add_new(Z_ARRVAL_P(return_value), key, &val);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns an associative array containing this class' constants as ReflectionClassConstant objects */
ZEND_METHOD(ReflectionClass, getReflectionConstants)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_string *name;
	zend_class_constant *constant;
	zend_long filter;
	zend_bool filter_is_null = 1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l!", &filter, &filter_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	if (filter_is_null) {
		filter = ZEND_ACC_PPP_MASK;
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(&ce->constants_table, name, constant) {
		if (Z_ACCESS_FLAGS(constant->value) & filter) {
			zval class_const;
			reflection_class_constant_factory(name, constant, &class_const);
			zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &class_const);
		}
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns the class' constant specified by its name */
ZEND_METHOD(ReflectionClass, getConstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_class_constant *c;
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns the class' constant as ReflectionClassConstant objects */
ZEND_METHOD(ReflectionClass, getReflectionConstant)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zend_class_constant *constant;
	zend_string *name;

	GET_REFLECTION_OBJECT_PTR(ce);
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		RETURN_THROWS();
	}

	if ((constant = zend_hash_find_ptr(&ce->constants_table, name)) == NULL) {
		RETURN_FALSE;
	}
	reflection_class_constant_factory(name, constant, return_value);
}
/* }}} */

/* {{{ _class_check_flag */
static void _class_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETVAL_BOOL(ce->ce_flags & mask);
}
/* }}} */

/* {{{ Returns whether this class is instantiable */
ZEND_METHOD(ReflectionClass, isInstantiable)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns whether this class is cloneable */
ZEND_METHOD(ReflectionClass, isCloneable)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval obj;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns whether this is an interface or a class */
ZEND_METHOD(ReflectionClass, isInterface)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_INTERFACE);
}
/* }}} */

/* {{{ Returns whether this is a trait */
ZEND_METHOD(ReflectionClass, isTrait)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_TRAIT);
}
/* }}} */

/* {{{ Returns whether this class is final */
ZEND_METHOD(ReflectionClass, isFinal)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_FINAL);
}
/* }}} */

/* {{{ Returns whether this class is abstract */
ZEND_METHOD(ReflectionClass, isAbstract)
{
	_class_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
}
/* }}} */

/* {{{ Returns a bitfield of the access modifiers for this class */
ZEND_METHOD(ReflectionClass, getModifiers)
{
	reflection_object *intern;
	zend_class_entry *ce;
	uint32_t keep_flags = ZEND_ACC_FINAL
		| ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_LONG((ce->ce_flags & keep_flags));
}
/* }}} */

/* {{{ Returns whether the given object is an instance of this class */
ZEND_METHOD(ReflectionClass, isInstance)
{
	reflection_object *intern;
	zend_class_entry *ce;
	zval *object;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &object) == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);
	RETURN_BOOL(instanceof_function(Z_OBJCE_P(object), ce));
}
/* }}} */

/* {{{ Returns an instance of this class */
ZEND_METHOD(ReflectionClass, newInstance)
{
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
		zval *params;
		int num_args;
		HashTable *named_params;

		if (!(constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Access to non-public constructor of class %s", ZSTR_VAL(ce->name));
			zval_ptr_dtor(return_value);
			RETURN_NULL();
		}

		ZEND_PARSE_PARAMETERS_START(0, -1)
			Z_PARAM_VARIADIC_WITH_NAMED(params, num_args, named_params)
		ZEND_PARSE_PARAMETERS_END();

		zend_call_known_function(
			constructor, Z_OBJ_P(return_value), Z_OBJCE_P(return_value), NULL,
			num_args, params, named_params);

		if (EG(exception)) {
			zend_object_store_ctor_failed(Z_OBJ_P(return_value));
		}
	} else if (ZEND_NUM_ARGS()) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s does not have a constructor, so you cannot pass any constructor arguments", ZSTR_VAL(ce->name));
	}
}
/* }}} */

/* {{{ Returns an instance of this class without invoking its constructor */
ZEND_METHOD(ReflectionClass, newInstanceWithoutConstructor)
{
	reflection_object *intern;
	zend_class_entry *ce;

	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	if (ce->type == ZEND_INTERNAL_CLASS
			&& ce->create_object != NULL && (ce->ce_flags & ZEND_ACC_FINAL)) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s is an internal class marked as final that cannot be instantiated without invoking its constructor", ZSTR_VAL(ce->name));
		RETURN_THROWS();
	}

	object_init_ex(return_value, ce);
}
/* }}} */

/* {{{ Returns an instance of this class */
ZEND_METHOD(ReflectionClass, newInstanceArgs)
{
	reflection_object *intern;
	zend_class_entry *ce, *old_scope;
	int argc = 0;
	HashTable *args = NULL;
	zend_function *constructor;

	GET_REFLECTION_OBJECT_PTR(ce);

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|h", &args) == FAILURE) {
		RETURN_THROWS();
	}

	if (args) {
		argc = zend_hash_num_elements(args);
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
		if (!(constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Access to non-public constructor of class %s", ZSTR_VAL(ce->name));
			zval_ptr_dtor(return_value);
			RETURN_NULL();
		}

		zend_call_known_function(
			constructor, Z_OBJ_P(return_value), Z_OBJCE_P(return_value), NULL, 0, NULL, args);

		if (EG(exception)) {
			zend_object_store_ctor_failed(Z_OBJ_P(return_value));
		}
	} else if (argc) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "Class %s does not have a constructor, so you cannot pass any constructor arguments", ZSTR_VAL(ce->name));
	}
}
/* }}} */

/* {{{ Returns an array of interfaces this class implements */
ZEND_METHOD(ReflectionClass, getInterfaces)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an array of names of interfaces this class implements */
ZEND_METHOD(ReflectionClass, getInterfaceNames)
{
	reflection_object *intern;
	zend_class_entry *ce;
	uint32_t i;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an array of traits used by this class */
ZEND_METHOD(ReflectionClass, getTraits)
{
	reflection_object *intern;
	zend_class_entry *ce;
	uint32_t i;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an array of names of traits used by this class */
ZEND_METHOD(ReflectionClass, getTraitNames)
{
	reflection_object *intern;
	zend_class_entry *ce;
	uint32_t i;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an array of trait aliases */
ZEND_METHOD(ReflectionClass, getTraitAliases)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns the class' parent class, or, if none exists, FALSE */
ZEND_METHOD(ReflectionClass, getParentClass)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->parent) {
		zend_reflection_class_factory(ce->parent, return_value);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns whether this class is a subclass of another class */
ZEND_METHOD(ReflectionClass, isSubclassOf)
{
	reflection_object *intern, *argument;
	zend_class_entry *ce, *class_ce;
	zend_string *class_str;
	zend_object *class_obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(class_obj, reflection_class_ptr, class_str)
	ZEND_PARSE_PARAMETERS_END();

	if (class_obj) {
		argument = reflection_object_from_obj(class_obj);
		if (argument->ptr == NULL) {
			zend_throw_error(NULL, "Internal error: Failed to retrieve the argument's reflection object");
			RETURN_THROWS();
		}

		class_ce = argument->ptr;
	} else {
		if ((class_ce = zend_lookup_class(class_str)) == NULL) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Class \"%s\" does not exist", ZSTR_VAL(class_str));
			RETURN_THROWS();
		}
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_BOOL((ce != class_ce && instanceof_function(ce, class_ce)));
}
/* }}} */

/* {{{ Returns whether this class is a subclass of another class */
ZEND_METHOD(ReflectionClass, implementsInterface)
{
	reflection_object *intern, *argument;
	zend_string *interface_str;
	zend_class_entry *ce, *interface_ce;
	zend_object *interface_obj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJ_OF_CLASS_OR_STR(interface_obj, reflection_class_ptr, interface_str)
	ZEND_PARSE_PARAMETERS_END();

	if (interface_obj) {
		argument = reflection_object_from_obj(interface_obj);
		if (argument->ptr == NULL) {
			zend_throw_error(NULL, "Internal error: Failed to retrieve the argument's reflection object");
			RETURN_THROWS();
		}

		interface_ce = argument->ptr;
	} else {
		if ((interface_ce = zend_lookup_class(interface_str)) == NULL) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Interface \"%s\" does not exist", ZSTR_VAL(interface_str));
			RETURN_THROWS();
		}
	}

	if (!(interface_ce->ce_flags & ZEND_ACC_INTERFACE)) {
		zend_throw_exception_ex(reflection_exception_ptr, 0, "%s is not an interface", ZSTR_VAL(interface_ce->name));
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	RETURN_BOOL(instanceof_function(ce, interface_ce));
}
/* }}} */

/* {{{ Returns whether this class is iterable (can be used inside foreach) */
ZEND_METHOD(ReflectionClass, isIterable)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if (ce->ce_flags & (ZEND_ACC_INTERFACE | ZEND_ACC_IMPLICIT_ABSTRACT_CLASS |
	                    ZEND_ACC_TRAIT     | ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		RETURN_FALSE;
	}

	RETURN_BOOL(ce->get_iterator || instanceof_function(ce, zend_ce_traversable));
}
/* }}} */

/* {{{ Returns NULL or the extension the class belongs to */
ZEND_METHOD(ReflectionClass, getExtension)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if ((ce->type == ZEND_INTERNAL_CLASS) && ce->info.internal.module) {
		reflection_extension_factory(return_value, ce->info.internal.module->name);
	}
}
/* }}} */

/* {{{ Returns false or the name of the extension the class belongs to */
ZEND_METHOD(ReflectionClass, getExtensionName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	if ((ce->type == ZEND_INTERNAL_CLASS) && ce->info.internal.module) {
		RETURN_STRING(ce->info.internal.module->name);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ Returns whether this class is defined in namespace */
ZEND_METHOD(ReflectionClass, inNamespace)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	zend_string *name = ce->name;
	const char *backslash = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	RETURN_BOOL(backslash && backslash > ZSTR_VAL(name));
}
/* }}} */

/* {{{ Returns the name of namespace where this class is defined */
ZEND_METHOD(ReflectionClass, getNamespaceName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	zend_string *name = ce->name;
	const char *backslash = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	if (backslash && backslash > ZSTR_VAL(name)) {
		RETURN_STRINGL(ZSTR_VAL(name), backslash - ZSTR_VAL(name));
	}
	RETURN_EMPTY_STRING();
}
/* }}} */

/* {{{ Returns the short name of the class (without namespace part) */
ZEND_METHOD(ReflectionClass, getShortName)
{
	reflection_object *intern;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ce);

	zend_string *name = ce->name;
	const char *backslash = zend_memrchr(ZSTR_VAL(name), '\\', ZSTR_LEN(name));
	if (backslash && backslash > ZSTR_VAL(name)) {
		RETURN_STRINGL(backslash + 1, ZSTR_LEN(name) - (backslash - ZSTR_VAL(name) + 1));
	}
	RETURN_STR_COPY(name);
}
/* }}} */

/* {{{ Constructor. Takes an instance as an argument */
ZEND_METHOD(ReflectionObject, __construct)
{
	reflection_class_object_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Constructor. Throws an Exception in case the given property does not exist */
ZEND_METHOD(ReflectionProperty, __construct)
{
	zend_string *classname_str;
	zend_object *classname_obj;
	zend_string *name;
	int dynam_prop = 0;
	zval *object;
	reflection_object *intern;
	zend_class_entry *ce;
	zend_property_info *property_info = NULL;
	property_reference *reference;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_OBJ_OR_STR(classname_obj, classname_str)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	if (classname_obj) {
		ce = classname_obj->ce;
	} else {
		if ((ce = zend_lookup_class(classname_str)) == NULL) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Class \"%s\" does not exist", ZSTR_VAL(classname_str));
			RETURN_THROWS();
		}
	}

	property_info = zend_hash_find_ptr(&ce->properties_info, name);
	if (property_info == NULL
	 || ((property_info->flags & ZEND_ACC_PRIVATE)
	  && property_info->ce != ce)) {
		/* Check for dynamic properties */
		if (property_info == NULL && classname_obj) {
			if (zend_hash_exists(classname_obj->handlers->get_properties(classname_obj), name)) {
				dynam_prop = 1;
			}
		}
		if (dynam_prop == 0) {
			zend_throw_exception_ex(reflection_exception_ptr, 0, "Property %s::$%s does not exist", ZSTR_VAL(ce->name), ZSTR_VAL(name));
			RETURN_THROWS();
		}
	}

	ZVAL_STR_COPY(reflection_prop_name(object), name);
	if (dynam_prop == 0) {
		ZVAL_STR_COPY(reflection_prop_class(object), property_info->ce->name);
	} else {
		ZVAL_STR_COPY(reflection_prop_class(object), ce->name);
	}

	reference = (property_reference*) emalloc(sizeof(property_reference));
	reference->prop = dynam_prop ? NULL : property_info;
	reference->unmangled_name = zend_string_copy(name);
	intern->ptr = reference;
	intern->ref_type = REF_TYPE_PROPERTY;
	intern->ce = ce;
	intern->ignore_visibility = 0;
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionProperty, __toString)
{
	reflection_object *intern;
	property_reference *ref;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	_property_string(&str, ref->prop, ZSTR_VAL(ref->unmangled_name), "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ Returns the class' name */
ZEND_METHOD(ReflectionProperty, getName)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_STR_COPY(ref->unmangled_name);
}
/* }}} */

static void _property_check_flag(INTERNAL_FUNCTION_PARAMETERS, int mask) /* {{{ */
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(prop_get_flags(ref) & mask);
}
/* }}} */

/* {{{ Returns whether this property is public */
ZEND_METHOD(ReflectionProperty, isPublic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PUBLIC);
}
/* }}} */

/* {{{ Returns whether this property is private */
ZEND_METHOD(ReflectionProperty, isPrivate)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PRIVATE);
}
/* }}} */

/* {{{ Returns whether this property is protected */
ZEND_METHOD(ReflectionProperty, isProtected)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROTECTED);
}
/* }}} */

/* {{{ Returns whether this property is static */
ZEND_METHOD(ReflectionProperty, isStatic)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_STATIC);
}
/* }}} */

/* {{{ Returns whether this property is default (declared at compilation time). */
ZEND_METHOD(ReflectionProperty, isDefault)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	RETURN_BOOL(ref->prop != NULL);
}
/* }}} */

/* {{{ Returns whether this property has been promoted from a constructor */
ZEND_METHOD(ReflectionProperty, isPromoted)
{
	_property_check_flag(INTERNAL_FUNCTION_PARAM_PASSTHRU, ZEND_ACC_PROMOTED);
}
/* }}} */

/* {{{ Returns a bitfield of the access modifiers for this property */
ZEND_METHOD(ReflectionProperty, getModifiers)
{
	reflection_object *intern;
	property_reference *ref;
	uint32_t keep_flags = ZEND_ACC_PPP_MASK | ZEND_ACC_STATIC;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	RETURN_LONG(prop_get_flags(ref) & keep_flags);
}
/* }}} */

/* {{{ Returns this property's value */
ZEND_METHOD(ReflectionProperty, getValue)
{
	reflection_object *intern;
	property_reference *ref;
	zval *object = NULL;
	zval *member_p = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|o!", &object) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(prop_get_flags(ref) & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public property %s::$%s",
			ZSTR_VAL(intern->ce->name), ZSTR_VAL(ref->unmangled_name));
		RETURN_THROWS();
	}

	if (prop_get_flags(ref) & ZEND_ACC_STATIC) {
		member_p = zend_read_static_property_ex(intern->ce, ref->unmangled_name, 0);
		if (member_p) {
			ZVAL_COPY_DEREF(return_value, member_p);
		}
	} else {
		zval rv;

		if (!object) {
			zend_argument_type_error(1, "must be provided for instance properties");
			RETURN_THROWS();
		}

		/* TODO: Should this always use intern->ce? */
		if (!instanceof_function(Z_OBJCE_P(object), ref->prop ? ref->prop->ce : intern->ce)) {
			_DO_THROW("Given object is not an instance of the class this property was declared in");
			RETURN_THROWS();
		}

		member_p = zend_read_property_ex(intern->ce, Z_OBJ_P(object), ref->unmangled_name, 0, &rv);
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

/* {{{ Sets this property's value */
ZEND_METHOD(ReflectionProperty, setValue)
{
	reflection_object *intern;
	property_reference *ref;
	zval *object;
	zval *value;
	zval *tmp;

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(prop_get_flags(ref) & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public property %s::$%s",
			ZSTR_VAL(intern->ce->name), ZSTR_VAL(ref->unmangled_name));
		RETURN_THROWS();
	}

	if (prop_get_flags(ref) & ZEND_ACC_STATIC) {
		if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "z", &value) == FAILURE) {
			if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &tmp, &value) == FAILURE) {
				RETURN_THROWS();
			}
		}

		zend_update_static_property_ex(intern->ce, ref->unmangled_name, value);
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "oz", &object, &value) == FAILURE) {
			RETURN_THROWS();
		}

		zend_update_property_ex(intern->ce, Z_OBJ_P(object), ref->unmangled_name, value);
	}
}
/* }}} */

/* {{{ Returns this property's value */
ZEND_METHOD(ReflectionProperty, isInitialized)
{
	reflection_object *intern;
	property_reference *ref;
	zval *object = NULL;
	zval *member_p = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|o!", &object) == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!(prop_get_flags(ref) & ZEND_ACC_PUBLIC) && intern->ignore_visibility == 0) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot access non-public property %s::$%s",
			ZSTR_VAL(intern->ce->name), ZSTR_VAL(ref->unmangled_name));
		RETURN_THROWS();
	}

	if (prop_get_flags(ref) & ZEND_ACC_STATIC) {
		member_p = zend_read_static_property_ex(intern->ce, ref->unmangled_name, 1);
		if (member_p) {
			RETURN_BOOL(!Z_ISUNDEF_P(member_p));
		}
		RETURN_FALSE;
	} else {
		zend_class_entry *old_scope;
		int retval;

		if (!object) {
			zend_argument_type_error(1, "must be provided for instance properties");
			RETURN_THROWS();
		}

		/* TODO: Should this always use intern->ce? */
		if (!instanceof_function(Z_OBJCE_P(object), ref->prop ? ref->prop->ce : intern->ce)) {
			_DO_THROW("Given object is not an instance of the class this property was declared in");
			RETURN_THROWS();
		}

		old_scope = EG(fake_scope);
		EG(fake_scope) = intern->ce;
		retval = Z_OBJ_HT_P(object)->has_property(Z_OBJ_P(object), ref->unmangled_name, ZEND_PROPERTY_EXISTS, NULL);
		EG(fake_scope) = old_scope;

		RETVAL_BOOL(retval);
	}
}
/* }}} */

/* {{{ Get the declaring class */
ZEND_METHOD(ReflectionProperty, getDeclaringClass)
{
	reflection_object *intern;
	property_reference *ref;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);

	ce = ref->prop ? ref->prop->ce : intern->ce;
	zend_reflection_class_factory(ce, return_value);
}
/* }}} */

/* {{{ Returns the doc comment for this property */
ZEND_METHOD(ReflectionProperty, getDocComment)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(ref);
	if (ref->prop && ref->prop->doc_comment) {
		RETURN_STR_COPY(ref->prop->doc_comment);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ Returns the attributes of this property */
ZEND_METHOD(ReflectionProperty, getAttributes)
{
	reflection_object *intern;
	property_reference *ref;

	GET_REFLECTION_OBJECT_PTR(ref);

	reflect_attributes(INTERNAL_FUNCTION_PARAM_PASSTHRU,
		ref->prop->attributes, 0, ref->prop->ce, ZEND_ATTRIBUTE_TARGET_PROPERTY,
		ref->prop->ce->type == ZEND_USER_CLASS ? ref->prop->ce->info.user.filename : NULL);
}
/* }}} */

/* {{{ Sets whether non-public properties can be requested */
ZEND_METHOD(ReflectionProperty, setAccessible)
{
	reflection_object *intern;
	zend_bool visible;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "b", &visible) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_REFLECTION_P(ZEND_THIS);

	intern->ignore_visibility = visible;
}
/* }}} */

/* {{{ Returns the type associated with the property */
ZEND_METHOD(ReflectionProperty, getType)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	if (!ref->prop || !ZEND_TYPE_IS_SET(ref->prop->type)) {
		RETURN_NULL();
	}

	reflection_type_factory(ref->prop->type, return_value, 1);
}
/* }}} */

/* {{{ Returns whether property has a type */
ZEND_METHOD(ReflectionProperty, hasType)
{
	reflection_object *intern;
	property_reference *ref;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	RETVAL_BOOL(ref->prop && ZEND_TYPE_IS_SET(ref->prop->type));
}
/* }}} */

/* {{{ Returns whether property has a default value */
ZEND_METHOD(ReflectionProperty, hasDefaultValue)
{
	reflection_object *intern;
	property_reference *ref;
	zend_property_info *prop_info;
	zval *prop;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	prop_info = ref->prop;

	if (prop_info == NULL) {
		RETURN_FALSE;
	}

	prop = property_get_default(prop_info);
	RETURN_BOOL(!Z_ISUNDEF_P(prop));
}
/* }}} */

/* {{{ Returns the default value of a property */
ZEND_METHOD(ReflectionProperty, getDefaultValue)
{
	reflection_object *intern;
	property_reference *ref;
	zend_property_info *prop_info;
	zval *prop;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(ref);

	prop_info = ref->prop;

	if (prop_info == NULL) {
		return; // throw exception?
	}

	prop = property_get_default(prop_info);
	if (Z_ISUNDEF_P(prop)) {
		return;
	}

	/* copy: enforce read only access */
	ZVAL_DEREF(prop);
	ZVAL_COPY_OR_DUP(return_value, prop);

	/* this is necessary to make it able to work with default array
	* properties, returned to user */
	if (Z_TYPE_P(return_value) == IS_CONSTANT_AST) {
		if (UNEXPECTED(zval_update_constant_ex(return_value, prop_info->ce) != SUCCESS)) {
			RETURN_THROWS();
		}
	}
}
/* }}} */

/* {{{ Constructor. Throws an Exception in case the given extension does not exist */
ZEND_METHOD(ReflectionExtension, __construct)
{
	zval *object;
	char *lcname;
	reflection_object *intern;
	zend_module_entry *module;
	char *name_str;
	size_t name_len;
	ALLOCA_FLAG(use_heap)

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name_str, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);
	lcname = do_alloca(name_len + 1, use_heap);
	zend_str_tolower_copy(lcname, name_str, name_len);
	if ((module = zend_hash_str_find_ptr(&module_registry, lcname, name_len)) == NULL) {
		free_alloca(lcname, use_heap);
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Extension \"%s\" does not exist", name_str);
		RETURN_THROWS();
	}
	free_alloca(lcname, use_heap);
	ZVAL_STRING(reflection_prop_name(object), module->name);
	intern->ptr = module;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = NULL;
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionExtension, __toString)
{
	reflection_object *intern;
	zend_module_entry *module;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(module);
	_extension_string(&str, module, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ Returns this extension's name */
ZEND_METHOD(ReflectionExtension, getName)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(module);
	RETURN_STRING(module->name);
}
/* }}} */

/* {{{ Returns this extension's version */
ZEND_METHOD(ReflectionExtension, getVersion)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an array of this extension's functions */
ZEND_METHOD(ReflectionExtension, getFunctions)
{
	reflection_object *intern;
	zend_module_entry *module;
	zval function;
	zend_function *fptr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an associative array containing this extension's constants and their values */
ZEND_METHOD(ReflectionExtension, getConstants)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_constant *constant;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an associative array containing this extension's INI entries and their values */
ZEND_METHOD(ReflectionExtension, getINIEntries)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_ini_entry *ini_entry;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Returns an array containing ReflectionClass objects for all classes of this extension */
ZEND_METHOD(ReflectionExtension, getClasses)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_string *key;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(EG(class_table), key, ce) {
		add_extension_class(ce, key, return_value, module, 1);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns an array containing all names of all classes of this extension */
ZEND_METHOD(ReflectionExtension, getClassNames)
{
	reflection_object *intern;
	zend_module_entry *module;
	zend_string *key;
	zend_class_entry *ce;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(module);

	array_init(return_value);
	ZEND_HASH_FOREACH_STR_KEY_PTR(EG(class_table), key, ce) {
		add_extension_class(ce, key, return_value, module, 0);
	} ZEND_HASH_FOREACH_END();
}
/* }}} */

/* {{{ Returns an array containing all names of all extensions this extension depends on */
ZEND_METHOD(ReflectionExtension, getDependencies)
{
	reflection_object *intern;
	zend_module_entry *module;
	const zend_module_dep *dep;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
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

/* {{{ Prints phpinfo block for the extension */
ZEND_METHOD(ReflectionExtension, info)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(module);

	php_info_print_module(module);
}
/* }}} */

/* {{{ Returns whether this extension is persistent */
ZEND_METHOD(ReflectionExtension, isPersistent)
{
	reflection_object *intern;
    zend_module_entry *module;

    if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(module);

	RETURN_BOOL(module->type == MODULE_PERSISTENT);
}
/* }}} */

/* {{{ Returns whether this extension is temporary */
ZEND_METHOD(ReflectionExtension, isTemporary)
{
	reflection_object *intern;
	zend_module_entry *module;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(module);

	RETURN_BOOL(module->type == MODULE_TEMPORARY);
}
/* }}} */

/* {{{ Constructor. Throws an Exception in case the given Zend extension does not exist */
ZEND_METHOD(ReflectionZendExtension, __construct)
{
	zval *object;
	reflection_object *intern;
	zend_extension *extension;
	char *name_str;
	size_t name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name_str, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	object = ZEND_THIS;
	intern = Z_REFLECTION_P(object);

	extension = zend_get_extension(name_str);
	if (!extension) {
		zend_throw_exception_ex(reflection_exception_ptr, 0,
				"Zend Extension \"%s\" does not exist", name_str);
		RETURN_THROWS();
	}
	ZVAL_STRING(reflection_prop_name(object), extension->name);
	intern->ptr = extension;
	intern->ref_type = REF_TYPE_OTHER;
	intern->ce = NULL;
}
/* }}} */

/* {{{ Returns a string representation */
ZEND_METHOD(ReflectionZendExtension, __toString)
{
	reflection_object *intern;
	zend_extension *extension;
	smart_str str = {0};

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(extension);
	_zend_extension_string(&str, extension, "");
	RETURN_STR(smart_str_extract(&str));
}
/* }}} */

/* {{{ Returns the name of this Zend extension */
ZEND_METHOD(ReflectionZendExtension, getName)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	RETURN_STRING(extension->name);
}
/* }}} */

/* {{{ Returns the version information of this Zend extension */
ZEND_METHOD(ReflectionZendExtension, getVersion)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->version) {
		RETURN_STRING(extension->version);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Returns the name of this Zend extension's author */
ZEND_METHOD(ReflectionZendExtension, getAuthor)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->author) {
		RETURN_STRING(extension->author);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Returns this Zend extension's URL*/
ZEND_METHOD(ReflectionZendExtension, getURL)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->URL) {
		RETURN_STRING(extension->URL);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ Returns this Zend extension's copyright information */
ZEND_METHOD(ReflectionZendExtension, getCopyright)
{
	reflection_object *intern;
	zend_extension *extension;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(extension);

	if (extension->copyright) {
		RETURN_STRING(extension->copyright);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{     Dummy constructor -- always throws ReflectionExceptions. */
ZEND_METHOD(ReflectionReference, __construct)
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

/* {{{     Create ReflectionReference for array item. Returns null if not a reference. */
ZEND_METHOD(ReflectionReference, fromArrayElement)
{
	HashTable *ht;
	zval *item;
	zend_string *string_key = NULL;
	zend_long int_key = 0;
	reflection_object *intern;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_HT(ht)
		Z_PARAM_STR_OR_LONG(string_key, int_key)
	ZEND_PARSE_PARAMETERS_END();

	if (string_key) {
		item = zend_hash_find(ht, string_key);
	} else {
		item = zend_hash_index_find(ht, int_key);
	}

	if (!item) {
		_DO_THROW("Array key not found");
		RETURN_THROWS();
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

/* {{{     Returns a unique identifier for the reference.
 *     The format of the return value is unspecified and may change. */
ZEND_METHOD(ReflectionReference, getId)
{
	reflection_object *intern;
	unsigned char digest[20];
	PHP_SHA1_CTX context;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_REFLECTION_P(getThis());
	if (Z_TYPE(intern->obj) != IS_REFERENCE) {
		_DO_THROW("Corrupted ReflectionReference object");
		RETURN_THROWS();
	}

	if (!REFLECTION_G(key_initialized)) {
		if (php_random_bytes_throw(&REFLECTION_G(key_initialized), 16) == FAILURE) {
			RETURN_THROWS();
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

ZEND_METHOD(ReflectionAttribute, __construct)
{
}

ZEND_METHOD(ReflectionAttribute, __clone)
{
	/* Should never be executable */
	_DO_THROW("Cannot clone object using __clone()");
}

/* {{{ *	   Returns the name of the attribute */
ZEND_METHOD(ReflectionAttribute, getName)
{
	reflection_object *intern;
	attribute_reference *attr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(attr);

	RETURN_STR_COPY(attr->data->name);
}
/* }}} */

/* {{{ *	   Returns the target of the attribute */
ZEND_METHOD(ReflectionAttribute, getTarget)
{
	reflection_object *intern;
	attribute_reference *attr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(attr);

	RETURN_LONG(attr->target);
}
/* }}} */

/* {{{ *	   Returns true if the attribute is repeated */
ZEND_METHOD(ReflectionAttribute, isRepeated)
{
	reflection_object *intern;
	attribute_reference *attr;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(attr);

	RETURN_BOOL(zend_is_attribute_repeated(attr->attributes, attr->data));
}
/* }}} */

/* {{{ *	   Returns the arguments passed to the attribute */
ZEND_METHOD(ReflectionAttribute, getArguments)
{
	reflection_object *intern;
	attribute_reference *attr;

	zval tmp;
	uint32_t i;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}
	GET_REFLECTION_OBJECT_PTR(attr);

	array_init(return_value);

	for (i = 0; i < attr->data->argc; i++) {
		if (FAILURE == zend_get_attribute_value(&tmp, attr->data, i, attr->scope)) {
			RETURN_THROWS();
		}

		if (attr->data->args[i].name) {
			/* We ensured at compile-time that there are no duplicate parameter names. */
			zend_hash_add_new(Z_ARRVAL_P(return_value), attr->data->args[i].name, &tmp);
		} else {
			add_next_index_zval(return_value, &tmp);
		}
	}
}
/* }}} */

static int call_attribute_constructor(
	zend_attribute *attr, zend_class_entry *ce, zend_object *obj,
	zval *args, uint32_t argc, HashTable *named_params, zend_string *filename)
{
	zend_function *ctor = ce->constructor;
	zend_execute_data *prev_execute_data, dummy_frame;
	zend_function dummy_func;
	zend_op dummy_opline;
	ZEND_ASSERT(ctor != NULL);

	if (!(ctor->common.fn_flags & ZEND_ACC_PUBLIC)) {
		zend_throw_error(NULL, "Attribute constructor of class %s must be public", ZSTR_VAL(ce->name));
		return FAILURE;
	}

	if (filename) {
		/* Set up dummy call frame that makes it look like the attribute was invoked
		 * from where it occurs in the code. */
		memset(&dummy_frame, 0, sizeof(zend_execute_data));
		memset(&dummy_func, 0, sizeof(zend_function));
		memset(&dummy_opline, 0, sizeof(zend_op));

		prev_execute_data = EG(current_execute_data);
		dummy_frame.prev_execute_data = prev_execute_data;
		dummy_frame.func = &dummy_func;
		dummy_frame.opline = &dummy_opline;

		dummy_func.type = ZEND_USER_FUNCTION;
		dummy_func.common.fn_flags =
			attr->flags & ZEND_ATTRIBUTE_STRICT_TYPES ? ZEND_ACC_STRICT_TYPES : 0;
		dummy_func.op_array.filename = filename;

		dummy_opline.opcode = ZEND_DO_FCALL;
		dummy_opline.lineno = attr->lineno;

		EG(current_execute_data) = &dummy_frame;
	}

	zend_call_known_function(ctor, obj, obj->ce, NULL, argc, args, named_params);

	if (filename) {
		EG(current_execute_data) = prev_execute_data;
	}

	if (EG(exception)) {
		zend_object_store_ctor_failed(obj);
		return FAILURE;
	}

	return SUCCESS;
}

static void attribute_ctor_cleanup(
		zval *obj, zval *args, uint32_t argc, HashTable *named_params) /* {{{ */
{
	if (obj) {
		zval_ptr_dtor(obj);
	}

	if (args) {
		uint32_t i;

		for (i = 0; i < argc; i++) {
			zval_ptr_dtor(&args[i]);
		}

		efree(args);
	}

	if (named_params) {
		zend_array_destroy(named_params);
	}
}
/* }}} */

/* {{{ *	   Returns the attribute as an object */
ZEND_METHOD(ReflectionAttribute, newInstance)
{
	reflection_object *intern;
	attribute_reference *attr;
	zend_attribute *marker;

	zend_class_entry *ce;
	zval obj;

	zval *args = NULL;
	HashTable *named_params = NULL;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	GET_REFLECTION_OBJECT_PTR(attr);

	if (NULL == (ce = zend_lookup_class(attr->data->name))) {
		zend_throw_error(NULL, "Attribute class \"%s\" not found", ZSTR_VAL(attr->data->name));
		RETURN_THROWS();
	}

	if (NULL == (marker = zend_get_attribute_str(ce->attributes, ZEND_STRL("attribute")))) {
		zend_throw_error(NULL, "Attempting to use non-attribute class \"%s\" as attribute", ZSTR_VAL(attr->data->name));
		RETURN_THROWS();
	}

	if (ce->type == ZEND_USER_CLASS) {
		uint32_t flags = ZEND_ATTRIBUTE_TARGET_ALL;

		if (marker->argc > 0) {
			zval tmp;

			if (FAILURE == zend_get_attribute_value(&tmp, marker, 0, ce)) {
				RETURN_THROWS();
			}

			flags = (uint32_t) Z_LVAL(tmp);
		}

		if (!(attr->target & flags)) {
			zend_string *location = zend_get_attribute_target_names(attr->target);
			zend_string *allowed = zend_get_attribute_target_names(flags);

			zend_throw_error(NULL, "Attribute \"%s\" cannot target %s (allowed targets: %s)",
				ZSTR_VAL(attr->data->name), ZSTR_VAL(location), ZSTR_VAL(allowed)
			);

			zend_string_release(location);
			zend_string_release(allowed);

			RETURN_THROWS();
		}

		if (!(flags & ZEND_ATTRIBUTE_IS_REPEATABLE)) {
			if (zend_is_attribute_repeated(attr->attributes, attr->data)) {
				zend_throw_error(NULL, "Attribute \"%s\" must not be repeated", ZSTR_VAL(attr->data->name));
				RETURN_THROWS();
			}
		}
	}

	if (SUCCESS != object_init_ex(&obj, ce)) {
		RETURN_THROWS();
	}

	uint32_t argc = 0;
	if (attr->data->argc) {
		args = emalloc(attr->data->argc * sizeof(zval));

		for (uint32_t i = 0; i < attr->data->argc; i++) {
			zval val;
			if (FAILURE == zend_get_attribute_value(&val, attr->data, i, attr->scope)) {
				attribute_ctor_cleanup(&obj, args, i, named_params);
				RETURN_THROWS();
			}
			if (attr->data->args[i].name) {
				if (!named_params) {
					named_params = zend_new_array(0);
				}
				zend_hash_add_new(named_params, attr->data->args[i].name, &val);
			} else {
				ZVAL_COPY_VALUE(&args[i], &val);
				argc++;
			}
		}
	}

	if (ce->constructor) {
		if (FAILURE == call_attribute_constructor(attr->data, ce, Z_OBJ(obj), args, argc, named_params, attr->filename)) {
			attribute_ctor_cleanup(&obj, args, argc, named_params);
			RETURN_THROWS();
		}
	} else if (argc || named_params) {
		attribute_ctor_cleanup(&obj, args, argc, named_params);
		zend_throw_error(NULL, "Attribute class %s does not have a constructor, cannot pass arguments", ZSTR_VAL(ce->name));
		RETURN_THROWS();
	}

	attribute_ctor_cleanup(NULL, args, argc, named_params);

	RETURN_COPY_VALUE(&obj);
}

/* {{{ _reflection_write_property */
static zval *_reflection_write_property(zend_object *object, zend_string *name, zval *value, void **cache_slot)
{
	if (zend_hash_exists(&object->ce->properties_info, name)
		&& (zend_string_equals_literal(name, "name") || zend_string_equals_literal(name, "class")))
	{
		zend_throw_exception_ex(reflection_exception_ptr, 0,
			"Cannot set read-only property %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(name));
		return &EG(uninitialized_zval);
	}
	else
	{
		return zend_std_write_property(object, name, value, cache_slot);
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

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionException", class_ReflectionException_methods);
	reflection_exception_ptr = zend_register_internal_class_ex(&_reflection_entry, zend_ce_exception);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflection", class_Reflection_methods);
	reflection_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "Reflector", class_Reflector_methods);
	reflector_ptr = zend_register_internal_interface(&_reflection_entry);
	zend_class_implements(reflector_ptr, 1, zend_ce_stringable);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionFunctionAbstract", class_ReflectionFunctionAbstract_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_function_abstract_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_function_abstract_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_function_abstract_ptr, "name", sizeof("name")-1, "", ZEND_ACC_ABSTRACT);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionFunction", class_ReflectionFunction_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_function_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_function_abstract_ptr);
	zend_declare_property_string(reflection_function_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(function, "IS_DEPRECATED", ZEND_ACC_DEPRECATED);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionGenerator", class_ReflectionGenerator_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_generator_ptr = zend_register_internal_class(&_reflection_entry);
	reflection_generator_ptr->ce_flags |= ZEND_ACC_FINAL;

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionParameter", class_ReflectionParameter_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_parameter_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_parameter_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_parameter_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionType", class_ReflectionType_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_type_ptr = zend_register_internal_class(&_reflection_entry);
	reflection_type_ptr->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
	zend_class_implements(reflection_type_ptr, 1, zend_ce_stringable);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionNamedType", class_ReflectionNamedType_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_named_type_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_type_ptr);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionUnionType", class_ReflectionUnionType_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_union_type_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_type_ptr);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionMethod", class_ReflectionMethod_methods);
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

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionClass", class_ReflectionClass_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_class_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_class_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_class_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	/* IS_IMPLICIT_ABSTRACT is not longer used */
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_IMPLICIT_ABSTRACT", ZEND_ACC_IMPLICIT_ABSTRACT_CLASS);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_EXPLICIT_ABSTRACT", ZEND_ACC_EXPLICIT_ABSTRACT_CLASS);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class, "IS_FINAL", ZEND_ACC_FINAL);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionObject", class_ReflectionObject_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_object_ptr = zend_register_internal_class_ex(&_reflection_entry, reflection_class_ptr);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionProperty", class_ReflectionProperty_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_property_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_property_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_property_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(reflection_property_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_STATIC", ZEND_ACC_STATIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PUBLIC", ZEND_ACC_PUBLIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PROTECTED", ZEND_ACC_PROTECTED);
	REGISTER_REFLECTION_CLASS_CONST_LONG(property, "IS_PRIVATE", ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionClassConstant", class_ReflectionClassConstant_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_class_constant_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_class_constant_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_class_constant_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);
	zend_declare_property_string(reflection_class_constant_ptr, "class", sizeof("class")-1, "", ZEND_ACC_PUBLIC);

	REGISTER_REFLECTION_CLASS_CONST_LONG(class_constant, "IS_PUBLIC", ZEND_ACC_PUBLIC);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class_constant, "IS_PROTECTED", ZEND_ACC_PROTECTED);
	REGISTER_REFLECTION_CLASS_CONST_LONG(class_constant, "IS_PRIVATE", ZEND_ACC_PRIVATE);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionExtension", class_ReflectionExtension_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_extension_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_extension_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_extension_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionZendExtension", class_ReflectionZendExtension_methods);
	reflection_init_class_handlers(&_reflection_entry);
	reflection_zend_extension_ptr = zend_register_internal_class(&_reflection_entry);
	zend_class_implements(reflection_zend_extension_ptr, 1, reflector_ptr);
	zend_declare_property_string(reflection_zend_extension_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionReference", class_ReflectionReference_methods);
	reflection_init_class_handlers(&_reflection_entry);
	_reflection_entry.ce_flags |= ZEND_ACC_FINAL;
	reflection_reference_ptr = zend_register_internal_class(&_reflection_entry);

	INIT_CLASS_ENTRY(_reflection_entry, "ReflectionAttribute", class_ReflectionAttribute_methods);
	reflection_init_class_handlers(&_reflection_entry);
	_reflection_entry.ce_flags |= ZEND_ACC_FINAL;
	reflection_attribute_ptr = zend_register_internal_class(&_reflection_entry);

	REGISTER_REFLECTION_CLASS_CONST_LONG(attribute, "IS_INSTANCEOF", REFLECTION_ATTRIBUTE_IS_INSTANCEOF);

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
	NULL,
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
