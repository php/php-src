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
   | Authors: Gustavo Lopes    <cataphract@php.net>                       |
   +----------------------------------------------------------------------+
 */

#include <php.h>
#include "php_sockets.h"
#include "sockaddr_conv.h"
#include "sendrecvmsg.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <limits.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <stddef.h>
#include <Zend/zend_llist.h>
#include <ext/standard/php_smart_str.h>
#ifdef ZTS
#include <TSRM/TSRM.h>
#endif

#define MAX_USER_BUFF_SIZE ((size_t)(100*1024*1024))
#define DEFAULT_BUFF_SIZE 8192
#define MAX_ARRAY_KEY_SIZE 128

#define LONG_CHECK_VALID_INT(l) \
	do { \
		if ((l) < INT_MIN && (l) > INT_MAX) { \
			php_error_docref0(NULL TSRMLS_CC, E_WARNING, "The value %ld does not fit inside " \
					"the boundaries of a native integer", (l)); \
			return; \
		} \
	} while (0)

struct err_s {
	int		has_error;
	char	*msg;
	int		level;
	int		should_free;
};

typedef struct {
	HashTable		params; /* stores pointers; has to be first */
	struct err_s	err;
	zend_llist		keys,
	/* common part to res_context ends here */
					allocations;
	php_socket		*sock;
} ser_context;

typedef struct {
	HashTable		params; /* stores pointers; has to be first */
	struct err_s	err;
	zend_llist		keys;
} res_context;

struct key_value {
	const char	*key;
	unsigned	key_size;
	void		*value;
};
#define KEY_FILL_SOCKADDR "fill_sockaddr"
#define KEY_RECVMSG_RET "recvmsg_ret"


typedef void (from_zval_write_field)(const zval *arr_value, char *field, ser_context *ctx);
typedef void (to_zval_read_field)(const char *data, zval *zv, res_context *ctx);

typedef struct {
	/* zval info */
	const char *name;
	unsigned name_size;
	int required;

	/* structure info */
	size_t field_offset; /* 0 to pass full structure, e.g. when more than
							one field is to be changed; in that case the
							callbacks need to know the name of the fields */

	/* callbacks */
	from_zval_write_field *from_zval;
	to_zval_read_field *to_zval;
} field_descriptor;

typedef struct {
	int	cmsg_level;	/* originating protocol */
	int	msg_type;	/* protocol-specific type */
} anc_reg_key;

static struct {
	int			initialized;
	HashTable	ht;
} ancillary_registry;

typedef socklen_t (*ancillary_size)(void);

typedef struct {
	socklen_t size; /* size of native structure */
	from_zval_write_field *from_array;
	to_zval_read_field *to_array;
} ancillary_reg_entry;

/* PARAMETERS */
static int param_get_bool(void *ctx, const char *key, int def)
{
	int **elem;
	if (zend_hash_find(ctx, key, strlen(key) + 1, (void**)&elem) == SUCCESS) {
		return **elem;
	} else {
		return def;
	}
}

/* FORWARD DECLARATIONS */
static ancillary_reg_entry *get_ancillary_reg_entry(int cmsg_level, int msg_type);

static inline void *accounted_emalloc(size_t alloc_size, ser_context *ctx)
{
	void *ret = emalloc(alloc_size);
	zend_llist_add_element(&ctx->allocations, &ret);
	return ret;
}

static inline void *accounted_ecalloc(size_t nmemb, size_t alloc_size, ser_context *ctx)
{
	void *ret = ecalloc(nmemb, alloc_size);
	zend_llist_add_element(&ctx->allocations, &ret);
	return ret;
}
static inline void *accounted_safe_ecalloc(size_t nmemb, size_t alloc_size, size_t offset, ser_context *ctx)
{
	void *ret = safe_emalloc(nmemb, alloc_size, offset);
	memset(ret, '\0', nmemb * alloc_size + offset);
	zend_llist_add_element(&ctx->allocations, &ret);
	return ret;
}

static void do_from_to_zval_err(struct err_s *err,
								zend_llist *keys,
								const char *what_conv,
								const char *fmt,
								va_list ap)
{
	smart_str			path = {0};
	const char			**node;
	char				*user_msg;
	int					user_msg_size;
	zend_llist_position	pos;

	if (err->has_error) {
		return;
	}

	for (node = zend_llist_get_first_ex(keys, &pos);
			node != NULL;
			node = zend_llist_get_next_ex(keys, &pos)) {
		smart_str_appends(&path, *node);
		smart_str_appends(&path, " > ");
	}

	if (path.len > 3) {
		path.len -= 3;
	}
	smart_str_0(&path);

	user_msg_size = vspprintf(&user_msg, 0, fmt, ap);

	err->has_error = 1;
	err->level = E_WARNING;
	spprintf(&err->msg, 0, "error converting %s data (path: %s): %.*s",
			what_conv,
			path.c && path.c != '\0' ? path.c : "unavailable",
			user_msg_size, user_msg);
	err->should_free = 1;

	efree(user_msg);
	smart_str_free_ex(&path, 0);
}
__attribute__ ((format (printf, 2, 3)))
static void do_from_zval_err(ser_context *ctx, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	do_from_to_zval_err(&ctx->err, &ctx->keys, "user", fmt, ap);
	va_end(ap);
}
__attribute__ ((format (printf, 2, 3)))
static void do_to_zval_err(res_context *ctx, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	do_from_to_zval_err(&ctx->err, &ctx->keys, "native", fmt, ap);
	va_end(ap);
}
static void err_msg_dispose(struct err_s *err TSRMLS_DC)
{
	if (err->msg != NULL) {
		php_error_docref0(NULL TSRMLS_CC, err->level, "%s", err->msg);
		if (err->should_free) {
			efree(err->msg);
		}
	}
}


