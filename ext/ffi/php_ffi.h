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
   | Author: Dmitry Stogov <dmitry@zend.com>                              |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_FFI_H
#define PHP_FFI_H

extern zend_module_entry ffi_module_entry;
#define phpext_ffi_ptr &ffi_module_entry

typedef enum _zend_ffi_api_restriction {
	ZEND_FFI_DISABLED = 0,  /* completely disabled */
	ZEND_FFI_ENABLED = 1,   /* enabled everywhere */
	ZEND_FFI_PRELOAD = 2,   /* enabled only in preloaded scripts and CLI */
} zend_ffi_api_restriction;

typedef struct _zend_ffi_type  zend_ffi_type;

ZEND_BEGIN_MODULE_GLOBALS(ffi)
	zend_ffi_api_restriction restriction;
	zend_bool is_cli;

	/* predefined ffi_types */
	HashTable types;

	/* preloading */
	char *preload;
	HashTable *scopes;           /* list of preloaded scopes */

	/* callbacks */
	HashTable *callbacks;

	/* weak type references */
	HashTable *weak_types;

	/* ffi_parser */
	JMP_BUF	bailout;
	unsigned const char *buf;
	unsigned const char *end;
	unsigned const char *pos;
	unsigned const char *text;
	int line;
	HashTable *symbols;
	HashTable *tags;
	zend_bool allow_vla;
	zend_bool attribute_parsing;
	zend_bool persistent;
	uint32_t  default_type_attr;
ZEND_END_MODULE_GLOBALS(ffi)

ZEND_EXTERN_MODULE_GLOBALS(ffi)

#ifdef PHP_WIN32
# define PHP_FFI_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_FFI_API __attribute__ ((visibility("default")))
#else
# define PHP_FFI_API
#endif

#define FFI_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(ffi, v)

#define ZEND_FFI_DCL_VOID            (1<<0)
#define ZEND_FFI_DCL_CHAR            (1<<1)
#define ZEND_FFI_DCL_SHORT           (1<<2)
#define ZEND_FFI_DCL_INT             (1<<3)
#define ZEND_FFI_DCL_LONG            (1<<4)
#define ZEND_FFI_DCL_LONG_LONG       (1<<5)
#define ZEND_FFI_DCL_FLOAT           (1<<6)
#define ZEND_FFI_DCL_DOUBLE          (1<<7)
#define ZEND_FFI_DCL_SIGNED          (1<<8)
#define ZEND_FFI_DCL_UNSIGNED        (1<<9)
#define ZEND_FFI_DCL_BOOL            (1<<10)
#define ZEND_FFI_DCL_COMPLEX         (1<<11)

#define ZEND_FFI_DCL_STRUCT          (1<<12)
#define ZEND_FFI_DCL_UNION           (1<<13)
#define ZEND_FFI_DCL_ENUM            (1<<14)
#define ZEND_FFI_DCL_TYPEDEF_NAME    (1<<15)

#define ZEND_FFI_DCL_TYPE_SPECIFIERS \
	(ZEND_FFI_DCL_VOID|ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT \
	|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG \
	|ZEND_FFI_DCL_FLOAT|ZEND_FFI_DCL_DOUBLE|ZEND_FFI_DCL_SIGNED \
	|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_BOOL|ZEND_FFI_DCL_COMPLEX \
	|ZEND_FFI_DCL_STRUCT|ZEND_FFI_DCL_UNION|ZEND_FFI_DCL_ENUM \
	|ZEND_FFI_DCL_TYPEDEF_NAME)

#define ZEND_FFI_DCL_TYPEDEF         (1<<16)
#define ZEND_FFI_DCL_EXTERN          (1<<17)
#define ZEND_FFI_DCL_STATIC          (1<<18)
#define ZEND_FFI_DCL_AUTO            (1<<19)
#define ZEND_FFI_DCL_REGISTER        (1<<20)

#define ZEND_FFI_DCL_STORAGE_CLASS \
	(ZEND_FFI_DCL_TYPEDEF|ZEND_FFI_DCL_EXTERN|ZEND_FFI_DCL_STATIC \
	|ZEND_FFI_DCL_AUTO|ZEND_FFI_DCL_REGISTER)

#define ZEND_FFI_DCL_CONST           (1<<21)
#define ZEND_FFI_DCL_RESTRICT        (1<<22)
#define ZEND_FFI_DCL_VOLATILE        (1<<23)
#define ZEND_FFI_DCL_ATOMIC          (1<<24)

#define ZEND_FFI_DCL_TYPE_QUALIFIERS \
	(ZEND_FFI_DCL_CONST|ZEND_FFI_DCL_RESTRICT|ZEND_FFI_DCL_VOLATILE \
	|ZEND_FFI_DCL_ATOMIC)