/* Generic Aggregated conversions */
static void from_zval_write_aggregation(const zval *container,
										char *structure,
										const field_descriptor *descriptors,
										ser_context *ctx)
{
	const field_descriptor	*descr;
	zval					**elem;

	if (Z_TYPE_P(container) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
	}

	for (descr = descriptors; descr->name != NULL && !ctx->err.has_error; descr++) {
		if (zend_hash_find(Z_ARRVAL_P(container),
				descr->name, descr->name_size, (void**)&elem) == SUCCESS) {

			if (descr->from_zval == NULL) {
				do_from_zval_err(ctx, "No information on how to convert value "
						"of key '%s'", descr->name);
				break;
			}

			zend_llist_add_element(&ctx->keys, (void*)&descr->name);
			descr->from_zval(*elem, ((char*)structure) + descr->field_offset, ctx);
			zend_llist_remove_tail(&ctx->keys);

		} else if (descr->required) {
			do_from_zval_err(ctx, "The key '%s' is required", descr->name);
			break;
		}
	}
}
static void to_zval_read_aggregation(const char *structure,
									 zval *zarr, /* initialized array */
									 const field_descriptor *descriptors,
									 res_context *ctx)
{
	const field_descriptor	*descr;

	assert(Z_TYPE_P(zarr) == IS_ARRAY);
	assert(Z_ARRVAL_P(zarr) != NULL);

	for (descr = descriptors; descr->name != NULL && !ctx->err.has_error; descr++) {
		zval *new_zv;

		if (descr->to_zval == NULL) {
			do_to_zval_err(ctx, "No information on how to convert native "
					"field into value for key '%s'", descr->name);
			break;
		}

		ALLOC_INIT_ZVAL(new_zv);
		add_assoc_zval_ex(zarr, descr->name, descr->name_size, new_zv);

		zend_llist_add_element(&ctx->keys, (void*)&descr->name);
		descr->to_zval(structure + descr->field_offset, new_zv, ctx);
		zend_llist_remove_tail(&ctx->keys);
	}
}

/* CONVERSIONS for integers */
static long from_zval_integer_common(const zval *arr_value, ser_context *ctx)
{
	long ret = 0;
	zval lzval = zval_used_for_init;

	if (Z_TYPE_P(arr_value) != IS_LONG) {
		ZVAL_COPY_VALUE(&lzval, arr_value);
		zval_copy_ctor(&lzval);
		arr_value = &lzval;
	}

	switch (Z_TYPE_P(arr_value)) {
	case IS_LONG:
long_case:
		ret = Z_LVAL_P(arr_value);
		break;

	/* if not long we're operating on lzval */
	case IS_DOUBLE:
double_case:
		convert_to_long(&lzval);
		goto long_case;

	case IS_OBJECT:
	case IS_STRING: {
		long lval;
		double dval;

		convert_to_string(&lzval);

		switch (is_numeric_string(Z_STRVAL(lzval), Z_STRLEN(lzval), &lval, &dval, 0)) {
		case IS_DOUBLE:
			zval_dtor(&lzval);
			Z_TYPE(lzval) = IS_DOUBLE;
			Z_DVAL(lzval) = dval;
			goto double_case;

		case IS_LONG:
			zval_dtor(&lzval);
			Z_TYPE(lzval) = IS_LONG;
			Z_DVAL(lzval) = lval;
			goto long_case;
		}

		/* if we get here, we don't have a numeric string */
		do_from_zval_err(ctx, "expected an integer, but got a non numeric "
				"string (possibly from a converted object): '%s'", Z_STRVAL_P(arr_value));
		break;
	}

	default:
		do_from_zval_err(ctx, "%s", "expected an integer, either of a PHP "
				"integer type or of a convertible type");
		break;
	}

	zval_dtor(&lzval);

	return ret;
}
static void from_zval_write_int(const zval *arr_value, char *field, ser_context *ctx)
{
	long lval;
	int ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (lval > INT_MAX || lval < INT_MIN) {
		do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
				"for a native int");
		return;
	}

	ival = (int)lval;
	memcpy(field, &ival, sizeof(ival));
}
static void from_zval_write_unsigned(const zval *arr_value, char *field, ser_context *ctx)
{
	long lval;
	unsigned ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (sizeof(long) > sizeof(ival) && (lval < 0 || lval > UINT_MAX)) {
		do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
				"for a native unsigned int");
		return;
	}

	ival = (unsigned)lval;
	memcpy(field, &ival, sizeof(ival));
}
static void from_zval_write_uint32(const zval *arr_value, char *field, ser_context *ctx)
{
	long lval;
	uint32_t ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (sizeof(long) > sizeof(uint32_t) && (lval < 0 || lval > 0xFFFFFFFF)) {
		do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
				"for an unsigned 32-bit integer");
		return;
	}

	ival = (uint32_t)lval;
	memcpy(field, &ival, sizeof(ival));
}
static void from_zval_write_net_uint16(const zval *arr_value, char *field, ser_context *ctx)
{
	long lval;
	uint16_t ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (lval < 0 || lval > 0xFFFF) {
		do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
				"for an unsigned 16-bit integer");
		return;
	}

	ival = htons((uint16_t)lval);
	memcpy(field, &ival, sizeof(ival));
}
static void from_zval_write_sa_family(const zval *arr_value, char *field, ser_context *ctx)
{
	long lval;
	sa_family_t ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (lval < 0 || lval > (sa_family_t)-1) { /* sa_family_t is unsigned */
		do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
				"for a sa_family_t value");
		return;
	}

	ival = (sa_family_t)lval;
	memcpy(field, &ival, sizeof(ival));
}