#define ZEND_FFI_DCL_INLINE          (1<<25)
#define ZEND_FFI_DCL_NO_RETURN       (1<<26)

#define ZEND_FFI_ABI_DEFAULT        0

#define ZEND_FFI_ABI_CDECL          1  // FFI_DEFAULT_ABI
#define ZEND_FFI_ABI_FASTCALL       2  // FFI_FASTCALL
#define ZEND_FFI_ABI_THISCALL       3  // FFI_THISCALL
#define ZEND_FFI_ABI_STDCALL        4  // FFI_STDCALL
#define	ZEND_FFI_ABI_PASCAL         5  // FFI_PASCAL
#define	ZEND_FFI_ABI_REGISTER       6  // FFI_REGISTER
#define	ZEND_FFI_ABI_MS             7  // FFI_MS_CDECL
#define	ZEND_FFI_ABI_SYSV           8  // FFI_SYSV
#define ZEND_FFI_ABI_VECTORCALL     9  // FFI_VECTORCALL

#define ZEND_FFI_ATTR_CONST             (1<<0)
#define ZEND_FFI_ATTR_INCOMPLETE_TAG    (1<<1)
#define ZEND_FFI_ATTR_VARIADIC          (1<<2)
#define ZEND_FFI_ATTR_INCOMPLETE_ARRAY  (1<<3)
#define ZEND_FFI_ATTR_VLA               (1<<4)
#define	ZEND_FFI_ATTR_UNION             (1<<5)
#define	ZEND_FFI_ATTR_PACKED            (1<<6)
#define	ZEND_FFI_ATTR_MS_STRUCT         (1<<7)
#define	ZEND_FFI_ATTR_GCC_STRUCT        (1<<8)

#define	ZEND_FFI_ATTR_PERSISTENT        (1<<9)
#define	ZEND_FFI_ATTR_STORED            (1<<10)

#define ZEND_FFI_STRUCT_ATTRS \
	(ZEND_FFI_ATTR_UNION|ZEND_FFI_ATTR_PACKED|ZEND_FFI_ATTR_MS_STRUCT \
	|ZEND_FFI_ATTR_GCC_STRUCT)

#define ZEND_FFI_ENUM_ATTRS \
	(ZEND_FFI_ATTR_PACKED)

#define ZEND_FFI_ARRAY_ATTRS \
	(ZEND_FFI_ATTR_CONST|ZEND_FFI_ATTR_VLA|ZEND_FFI_ATTR_INCOMPLETE_ARRAY)

#define ZEND_FFI_FUNC_ATTRS \
	(ZEND_FFI_ATTR_VARIADIC)

#define ZEND_FFI_POINTER_ATTRS \
	(ZEND_FFI_ATTR_CONST)

typedef struct _zend_ffi_dcl {
	uint32_t       flags;
	uint32_t       align;
	uint16_t       attr;
	uint16_t       abi;
	zend_ffi_type *type;
} zend_ffi_dcl;

#define ZEND_FFI_ATTR_INIT {0, 0, 0, 0, NULL}

typedef enum _zend_ffi_val_kind {
	ZEND_FFI_VAL_EMPTY,
	ZEND_FFI_VAL_ERROR,
	ZEND_FFI_VAL_INT32,
	ZEND_FFI_VAL_INT64,
	ZEND_FFI_VAL_UINT32,
	ZEND_FFI_VAL_UINT64,
	ZEND_FFI_VAL_FLOAT,
	ZEND_FFI_VAL_DOUBLE,
	ZEND_FFI_VAL_LONG_DOUBLE,
	ZEND_FFI_VAL_CHAR,
	ZEND_FFI_VAL_STRING,
	ZEND_FFI_VAL_NAME, /* attribute value */
} zend_ffi_val_kind;

#ifdef HAVE_LONG_DOUBLE
typedef long double zend_ffi_double;
#else
typedef double zend_ffi_double;
#endif

typedef struct _zend_ffi_val {
	zend_ffi_val_kind   kind;
	union {
		uint64_t        u64;
		int64_t         i64;
		zend_ffi_double d;
		signed char     ch;
		struct {
			const char *str;
			size_t      len;
		};
	};
} zend_ffi_val;

int zend_ffi_parse_decl(const char *str, size_t len);
int zend_ffi_parse_type(const char *str, size_t len, zend_ffi_dcl *dcl);