static void to_zval_read_int(const char *data, zval *zv, res_context *ctx)
{
	int ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (long)ival);
}
static void to_zval_read_unsigned(const char *data, zval *zv, res_context *ctx)
{
	unsigned ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (long)ival);
}
static void to_zval_read_net_uint16(const char *data, zval *zv, res_context *ctx)
{
	uint16_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (long)ntohs(ival));
}
static void to_zval_read_uint32(const char *data, zval *zv, res_context *ctx)
{
	uint32_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (long)ival);
}
static void to_zval_read_sa_family(const char *data, zval *zv, res_context *ctx)
{
	sa_family_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (long)ival);
}

/* CONVERSIONS for sockaddr */
static void from_zval_write_sin_addr(const zval *zaddr_str, char *inaddr, ser_context *ctx)
{
	int					res;
	struct sockaddr_in	saddr = {0};
	zval				lzval = zval_used_for_init;
	TSRMLS_FETCH();

	if (Z_TYPE_P(zaddr_str) != IS_STRING) {
		ZVAL_COPY_VALUE(&lzval, zaddr_str);
		zval_copy_ctor(&lzval);
		convert_to_string(&lzval);
		zaddr_str = &lzval;
	}

	res = php_set_inet_addr(&saddr, Z_STRVAL_P(zaddr_str), ctx->sock TSRMLS_CC);
	if (res) {
		memcpy(inaddr, &saddr.sin_addr, sizeof saddr.sin_addr);
	} else {
		/* error already emitted, but let's emit another more relevant */
		do_from_zval_err(ctx, "could not resolve address '%s' to get an AF_INET "
				"address", Z_STRVAL_P(zaddr_str));
	}

	zval_dtor(&lzval);
}
static void to_zval_read_sin_addr(const char *data, zval *zv, res_context *ctx)
{
	const struct in_addr *addr = (const struct in_addr *)data;
	socklen_t size = INET_ADDRSTRLEN;

	Z_TYPE_P(zv) = IS_STRING;
	Z_STRVAL_P(zv) = ecalloc(1, size);
	Z_STRLEN_P(zv) = 0;

	if (inet_ntop(AF_INET, addr, Z_STRVAL_P(zv), size) == NULL) {
		do_to_zval_err(ctx, "could not convert IPv4 address to string "
				"(errno %d)", errno);
		return;
	}

	Z_STRLEN_P(zv) = strlen(Z_STRVAL_P(zv));
}
static const field_descriptor descriptors_sockaddr_in[] = {
		{"family", sizeof("family"), 0, offsetof(struct sockaddr_in, sin_family), from_zval_write_sa_family, to_zval_read_sa_family},
		{"addr", sizeof("addr"), 0, offsetof(struct sockaddr_in, sin_addr), from_zval_write_sin_addr, to_zval_read_sin_addr},
		{"port", sizeof("port"), 0, offsetof(struct sockaddr_in, sin_port), from_zval_write_net_uint16, to_zval_read_net_uint16},
		{0}
};
static void from_zval_write_sockaddr_in(const zval *container, char *sockaddr, ser_context *ctx)
{
	from_zval_write_aggregation(container, sockaddr, descriptors_sockaddr_in, ctx);
}
static void to_zval_read_sockaddr_in(const char *data, zval *zv, res_context *ctx)
{
	to_zval_read_aggregation(data, zv, descriptors_sockaddr_in, ctx);
}
static void from_zval_write_sin6_addr(const zval *zaddr_str, char *addr6, ser_context *ctx)
{
	int					res;
	struct sockaddr_in6	saddr6 = {0};
	zval				lzval = zval_used_for_init;
	TSRMLS_FETCH();

	if (Z_TYPE_P(zaddr_str) != IS_STRING) {
		ZVAL_COPY_VALUE(&lzval, zaddr_str);
		zval_copy_ctor(&lzval);
		convert_to_string(&lzval);
		zaddr_str = &lzval;
	}

	res = php_set_inet6_addr(&saddr6,
			Z_STRVAL_P(zaddr_str), ctx->sock TSRMLS_CC);
	if (res) {
		memcpy(addr6, &saddr6.sin6_addr, sizeof saddr6.sin6_addr);
	} else {
		/* error already emitted, but let's emit another more relevant */
		do_from_zval_err(ctx, "could not resolve address '%s' to get an AF_INET6 "
				"address", Z_STRVAL_P(zaddr_str));
	}

	zval_dtor(&lzval);
}
static void to_zval_read_sin6_addr(const char *data, zval *zv, res_context *ctx)
{
	const struct in6_addr *addr = (const struct in6_addr *)data;
	socklen_t size = INET6_ADDRSTRLEN;

	Z_TYPE_P(zv) = IS_STRING;
	Z_STRVAL_P(zv) = ecalloc(1, size);
	Z_STRLEN_P(zv) = 0;

	if (inet_ntop(AF_INET6, addr, Z_STRVAL_P(zv), size) == NULL) {
		do_to_zval_err(ctx, "could not convert IPv6 address to string "
				"(errno %d)", errno);
		return;
	}

	Z_STRLEN_P(zv) = strlen(Z_STRVAL_P(zv));
}
static const field_descriptor descriptors_sockaddr_in6[] = {
		{"family", sizeof("family"), 0, offsetof(struct sockaddr_in6, sin6_family), from_zval_write_sa_family, to_zval_read_sa_family},
		{"addr", sizeof("addr"), 0, offsetof(struct sockaddr_in6, sin6_addr), from_zval_write_sin6_addr, to_zval_read_sin6_addr},
		{"port", sizeof("port"), 0, offsetof(struct sockaddr_in6, sin6_port), from_zval_write_net_uint16, to_zval_read_net_uint16},
		{"flowinfo", sizeof("flowinfo"), 0, offsetof(struct sockaddr_in6, sin6_flowinfo), from_zval_write_uint32, to_zval_read_uint32},
		{"scope_id", sizeof("scope_id"), 0, offsetof(struct sockaddr_in6, sin6_scope_id), from_zval_write_uint32, to_zval_read_uint32},
		{0}
};
static void from_zval_write_sockaddr_in6(const zval *container, char *sockaddr6, ser_context *ctx)
{
	from_zval_write_aggregation(container, sockaddr6, descriptors_sockaddr_in6, ctx);
}
static void to_zval_read_sockaddr_in6(const char *data, zval *zv, res_context *ctx)
{
	to_zval_read_aggregation(data, zv, descriptors_sockaddr_in6, ctx);
}
static void from_zval_write_sockaddr_aux(const zval *container,
										 struct sockaddr **sockaddr_ptr,
										 socklen_t *sockaddr_len,
										 ser_context *ctx)
{
	int		family;
	zval	**elem;
	int		fill_sockaddr;

	if (Z_TYPE_P(container) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
		return;
	}

	fill_sockaddr = param_get_bool(ctx, KEY_FILL_SOCKADDR, 1);

	if (zend_hash_find(Z_ARRVAL_P(container), "family", sizeof("family"), (void**)&elem) == SUCCESS
			&& Z_TYPE_PP(elem) != IS_NULL) {
		const char *node = "family";
		zend_llist_add_element(&ctx->keys, &node);
		from_zval_write_int(*elem, (char*)&family, ctx);
		zend_llist_remove_tail(&ctx->keys);
	} else {
		family = ctx->sock->type;
	}

	switch (family) {
	case AF_INET:
		/* though not all OSes support sockaddr_in used in IPv6 sockets */
		if (ctx->sock->type != AF_INET && ctx->sock->type != AF_INET6) {
			do_from_zval_err(ctx, "the specified family (number %d) is not "
					"supported on this socket", family);
			return;
		}
		*sockaddr_ptr = accounted_ecalloc(1, sizeof(struct sockaddr_in), ctx);
		*sockaddr_len = sizeof(struct sockaddr_in);
		if (fill_sockaddr) {
			from_zval_write_sockaddr_in(container, (char*)*sockaddr_ptr, ctx);
		}
		break;
	case AF_INET6:
		if (ctx->sock->type != AF_INET6) {
			do_from_zval_err(ctx, "the specified family (AF_INET6) is not "
					"supported on this socket");
			return;
		}
		*sockaddr_ptr = accounted_ecalloc(1, sizeof(struct sockaddr_in6), ctx);
		*sockaddr_len = sizeof(struct sockaddr_in6);
		if (fill_sockaddr) {
			from_zval_write_sockaddr_in6(container, (char*)*sockaddr_ptr, ctx);
		}
		break;
	default:
		do_from_zval_err(ctx, "%s", "the only families currently supported are "
				"AF_INET and AF_INET6");
		break;
	}
}
static void to_zval_read_sockaddr_aux(const char *sockaddr_c, zval *zv, res_context *ctx)
{
	const struct sockaddr *saddr = (struct sockaddr *)sockaddr_c;

	assert(Z_TYPE_P(zv) == IS_ARRAY);

	switch (saddr->sa_family) {
	case AF_INET:
		to_zval_read_sockaddr_in(sockaddr_c, zv, ctx);
		break;

	case AF_INET6:
		to_zval_read_sockaddr_in6(sockaddr_c, zv, ctx);
		break;

	default:
		do_to_zval_err(ctx, "cannot read struct sockaddr with family %d; "
				"not supported",
				(int)saddr->sa_family);
		break;
	}
}