/* parser callbacks */
void ZEND_NORETURN zend_ffi_parser_error(const char *msg, ...);
int zend_ffi_is_typedef_name(const char *name, size_t name_len);
void zend_ffi_resolve_typedef(const char *name, size_t name_len, zend_ffi_dcl *dcl);
void zend_ffi_resolve_const(const char *name, size_t name_len, zend_ffi_val *val);
void zend_ffi_declare_tag(const char *name, size_t name_len, zend_ffi_dcl *dcl, zend_bool incomplete);
void zend_ffi_make_enum_type(zend_ffi_dcl *dcl);
void zend_ffi_add_enum_val(zend_ffi_dcl *enum_dcl, const char *name, size_t name_len, zend_ffi_val *val, int64_t *min, int64_t *max, int64_t *last);
void zend_ffi_make_struct_type(zend_ffi_dcl *dcl);
void zend_ffi_add_field(zend_ffi_dcl *struct_dcl, const char *name, size_t name_len, zend_ffi_dcl *field_dcl);
void zend_ffi_add_anonymous_field(zend_ffi_dcl *struct_dcl, zend_ffi_dcl *field_dcl);
void zend_ffi_add_bit_field(zend_ffi_dcl *struct_dcl, const char *name, size_t name_len, zend_ffi_dcl *field_dcl, zend_ffi_val *bits);
void zend_ffi_adjust_struct_size(zend_ffi_dcl *dcl);
void zend_ffi_make_pointer_type(zend_ffi_dcl *dcl);
void zend_ffi_make_array_type(zend_ffi_dcl *dcl, zend_ffi_val *len);
void zend_ffi_make_func_type(zend_ffi_dcl *dcl, HashTable *args, zend_ffi_dcl *nested_dcl);
void zend_ffi_add_arg(HashTable **args, const char *name, size_t name_len, zend_ffi_dcl *arg_dcl);
void zend_ffi_declare(const char *name, size_t name_len, zend_ffi_dcl *dcl);
void zend_ffi_add_attribute(zend_ffi_dcl *dcl, const char *name, size_t name_len);
void zend_ffi_add_attribute_value(zend_ffi_dcl *dcl, const char *name, size_t name_len, int n, zend_ffi_val *val);
void zend_ffi_add_msvc_attribute_value(zend_ffi_dcl *dcl, const char *name, size_t name_len, zend_ffi_val *val);
void zend_ffi_set_abi(zend_ffi_dcl *dcl, uint16_t abi);
void zend_ffi_nested_declaration(zend_ffi_dcl *dcl, zend_ffi_dcl *nested_dcl);
void zend_ffi_align_as_type(zend_ffi_dcl *dcl, zend_ffi_dcl *align_dcl);
void zend_ffi_align_as_val(zend_ffi_dcl *dcl, zend_ffi_val *align_val);
void zend_ffi_validate_type_name(zend_ffi_dcl *dcl);

void zend_ffi_expr_conditional(zend_ffi_val *val, zend_ffi_val *op2, zend_ffi_val *op3);
void zend_ffi_expr_bool_or(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_bool_and(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_bw_or(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_bw_xor(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_bw_and(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_is_equal(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_is_not_equal(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_is_less(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_is_greater(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_is_less_or_equal(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_is_greater_or_equal(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_shift_left(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_shift_right(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_add(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_sub(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_mul(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_div(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_mod(zend_ffi_val *val, zend_ffi_val *op2);
void zend_ffi_expr_cast(zend_ffi_val *val, zend_ffi_dcl *dcl);
void zend_ffi_expr_plus(zend_ffi_val *val);
void zend_ffi_expr_neg(zend_ffi_val *val);
void zend_ffi_expr_bw_not(zend_ffi_val *val);
void zend_ffi_expr_bool_not(zend_ffi_val *val);
void zend_ffi_expr_sizeof_val(zend_ffi_val *val);
void zend_ffi_expr_sizeof_type(zend_ffi_val *val, zend_ffi_dcl *dcl);
void zend_ffi_expr_alignof_val(zend_ffi_val *val);
void zend_ffi_expr_alignof_type(zend_ffi_val *val, zend_ffi_dcl *dcl);

static zend_always_inline void zend_ffi_val_error(zend_ffi_val *val) /* {{{ */
{
	val->kind = ZEND_FFI_VAL_ERROR;
}
/* }}} */

void zend_ffi_val_number(zend_ffi_val *val, int base, const char *str, size_t str_len);
void zend_ffi_val_float_number(zend_ffi_val *val, const char *str, size_t str_len);
void zend_ffi_val_string(zend_ffi_val *val, const char *str, size_t str_len);
void zend_ffi_val_character(zend_ffi_val *val, const char *str, size_t str_len);

#endif	/* PHP_FFI_H */