/* CONVERSIONS for cmsghdr */
/*
 * [ level => , type => , data => [],]
 * struct cmsghdr {
 *  socklen_t cmsg_len;    // data byte count, including header
 *  int       cmsg_level;  // originating protocol
 *  int       cmsg_type;   // protocol-specific type
 *  // followed by unsigned char cmsg_data[];
 * };
 */
static void from_zval_write_control(const zval			*arr,
									void				**control_buf,
									zend_llist_element	*alloc,
									size_t				*control_len,
									size_t				*offset,
									ser_context			*ctx)
{
	struct cmsghdr		*cmsghdr;
	int					level,
						type;
	size_t				req_space,
						space_left;
	ancillary_reg_entry	*entry;

	static const field_descriptor descriptor_level[] = {
			{"level", sizeof("level"), 0, 0, from_zval_write_int, 0},
			{0}
	};
	static const field_descriptor descriptor_type[] = {
			{"type", sizeof("type"), 0, 0, from_zval_write_int, 0},
			{0}
	};
	field_descriptor descriptor_data[] = {
			{"data", sizeof("data"), 0, 0, 0, 0},
			{0}
	};

	from_zval_write_aggregation(arr, (char *)&level, descriptor_level, ctx);
	if (ctx->err.has_error) {
		return;
	}
	from_zval_write_aggregation(arr, (char *)&type, descriptor_type, ctx);
	if (ctx->err.has_error) {
		return;
	}

	entry = get_ancillary_reg_entry(level, type);
	if (entry == NULL) {
		do_from_zval_err(ctx, "cmsghdr with level %d and type %d not supported",
				level, type);
		return;
	}

	req_space = CMSG_SPACE(entry->size);
	space_left = *control_len - *offset;
	assert(*control_len >= *offset);

	if (space_left < req_space) {
		*control_buf = safe_erealloc(*control_buf, 2, req_space, *control_len);
		*control_len += 2 * req_space;
		memcpy(&alloc->data, *control_buf, sizeof *control_buf);
	}

	cmsghdr = (struct cmsghdr*)(((char*)*control_buf) + *offset);
	cmsghdr->cmsg_level	= level;
	cmsghdr->cmsg_type	= type;
	cmsghdr->cmsg_len	= CMSG_LEN(entry->size);

	descriptor_data[0].from_zval = entry->from_array;
	from_zval_write_aggregation(arr, (char*)CMSG_DATA(cmsghdr), descriptor_data, ctx);

	*offset += req_space;
}
static void from_zval_write_control_array(const zval *arr, char *msghdr_c, ser_context *ctx)
{
	HashPosition		pos;
	char				buf[sizeof("element #4294967295")];
	char				*bufp = buf;
	zval				**elem;
	uint32_t			i;
	int					num_elems;
	void				*control_buf;
	zend_llist_element	*alloc;
	size_t				control_len,
						cur_offset;
	struct msghdr		*msg = (struct msghdr*)msghdr_c;

	if (Z_TYPE_P(arr) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
		return;
	}

	num_elems = zend_hash_num_elements(Z_ARRVAL_P(arr));
	if (num_elems == 0) {
		return;
	}

	/* estimate each message at 20 bytes */
	control_buf	= accounted_safe_ecalloc(num_elems, CMSG_SPACE(20), 0, ctx);
	alloc		= ctx->allocations.tail;
	control_len = (size_t)num_elems * CMSG_SPACE(20);
	cur_offset	= 0;

    for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos), i = 0;
			!ctx->err.has_error
			&& zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **)&elem, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos)) {

		if (snprintf(buf, sizeof(buf), "element #%u", (unsigned)i++) >= sizeof(buf)) {
			memcpy(buf, "element", sizeof("element"));
		}
		zend_llist_add_element(&ctx->keys, &bufp);

		from_zval_write_control(*elem, &control_buf, alloc, &control_len,
				&cur_offset, ctx);

		zend_llist_remove_tail(&ctx->keys);
    }

    msg->msg_control = control_buf;
    msg->msg_controllen = control_len;
}
static void to_zval_read_cmsg_data(const char *cmsghdr_c, zval *zv, res_context *ctx)
{
	const struct cmsghdr *cmsg = (const struct cmsghdr *)cmsghdr_c;
	ancillary_reg_entry	*entry;

	entry = get_ancillary_reg_entry(cmsg->cmsg_level, cmsg->cmsg_type);
	if (entry == NULL) {
		do_to_zval_err(ctx, "cmsghdr with level %d and type %d not supported",
				cmsg->cmsg_level, cmsg->cmsg_type);
		return;
	}
	if (CMSG_LEN(entry->size) > cmsg->cmsg_len) {
		do_to_zval_err(ctx, "the cmsghdr structure is unexpectedly small; "
				"expected a length of at least %ld, but got %ld",
				(long)CMSG_LEN(entry->size), (long)cmsg->cmsg_len);
		return;
	}

	entry->to_array((const char *)CMSG_DATA(cmsg), zv, ctx);
}
static void to_zval_read_control(const char *cmsghdr_c, zval *zv, res_context *ctx)
{
	/* takes a cmsghdr, not a msghdr like from_zval_write_control */
	static const field_descriptor descriptors[] = {
			{"level", sizeof("level"), 0, offsetof(struct cmsghdr, cmsg_level), 0, to_zval_read_int},
			{"type", sizeof("type"), 0, offsetof(struct cmsghdr, cmsg_type), 0, to_zval_read_int},
			{"data", sizeof("data"), 0, 0 /* cmsghdr passed */, 0, to_zval_read_cmsg_data},
			{0}
	};

	array_init_size(zv, 3);
	to_zval_read_aggregation(cmsghdr_c, zv, descriptors, ctx);
}
static void to_zval_read_control_array(const char *msghdr_c, zval *zv, res_context *ctx)
{
	struct msghdr	*msg = (struct msghdr *)msghdr_c;
	struct cmsghdr	*cmsg;
	char			buf[sizeof("element #4294967295")];
	char			*bufp = buf;
	uint32_t		i = 1;

	/*if (msg->msg_flags & MSG_CTRUNC) {
		php_error_docref0(NULL, E_WARNING, "The MSG_CTRUNC flag is present; will not "
				"attempt to read control messages");
		ZVAL_FALSE(zv);
		return;
	}*/

	array_init(zv);

	for (cmsg = CMSG_FIRSTHDR(msg);
			cmsg != NULL && !ctx->err.has_error;
			cmsg = CMSG_NXTHDR(msg,cmsg)) {
		zval *elem;

		ALLOC_INIT_ZVAL(elem);
		add_next_index_zval(zv, elem);

		if (snprintf(buf, sizeof(buf), "element #%u", (unsigned)i++) >= sizeof(buf)) {
			memcpy(buf, "element", sizeof("element"));
		}
		zend_llist_add_element(&ctx->keys, &bufp);

		to_zval_read_control((const char *)cmsg, elem, ctx);

		zend_llist_remove_tail(&ctx->keys);
	}
}

/* CONVERSIONS for msghdr */
static void from_zval_write_name(const zval *zname_arr, char *msghdr_c, ser_context *ctx)
{
	struct sockaddr	*sockaddr;
	socklen_t		sockaddr_len;
	struct msghdr	*msghdr = (struct msghdr *)msghdr_c;

	from_zval_write_sockaddr_aux(zname_arr, &sockaddr, &sockaddr_len, ctx);

	msghdr->msg_name = sockaddr;
	msghdr->msg_namelen = sockaddr_len;
}
static void to_zval_read_name(const char *sockaddr_p, zval *zv, res_context *ctx)
{
	void *name = (void*)*(void**)sockaddr_p;
	if (name == NULL) {
		ZVAL_NULL(zv);
	} else {
		array_init(zv);
		to_zval_read_sockaddr_aux(name, zv, ctx);
	}
}
static void from_zval_write_msghdr_buffer_size(const zval *elem, char *msghdr_c, ser_context *ctx)
{
	long lval;
	struct msghdr *msghdr = (struct msghdr *)msghdr_c;

	lval = from_zval_integer_common(elem, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (lval < 0 || lval > MAX_USER_BUFF_SIZE) {
		do_from_zval_err(ctx, "the buffer size must be between 1 and %ld; "
				"given %ld", (long)MAX_USER_BUFF_SIZE, lval);
		return;
	}

	msghdr->msg_iovlen = 1;
	msghdr->msg_iov = accounted_emalloc(sizeof(*msghdr->msg_iov) * 1, ctx);
	msghdr->msg_iov[0].iov_base = accounted_emalloc((size_t)lval, ctx);
	msghdr->msg_iov[0].iov_len = (size_t)lval;
}
static void from_zval_write_iov_array(const zval *arr, char *msghdr_c, ser_context *ctx)
{
	HashPosition	pos;
	int				num_elem;
	zval			**elem;
	unsigned		i;
	struct msghdr	*msg = (struct msghdr*)msghdr_c;
	char			buf[sizeof("element #4294967295")];
	char			*bufp = buf;

	if (Z_TYPE_P(arr) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
	}

	num_elem = zend_hash_num_elements(Z_ARRVAL_P(arr));
	if (num_elem == 0) {
		return;
	}

	msg->msg_iov = accounted_safe_ecalloc(num_elem, sizeof *msg->msg_iov, 0, ctx);
	msg->msg_iovlen = (size_t)num_elem;

    for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos), i = 0;
			!ctx->err.has_error
			&& zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **)&elem, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos)) {
    	size_t len;

		if (snprintf(buf, sizeof(buf), "element #%u", (unsigned)i++) >= sizeof(buf)) {
			memcpy(buf, "element", sizeof("element"));
		}
		zend_llist_add_element(&ctx->keys, &bufp);

		zval_add_ref(elem);
		convert_to_string_ex(elem);

		len = Z_STRLEN_PP(elem);
		msg->msg_iov[i - 1].iov_base = accounted_emalloc(len, ctx);
		msg->msg_iov[i - 1].iov_len = len;
		memcpy(msg->msg_iov[i - 1].iov_base, Z_STRVAL_PP(elem), len);

		zval_ptr_dtor(elem);

		zend_llist_remove_tail(&ctx->keys);
    }

}
static void from_zval_write_controllen(const zval *elem, char *msghdr_c, ser_context *ctx)
{
	struct msghdr *msghdr = (struct msghdr *)msghdr_c;
	uint32_t len;

	/* controllen should be an unsigned with at least 32-bit. Let's assume
	 * this least common denominator
	 */
	from_zval_write_uint32(elem, (char*)&len, ctx);
	if (!ctx->err.has_error && len == 0) {
		do_from_zval_err(ctx, "controllen cannot be 0");
	}
	msghdr->msg_control = accounted_emalloc(len, ctx);
	msghdr->msg_controllen = len;
}
static void from_zval_write_msghdr_send(const zval *container, char *msghdr_c, ser_context *ctx)
{
	static const field_descriptor descriptors[] = {
			{"name", sizeof("name"), 0, 0, from_zval_write_name, 0},
			{"iov", sizeof("iov"), 0, 0, from_zval_write_iov_array, 0},
			{"control", sizeof("control"), 0, 0, from_zval_write_control_array, 0},
			{0}
	};

	from_zval_write_aggregation(container, msghdr_c, descriptors, ctx);
}
static void from_zval_write_msghdr_recv(const zval *container, char *msghdr_c, ser_context *ctx)
{
	/* zval to struct msghdr, version for recvmsg(). It differs from the version
	 * for sendmsg() in that it:
	 * 	- has a buffer_size instead of an iov array;
	 * 	- has no control element; has a controllen element instead
	 * struct msghdr {
	 *    void *msg_name;
	 *    socklen_t msg_namelen;
	 *    struct iovec *msg_iov;
	 *    size_t msg_iovlen;
	 *    void *msg_control;
	 *    size_t msg_controllen; //can also be socklen_t
	 *    int msg_flags;
	 * };
	 */
	static const field_descriptor descriptors[] = {
			{"name", sizeof("name"), 0, 0, from_zval_write_name, 0},
			{"buffer_size", sizeof("buffer_size"), 0, 0, from_zval_write_msghdr_buffer_size, 0},
			{"controllen", sizeof("controllen"), 1, 0, from_zval_write_controllen, 0},
			{0}
	};
	struct msghdr 	*msghdr = (struct msghdr *)msghdr_c;
	const int		falsev = 0,
					*falsevp = &falsev;

	if (zend_hash_add(&ctx->params, KEY_FILL_SOCKADDR, sizeof(KEY_FILL_SOCKADDR),
			(void*)&falsevp, sizeof(falsevp), NULL) == FAILURE) {
		do_from_zval_err(ctx, "could not add fill_sockaddr; this is a bug");
		return;
	}

	from_zval_write_aggregation(container, msghdr_c, descriptors, ctx);

	zend_hash_del(&ctx->params, KEY_FILL_SOCKADDR, sizeof(KEY_FILL_SOCKADDR));
	if (ctx->err.has_error) {
		return;
	}

	if (msghdr->msg_iovlen == 0) {
		msghdr->msg_iovlen = 1;
		msghdr->msg_iov = accounted_emalloc(sizeof(*msghdr->msg_iov) * 1, ctx);
		msghdr->msg_iov[0].iov_base = accounted_emalloc((size_t)DEFAULT_BUFF_SIZE, ctx);
		msghdr->msg_iov[0].iov_len = (size_t)DEFAULT_BUFF_SIZE;
	}
}

static void to_zval_read_iov(const char *msghdr_c, zval *zv, res_context *ctx)
{
	const struct msghdr	*msghdr = (const struct msghdr *)msghdr_c;
	size_t				iovlen = msghdr->msg_iovlen;
	ssize_t				**recvmsg_ret,
						bytes_left;
	uint				i;

	if (iovlen > UINT_MAX) {
		do_to_zval_err(ctx, "unexpectedly large value for iov_len: %lu",
				(unsigned long)iovlen);
	}
	array_init_size(zv, (uint)iovlen);

	if (zend_hash_find(&ctx->params, KEY_RECVMSG_RET, sizeof(KEY_RECVMSG_RET),
			(void**)&recvmsg_ret) == FAILURE) {
		do_to_zval_err(ctx, "recvmsg_ret not found in params. This is a bug");
		return;
	}
	bytes_left = **recvmsg_ret;

	for (i = 0; bytes_left > 0 && i < (uint)iovlen; i++) {
		zval	*elem;
		size_t	len		= MIN(msghdr->msg_iov[i].iov_len, bytes_left);
		char	*buf	= safe_emalloc(1, len, 1);

		MAKE_STD_ZVAL(elem);
		memcpy(buf, msghdr->msg_iov[i].iov_base, len);
		buf[len] = '\0';

		ZVAL_STRINGL(elem, buf, len, 0);
		add_next_index_zval(zv, elem);
		bytes_left -= len;
	}
}
static void to_zval_read_msghdr(const char *msghdr_c, zval *zv, res_context *ctx)
{
	static const field_descriptor descriptors[] = {
			{"name", sizeof("name"), 0, offsetof(struct msghdr, msg_name), 0, to_zval_read_name},
			{"control", sizeof("control"), 0, 0, 0, to_zval_read_control_array},
			{"iov", sizeof("iov"), 0, 0, 0, to_zval_read_iov},
			{"flags", sizeof("flags"), 0, offsetof(struct msghdr, msg_flags), 0, to_zval_read_int},
			{0}
	};

	array_init_size(zv, 4);

	to_zval_read_aggregation(msghdr_c, zv, descriptors, ctx);
}


/* CONVERSIONS for struct in6_pktinfo */
static const field_descriptor descriptors_in6_pktinfo[] = {
		{"addr", sizeof("addr"), 1, offsetof(struct in6_pktinfo, ipi6_addr), from_zval_write_sin6_addr, to_zval_read_sin6_addr},
		{"ifindex", sizeof("ifindex"), 1, offsetof(struct in6_pktinfo, ipi6_ifindex), from_zval_write_unsigned, to_zval_read_unsigned},
		{0}
};
static void from_zval_write_in6_pktinfo(const zval *container, char *in6_pktinfo_c, ser_context *ctx)
{
	from_zval_write_aggregation(container, in6_pktinfo_c, descriptors_in6_pktinfo, ctx);
}
static void to_zval_read_in6_pktinfo(const char *data, zval *zv, res_context *ctx)
{
	array_init_size(zv, 2);

	to_zval_read_aggregation(data, zv, descriptors_in6_pktinfo, ctx);
}

/* ENTRY POINT for conversions */
static void free_from_zval_allocation(void *alloc_ptr_ptr)
{
	efree(*(void**)alloc_ptr_ptr);
}
static void *from_zval_run_conversions(const zval			*container,
									  php_socket			*sock,
									  from_zval_write_field	*writer,
									  size_t				struct_size,
									  const char			*top_name,
									  zend_llist			**allocations /* out */,
									  struct err_s			*err /* in/out */)
{
	ser_context ctx = {{0}};
	char *structure = NULL;

	*allocations = NULL;

	if (err->has_error) {
		return NULL;
	}

	zend_hash_init(&ctx.params, 8, NULL, NULL, 0);
	zend_llist_init(&ctx.keys, sizeof(const char *), NULL, 0);
	zend_llist_init(&ctx.allocations, sizeof(void *), &free_from_zval_allocation, 0);
	ctx.sock = sock;

	structure = ecalloc(1, struct_size);

	zend_llist_add_element(&ctx.keys, &top_name);
	zend_llist_add_element(&ctx.allocations, &structure);

	/* main call */
	writer(container, structure, &ctx);

	if (ctx.err.has_error) {
		zend_llist_destroy(&ctx.allocations); /* deallocates structure as well */
		structure = NULL;
		*err = ctx.err;
	} else {
		*allocations = emalloc(sizeof **allocations);
		**allocations = ctx.allocations;
	}

	zend_llist_destroy(&ctx.keys);
	zend_hash_destroy(&ctx.params);

	return structure;
}
static zval *to_zval_run_conversions(const char *structure,
									 to_zval_read_field *reader,
									 const char *top_name,
									 const struct key_value *key_value_pairs,
									 struct err_s *err)
{
	res_context				ctx = {{0}, {0}};
	const struct key_value	*kv;
	zval					*zv = NULL;

	if (err->has_error) {
		return NULL;
	}

	ALLOC_INIT_ZVAL(zv);

	zend_llist_init(&ctx.keys, sizeof(const char *), NULL, 0);
	zend_llist_add_element(&ctx.keys, &top_name);

	zend_hash_init(&ctx.params, 8, NULL, NULL, 0);
	for (kv = key_value_pairs; kv->key != NULL; kv++) {
		zend_hash_update(&ctx.params, kv->key, kv->key_size,
				(void*)&kv->value, sizeof(kv->value), NULL);
	}

	/* main call */
	reader(structure, zv, &ctx);

	if (ctx.err.has_error) {
		zval_ptr_dtor(&zv);
		zv = NULL;
		*err = ctx.err;
	}

	zend_llist_destroy(&ctx.keys);
	zend_hash_destroy(&ctx.params);

	return zv;
}

#ifdef ZTS
static MUTEX_T ancillary_mutex;
#endif
static void init_ancillary_registry(void)
{
	ancillary_reg_entry entry;
	anc_reg_key key;
	ancillary_registry.initialized = 1;

	zend_hash_init(&ancillary_registry.ht, 32, NULL, NULL, 1);

	/* struct in6_pktinfo *pktinfo; */
	entry.size			= sizeof(struct in6_pktinfo);
	entry.from_array	= from_zval_write_in6_pktinfo;
	entry.to_array		= to_zval_read_in6_pktinfo;
	key.cmsg_level		= IPPROTO_IPV6;
	key.msg_type		= IPV6_PKTINFO;
	zend_hash_update(&ancillary_registry.ht, (char*)&key, sizeof(key),
			(void*)&entry, sizeof(entry), NULL);
}
static ancillary_reg_entry *get_ancillary_reg_entry(int cmsg_level, int msg_type)
{
	anc_reg_key			key = { cmsg_level, msg_type };
	ancillary_reg_entry	*entry;

#ifdef ZTS
	tsrm_mutex_lock(ancillary_mutex);
#endif
	if (!ancillary_registry.initialized) {
		init_ancillary_registry();
	}
#ifdef ZTS
	tsrm_mutex_unlock(ancillary_mutex);
#endif

	if (zend_hash_find(&ancillary_registry.ht, (char*)&key, sizeof(key),
			(void**)&entry) == SUCCESS) {
		return entry;
	} else {
		return NULL;
	}
}

PHP_FUNCTION(socket_sendmsg)
{
	zval			*zsocket,
					*zmsg;
	long			flags = 0;
	php_socket		*php_sock;
	struct msghdr	*msghdr;
	zend_llist		*allocations;
	struct err_s	err = {0};
	ssize_t			res;

	/* zmsg should be passed by ref */
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|l", &zsocket, &zmsg, &flags) == FAILURE) {
		return;
	}

	LONG_CHECK_VALID_INT(flags);

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &zsocket, -1,
			php_sockets_le_socket_name, php_sockets_le_socket());

	msghdr = from_zval_run_conversions(zmsg, php_sock, from_zval_write_msghdr_send,
			sizeof(*msghdr), "msghdr", &allocations, &err);

	if (err.has_error) {
		err_msg_dispose(&err TSRMLS_CC);
		RETURN_FALSE;
	}

	res = sendmsg(php_sock->bsd_socket, msghdr, (int)flags);

	if (res != -1) {
		zend_llist_destroy(allocations);
		efree(allocations);

		RETURN_LONG((long)res);
	} else {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error in sendmsg [%d]: %s",
				errno, sockets_strerror(errno TSRMLS_CC));
		RETURN_FALSE;
	}
}

PHP_FUNCTION(socket_recvmsg)
{
	zval			*zsocket,
					*zmsg;
	long			flags = 0;
	php_socket		*php_sock;
	ssize_t			res;
	struct msghdr	*msghdr;
	zend_llist		*allocations;
	struct err_s	err = {0};

	//ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra|l",
			&zsocket, &zmsg, &flags) == FAILURE) {
		return;
	}

	LONG_CHECK_VALID_INT(flags);

	ZEND_FETCH_RESOURCE(php_sock, php_socket *, &zsocket, -1,
			php_sockets_le_socket_name, php_sockets_le_socket());

	msghdr = from_zval_run_conversions(zmsg, php_sock, from_zval_write_msghdr_recv,
			sizeof(*msghdr), "msghdr", &allocations, &err);

	if (err.has_error) {
		err_msg_dispose(&err TSRMLS_CC);
		RETURN_FALSE;
	}

	res = recvmsg(php_sock->bsd_socket, msghdr, (int)flags);

	if (res != -1) {
		zval *zres;
		struct key_value kv[] = {
				{KEY_RECVMSG_RET, sizeof(KEY_RECVMSG_RET), &res},
				{0}
		};


		zres = to_zval_run_conversions((char *)msghdr, to_zval_read_msghdr,
				"msghdr", kv, &err);

		/* we don;t need msghdr anymore; free it */
		msghdr = NULL;
		zend_llist_destroy(allocations);
		efree(allocations);

		zval_dtor(zmsg);
		if (!err.has_error) {
			ZVAL_COPY_VALUE(zmsg, zres);
			efree(zres); /* only shallow destruction */
		} else {
			err_msg_dispose(&err TSRMLS_CC);
			ZVAL_FALSE(zmsg);
			/* no need to destroy/free zres -- it's NULL in this circumstance */
			assert(zres == NULL);
		}
	} else {
		SOCKETS_G(last_error) = errno;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "error in recvmsg [%d]: %s",
				errno, sockets_strerror(errno TSRMLS_CC));
		RETURN_FALSE;
	}

	RETURN_LONG((long)res);
}

PHP_FUNCTION(socket_cmsg_space)
{
	long				level,
						type;
	ancillary_reg_entry	*entry;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &level, &type) == FAILURE) {
		return;
	}

	LONG_CHECK_VALID_INT(level);
	LONG_CHECK_VALID_INT(type);

	entry = get_ancillary_reg_entry(level, type);
	if (entry == NULL) {
		php_error_docref0(NULL TSRMLS_CC, E_WARNING, "The pair level %ld/type %ld is "
				"not supported by PHP", level, type);
		return;
	}

	RETURN_LONG((long)CMSG_SPACE(entry->size));
}

void _socket_sendrecvmsg_init(INIT_FUNC_ARGS)
{
	REGISTER_LONG_CONSTANT("IPV6_RECVPKTINFO",		IPV6_RECVPKTINFO,	CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("IPV6_PKTINFO",			IPV6_PKTINFO,		CONST_CS | CONST_PERSISTENT);

#ifdef ZTS
	ancillary_mutex = tsrm_mutex_alloc();
#endif
}

void _socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS)
{
#ifdef ZTS
	tsrm_mutex_free(ancillary_mutex);
#endif
}
