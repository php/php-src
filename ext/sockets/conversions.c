#ifdef __sun
/* to enable 'new' ancillary data layout instead */
# define _XPG4_2
#endif
#include "sockaddr_conv.h"
#include "conversions.h"
#include "sendrecvmsg.h" /* for ancillary registry */
#ifdef PHP_WIN32
# include "windows_common.h"
#endif

#include <Zend/zend_llist.h>
#include <zend_smart_str.h>

#ifndef PHP_WIN32
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/un.h>
# include <sys/ioctl.h>
# include <net/if.h>
#else
# include <stdint.h>
#endif

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef PHP_WIN32
typedef unsigned short sa_family_t;
# define msghdr			_WSAMSG
/*
struct _WSAMSG {
    LPSOCKADDR       name;				//void *msg_name
    INT              namelen;			//socklen_t msg_namelen
    LPWSABUF         lpBuffers;			//struct iovec *msg_iov
    ULONG            dwBufferCount;		//size_t msg_iovlen
    WSABUF           Control;			//void *msg_control, size_t msg_controllen
    DWORD            dwFlags;			//int msg_flags
}
*/
# define msg_name		name
# define msg_namelen	namelen
# define msg_iov		lpBuffers
# define msg_iovlen		dwBufferCount
# define msg_control	Control.buf
# define msg_controllen	Control.len
# define msg_flags		dwFlags
# define iov_base		buf
# define iov_len		len

# ifdef CMSG_DATA
#  undef CMSG_DATA
# endif
# define CMSG_DATA		WSA_CMSG_DATA
#endif

#define MAX_USER_BUFF_SIZE ((size_t)(100*1024*1024))
#define DEFAULT_BUFF_SIZE 8192

struct _ser_context {
	HashTable		params; /* stores pointers; has to be first */
	struct err_s	err;
	zend_llist		keys,
	/* common part to res_context ends here */
					allocations;
	php_socket		*sock;
};
struct _res_context {
	HashTable		params; /* stores pointers; has to be first */
	struct err_s	err;
	zend_llist		keys;
};

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

#define KEY_FILL_SOCKADDR "fill_sockaddr"
#define KEY_RECVMSG_RET   "recvmsg_ret"
#define KEY_CMSG_LEN	  "cmsg_len"

const struct key_value empty_key_value_list[] = {{0}};

/* PARAMETERS */
static int param_get_bool(void *ctx, const char *key, int def)
{
	int *elem;
	if ((elem = zend_hash_str_find_ptr(ctx, key, strlen(key))) != NULL) {
		return *elem;
	} else {
		return def;
	}
}

/* MEMORY */
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

/* ERRORS */
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

	if (path.s && ZSTR_LEN(path.s) > 3) {
		ZSTR_LEN(path.s) -= 3;
	}
	smart_str_0(&path);

	user_msg_size = vspprintf(&user_msg, 0, fmt, ap);

	err->has_error = 1;
	err->level = E_WARNING;
	spprintf(&err->msg, 0, "error converting %s data (path: %s): %.*s",
			what_conv,
			path.s && *ZSTR_VAL(path.s) != '\0' ? ZSTR_VAL(path.s) : "unavailable",
			user_msg_size, user_msg);
	err->should_free = 1;

	efree(user_msg);
	smart_str_free(&path);
}
ZEND_ATTRIBUTE_FORMAT(printf, 2 ,3)
static void do_from_zval_err(ser_context *ctx, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	do_from_to_zval_err(&ctx->err, &ctx->keys, "user", fmt, ap);
	va_end(ap);
}
ZEND_ATTRIBUTE_FORMAT(printf, 2 ,3)
static void do_to_zval_err(res_context *ctx, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	do_from_to_zval_err(&ctx->err, &ctx->keys, "native", fmt, ap);
	va_end(ap);
}

void err_msg_dispose(struct err_s *err)
{
	if (err->msg != NULL) {
		php_error_docref(NULL, err->level, "%s", err->msg);
		if (err->should_free) {
			efree(err->msg);
		}
	}
}
void allocations_dispose(zend_llist **allocations)
{
	zend_llist_destroy(*allocations);
	efree(*allocations);
	*allocations = NULL;
}

static unsigned from_array_iterate(const zval *arr,
								   void (*func)(zval *elem, unsigned i, void **args, ser_context *ctx),
								   void **args,
								   ser_context *ctx)
{
	unsigned		i;
	zval			*elem;
	char			buf[sizeof("element #4294967295")];
	char			*bufp = buf;

	/* Note i starts at 1, not 0! */
	i = 1;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), elem) {
		if ((size_t)snprintf(buf, sizeof(buf), "element #%u", i) >= sizeof(buf)) {
			memcpy(buf, "element", sizeof("element"));
		}
		zend_llist_add_element(&ctx->keys, &bufp);

		func(elem, i, args, ctx);

		zend_llist_remove_tail(&ctx->keys);
		if (ctx->err.has_error) {
			break;
		}
		i++;
	} ZEND_HASH_FOREACH_END();

	return i -1;
}

/* Generic Aggregated conversions */
static void from_zval_write_aggregation(const zval *container,
										char *structure,
										const field_descriptor *descriptors,
										ser_context *ctx)
{
	const field_descriptor	*descr;
	zval					*elem;

	if (Z_TYPE_P(container) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
	}

	for (descr = descriptors; descr->name != NULL && !ctx->err.has_error; descr++) {
		if ((elem = zend_hash_str_find(Z_ARRVAL_P(container),
				descr->name, descr->name_size - 1)) != NULL) {

			if (descr->from_zval == NULL) {
				do_from_zval_err(ctx, "No information on how to convert value "
						"of key '%s'", descr->name);
				break;
			}

			zend_llist_add_element(&ctx->keys, (void*)&descr->name);
			descr->from_zval(elem, ((char*)structure) + descr->field_offset, ctx);
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
		zval *new_zv, tmp;

		if (descr->to_zval == NULL) {
			do_to_zval_err(ctx, "No information on how to convert native "
					"field into value for key '%s'", descr->name);
			break;
		}

		ZVAL_NULL(&tmp);
		new_zv = zend_symtable_str_update(Z_ARRVAL_P(zarr), descr->name, descr->name_size - 1, &tmp);

		zend_llist_add_element(&ctx->keys, (void*)&descr->name);
		descr->to_zval(structure + descr->field_offset, new_zv, ctx);
		zend_llist_remove_tail(&ctx->keys);
	}
}

/* CONVERSIONS for integers */
static zend_long from_zval_integer_common(const zval *arr_value, ser_context *ctx)
{
	zend_long ret = 0;
	zval lzval;

	ZVAL_NULL(&lzval);
	if (Z_TYPE_P(arr_value) != IS_LONG) {
		ZVAL_COPY(&lzval, (zval *)arr_value);
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
		zend_long lval;
		double dval;

		if (!try_convert_to_string(&lzval)) {
			ctx->err.has_error = 1;
			break;
		}

		switch (is_numeric_string(Z_STRVAL(lzval), Z_STRLEN(lzval), &lval, &dval, 0)) {
		case IS_DOUBLE:
			zval_ptr_dtor_str(&lzval);
			ZVAL_DOUBLE(&lzval, dval);
			goto double_case;

		case IS_LONG:
			zval_ptr_dtor_str(&lzval);
			ZVAL_LONG(&lzval, lval);
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

	zval_ptr_dtor(&lzval);

	return ret;
}
void from_zval_write_int(const zval *arr_value, char *field, ser_context *ctx)
{
	zend_long lval;
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
static void from_zval_write_uint32(const zval *arr_value, char *field, ser_context *ctx)
{
	zend_long lval;
	uint32_t ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (sizeof(zend_long) > sizeof(uint32_t) && (lval < 0 || lval > 0xFFFFFFFF)) {
		do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
				"for an unsigned 32-bit integer");
		return;
	}

	ival = (uint32_t)lval;
	memcpy(field, &ival, sizeof(ival));
}
static void from_zval_write_net_uint16(const zval *arr_value, char *field, ser_context *ctx)
{
	zend_long lval;
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
	zend_long lval;
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

#if defined(SO_PASSCRED) || defined(LOCAL_CREDS_PERSISTENT) || defined(LOCAL_CREDS)
static void from_zval_write_pid_t(const zval *arr_value, char *field, ser_context *ctx)
{
	zend_long lval;
	pid_t ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (lval < 0 || (pid_t)lval != lval) { /* pid_t is signed */
		do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
				"for a pid_t value");
		return;
	}

	ival = (pid_t)lval;
	memcpy(field, &ival, sizeof(ival));
}
static void from_zval_write_uid_t(const zval *arr_value, char *field, ser_context *ctx)
{
	zend_long lval;
	uid_t ival;

	lval = from_zval_integer_common(arr_value, ctx);
	if (ctx->err.has_error) {
		return;
	}

	/* uid_t can be signed or unsigned (generally unsigned) */
	if ((uid_t)-1 > (uid_t)0) {
		if (sizeof(zend_long) > sizeof(uid_t) && (lval < 0 || (uid_t)lval != lval)) {
			do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
					"for a uid_t value");
			return;
		}
	} else {
		if (sizeof(zend_long) > sizeof(uid_t) && (uid_t)lval != lval) {
			do_from_zval_err(ctx, "%s", "given PHP integer is out of bounds "
					"for a uid_t value");
			return;
		}
	}

	ival = (uid_t)lval;
	memcpy(field, &ival, sizeof(ival));
}
#endif

void to_zval_read_int(const char *data, zval *zv, res_context *ctx)
{
	int ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (zend_long)ival);
}
static void to_zval_read_net_uint16(const char *data, zval *zv, res_context *ctx)
{
	uint16_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (zend_long)ntohs(ival));
}
static void to_zval_read_sa_family(const char *data, zval *zv, res_context *ctx)
{
	sa_family_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (zend_long)ival);
}
#if HAVE_IPV6
static void to_zval_read_unsigned(const char *data, zval *zv, res_context *ctx)
{
	unsigned ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (zend_long)ival);
}
static void to_zval_read_uint32(const char *data, zval *zv, res_context *ctx)
{
	uint32_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (zend_long)ival);
}
#endif
#if defined(SO_PASSCRED) || defined(LOCAL_CREDS_PERSISTENT) || defined(LOCAL_CREDS)
static void to_zval_read_pid_t(const char *data, zval *zv, res_context *ctx)
{
	pid_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (zend_long)ival);
}
static void to_zval_read_uid_t(const char *data, zval *zv, res_context *ctx)
{
	uid_t ival;
	memcpy(&ival, data, sizeof(ival));

	ZVAL_LONG(zv, (zend_long)ival);
}
#endif

/* CONVERSIONS for sockaddr */
static void from_zval_write_sin_addr(const zval *zaddr_str, char *inaddr, ser_context *ctx)
{
	int					res;
	struct sockaddr_in	saddr = {0};
	zend_string			*addr_str, *tmp_addr_str;

	addr_str = zval_get_tmp_string((zval *) zaddr_str, &tmp_addr_str);
	res = php_set_inet_addr(&saddr, ZSTR_VAL(addr_str), ctx->sock);
	if (res) {
		memcpy(inaddr, &saddr.sin_addr, sizeof saddr.sin_addr);
	} else {
		/* error already emitted, but let's emit another more relevant */
		do_from_zval_err(ctx, "could not resolve address '%s' to get an AF_INET "
				"address", ZSTR_VAL(addr_str));
	}

	zend_tmp_string_release(tmp_addr_str);
}
static void to_zval_read_sin_addr(const char *data, zval *zv, res_context *ctx)
{
	const struct in_addr *addr = (const struct in_addr *)data;
	socklen_t size = INET_ADDRSTRLEN;
	zend_string *str = zend_string_alloc(size - 1, 0);
	memset(ZSTR_VAL(str), '\0', size);

	ZVAL_NEW_STR(zv, str);

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
#if HAVE_IPV6
static void from_zval_write_sin6_addr(const zval *zaddr_str, char *addr6, ser_context *ctx)
{
	int					res;
	struct sockaddr_in6	saddr6 = {0};
	zend_string			*addr_str, *tmp_addr_str;

	addr_str = zval_get_tmp_string((zval *) zaddr_str, &tmp_addr_str);
	res = php_set_inet6_addr(&saddr6, ZSTR_VAL(addr_str), ctx->sock);
	if (res) {
		memcpy(addr6, &saddr6.sin6_addr, sizeof saddr6.sin6_addr);
	} else {
		/* error already emitted, but let's emit another more relevant */
		do_from_zval_err(ctx, "could not resolve address '%s' to get an AF_INET6 "
				"address", Z_STRVAL_P(zaddr_str));
	}

	zend_tmp_string_release(tmp_addr_str);
}
static void to_zval_read_sin6_addr(const char *data, zval *zv, res_context *ctx)
{
	const struct in6_addr *addr = (const struct in6_addr *)data;
	socklen_t size = INET6_ADDRSTRLEN;
	zend_string *str = zend_string_alloc(size - 1, 0);

	memset(ZSTR_VAL(str), '\0', size);

	ZVAL_NEW_STR(zv, str);

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
#endif /* HAVE_IPV6 */
static void from_zval_write_sun_path(const zval *path, char *sockaddr_un_c, ser_context *ctx)
{
	zend_string			*path_str, *tmp_path_str;
	struct sockaddr_un	*saddr = (struct sockaddr_un*)sockaddr_un_c;

	path_str = zval_get_tmp_string((zval *) path, &tmp_path_str);

	/* code in this file relies on the path being nul terminated, even though
	 * this is not required, at least on linux for abstract paths. It also
	 * assumes that the path is not empty */
	if (ZSTR_LEN(path_str) == 0) {
		do_from_zval_err(ctx, "%s", "the path is cannot be empty");
		zend_tmp_string_release(tmp_path_str);
		return;
	}
	if (ZSTR_LEN(path_str) >= sizeof(saddr->sun_path)) {
		do_from_zval_err(ctx, "the path is too long, the maximum permitted "
				"length is %zd", sizeof(saddr->sun_path) - 1);
		zend_tmp_string_release(tmp_path_str);
		return;
	}

	memcpy(&saddr->sun_path, ZSTR_VAL(path_str), ZSTR_LEN(path_str));
	saddr->sun_path[ZSTR_LEN(path_str)] = '\0';

	zend_tmp_string_release(tmp_path_str);
}
static void to_zval_read_sun_path(const char *data, zval *zv, res_context *ctx) {
	struct sockaddr_un	*saddr = (struct sockaddr_un*)data;
	char *nul_pos;

	nul_pos = memchr(&saddr->sun_path, '\0', sizeof(saddr->sun_path));
	if (nul_pos == NULL) {
		do_to_zval_err(ctx, "could not find a NUL in the path");
		return;
	}

	ZVAL_STRINGL(zv, saddr->sun_path, nul_pos - (char*)&saddr->sun_path);
}
static const field_descriptor descriptors_sockaddr_un[] = {
		{"family", sizeof("family"), 0, offsetof(struct sockaddr_un, sun_family), from_zval_write_sa_family, to_zval_read_sa_family},
		{"path", sizeof("path"), 0, 0, from_zval_write_sun_path, to_zval_read_sun_path},
		{0}
};
static void from_zval_write_sockaddr_un(const zval *container, char *sockaddr, ser_context *ctx)
{
	from_zval_write_aggregation(container, sockaddr, descriptors_sockaddr_un, ctx);
}
static void to_zval_read_sockaddr_un(const char *data, zval *zv, res_context *ctx)
{
	to_zval_read_aggregation(data, zv, descriptors_sockaddr_un, ctx);
}
static void from_zval_write_sockaddr_aux(const zval *container,
										 struct sockaddr **sockaddr_ptr,
										 socklen_t *sockaddr_len,
										 ser_context *ctx)
{
	int		family;
	zval	*elem;
	int		fill_sockaddr;

	*sockaddr_ptr = NULL;
	*sockaddr_len = 0;

	if (Z_TYPE_P(container) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
		return;
	}

	fill_sockaddr = param_get_bool(ctx, KEY_FILL_SOCKADDR, 1);

	if ((elem = zend_hash_str_find(Z_ARRVAL_P(container), "family", sizeof("family") - 1)) != NULL
			&& Z_TYPE_P(elem) != IS_NULL) {
		const char *node = "family";
		zend_llist_add_element(&ctx->keys, &node);
		from_zval_write_int(elem, (char*)&family, ctx);
		zend_llist_remove_tail(&ctx->keys);

		if (UNEXPECTED(ctx->err.has_error)) {
			return;
		}
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
			(*sockaddr_ptr)->sa_family = AF_INET;
		}
		break;

#if HAVE_IPV6
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
			(*sockaddr_ptr)->sa_family = AF_INET6;
		}
		break;
#endif /* HAVE_IPV6 */

	case AF_UNIX:
		if (ctx->sock->type != AF_UNIX) {
			do_from_zval_err(ctx, "the specified family (AF_UNIX) is not "
					"supported on this socket");
			return;
		}
		*sockaddr_ptr = accounted_ecalloc(1, sizeof(struct sockaddr_un), ctx);
		if (fill_sockaddr) {
			struct sockaddr_un *sock_un = (struct sockaddr_un*)*sockaddr_ptr;

			from_zval_write_sockaddr_un(container, (char*)*sockaddr_ptr, ctx);
			(*sockaddr_ptr)->sa_family = AF_UNIX;

			/* calculating length is more complicated here. Giving the size of
			 * struct sockaddr_un here and relying on the nul termination of
			 * sun_path does not work for paths in the abstract namespace. Note
			 * that we always assume the path is not empty and nul terminated */
			*sockaddr_len = offsetof(struct sockaddr_un, sun_path) +
					(sock_un->sun_path[0] == '\0'
					? (1 + strlen(&sock_un->sun_path[1]))
					: strlen(sock_un->sun_path));
		} else {
			*sockaddr_len = sizeof(struct sockaddr_un);
		}
		break;

	default:
		do_from_zval_err(ctx, "%s", "the only families currently supported are "
				"AF_INET, AF_INET6 and AF_UNIX");
		break;
	}
}
static void to_zval_read_sockaddr_aux(const char *sockaddr_c, zval *zv, res_context *ctx)
{
	const struct sockaddr *saddr = (struct sockaddr *)sockaddr_c;

	if (saddr->sa_family == 0) {
		ZVAL_NULL(zv);
		return;
	}

	array_init(zv);

	switch (saddr->sa_family) {
	case AF_INET:
		to_zval_read_sockaddr_in(sockaddr_c, zv, ctx);
		break;

#if HAVE_IPV6
	case AF_INET6:
		to_zval_read_sockaddr_in6(sockaddr_c, zv, ctx);
		break;
#endif /* HAVE_IPV6 */

	case AF_UNIX:
		to_zval_read_sockaddr_un(sockaddr_c, zv, ctx);
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
	size_t				data_len,
						req_space,
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

	if (entry->calc_space) {
		zval *data_elem;
		/* arr must be an array at this point */
		if ((data_elem = zend_hash_str_find(Z_ARRVAL_P(arr), "data", sizeof("data") - 1)) == NULL) {
			do_from_zval_err(ctx, "cmsghdr should have a 'data' element here");
			return;
		}
		data_len = entry->calc_space(data_elem, ctx);
		if (ctx->err.has_error) {
			return;
		}
	} else {
		data_len = entry->size;
	}
	req_space = CMSG_SPACE(data_len);
	space_left = *control_len - *offset;
	assert(*control_len >= *offset);

	if (space_left < req_space) {
		*control_buf = safe_erealloc(*control_buf, 2, req_space, *control_len);
		*control_len += 2 * req_space;
		memset((char *)*control_buf + *offset, '\0', *control_len - *offset);
		memcpy(&alloc->data, control_buf, sizeof *control_buf);
	}

	cmsghdr = (struct cmsghdr*)(((char*)*control_buf) + *offset);
	cmsghdr->cmsg_level	= level;
	cmsghdr->cmsg_type	= type;
	cmsghdr->cmsg_len	= CMSG_LEN(data_len);

	descriptor_data[0].from_zval = entry->from_array;
	from_zval_write_aggregation(arr, (char*)CMSG_DATA(cmsghdr), descriptor_data, ctx);

	*offset += req_space;
}
static void from_zval_write_control_array(const zval *arr, char *msghdr_c, ser_context *ctx)
{
	char				buf[sizeof("element #4294967295")];
	char				*bufp = buf;
	zval				*elem;
	uint32_t			i = 0;
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

	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arr), elem) {
		if (ctx->err.has_error) {
			break;
		}

		if ((size_t)snprintf(buf, sizeof(buf), "element #%u", (unsigned)i++) >= sizeof(buf)) {
			memcpy(buf, "element", sizeof("element"));
		}
		zend_llist_add_element(&ctx->keys, &bufp);

		from_zval_write_control(elem, &control_buf, alloc, &control_len, &cur_offset, ctx);

		zend_llist_remove_tail(&ctx->keys);
	} ZEND_HASH_FOREACH_END();

	msg->msg_control = control_buf;
	msg->msg_controllen = cur_offset; /* not control_len, which may be larger */
}
static void to_zval_read_cmsg_data(const char *cmsghdr_c, zval *zv, res_context *ctx)
{
	const struct cmsghdr	*cmsg = (const struct cmsghdr *)cmsghdr_c;
	ancillary_reg_entry		*entry;
	size_t					len,
							*len_p = &len;

	entry = get_ancillary_reg_entry(cmsg->cmsg_level, cmsg->cmsg_type);
	if (entry == NULL) {
		do_to_zval_err(ctx, "cmsghdr with level %d and type %d not supported",
				cmsg->cmsg_level, cmsg->cmsg_type);
		return;
	}
	if (CMSG_LEN(entry->size) > cmsg->cmsg_len) {
		do_to_zval_err(ctx, "the cmsghdr structure is unexpectedly small; "
				"expected a length of at least " ZEND_LONG_FMT ", but got " ZEND_LONG_FMT,
				(zend_long)CMSG_LEN(entry->size), (zend_long)cmsg->cmsg_len);
		return;
	}

	len = (size_t)cmsg->cmsg_len; /* use another var because type of cmsg_len varies */

	if (zend_hash_str_add_ptr(&ctx->params, KEY_CMSG_LEN, sizeof(KEY_CMSG_LEN) - 1, len_p) == NULL) {
		do_to_zval_err(ctx, "%s", "could not set parameter " KEY_CMSG_LEN);
		return;
	}

	entry->to_array((const char *)CMSG_DATA(cmsg), zv, ctx);

	zend_hash_str_del(&ctx->params, KEY_CMSG_LEN, sizeof(KEY_CMSG_LEN) - 1);
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

	array_init(zv);

	for (cmsg = CMSG_FIRSTHDR(msg);
			cmsg != NULL && !ctx->err.has_error;
			cmsg = CMSG_NXTHDR(msg, cmsg)) {
		zval *elem, tmp;

		ZVAL_NULL(&tmp);
		elem = zend_hash_next_index_insert(Z_ARRVAL_P(zv), &tmp);

		if ((size_t)snprintf(buf, sizeof(buf), "element #%u", (unsigned)i++) >= sizeof(buf)) {
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
		to_zval_read_sockaddr_aux(name, zv, ctx);
	}
}
static void from_zval_write_msghdr_buffer_size(const zval *elem, char *msghdr_c, ser_context *ctx)
{
	zend_long lval;
	struct msghdr *msghdr = (struct msghdr *)msghdr_c;

	lval = from_zval_integer_common(elem, ctx);
	if (ctx->err.has_error) {
		return;
	}

	if (lval < 0 || (zend_ulong)lval > MAX_USER_BUFF_SIZE) {
		do_from_zval_err(ctx, "the buffer size must be between 1 and " ZEND_LONG_FMT "; "
				"given " ZEND_LONG_FMT, (zend_long)MAX_USER_BUFF_SIZE, lval);
		return;
	}

	msghdr->msg_iovlen = 1;
	msghdr->msg_iov = accounted_emalloc(sizeof(*msghdr->msg_iov) * 1, ctx);
	msghdr->msg_iov[0].iov_base = accounted_emalloc((size_t)lval, ctx);
	msghdr->msg_iov[0].iov_len = (size_t)lval;
}
static void from_zval_write_iov_array_aux(zval *elem, unsigned i, void **args, ser_context *ctx)
{
	struct msghdr	*msg = args[0];
	zend_string     *str, *tmp_str;

	str = zval_get_tmp_string(elem, &tmp_str);

	msg->msg_iov[i - 1].iov_base = accounted_emalloc(ZSTR_LEN(str), ctx);
	msg->msg_iov[i - 1].iov_len = ZSTR_LEN(str);
	memcpy(msg->msg_iov[i - 1].iov_base, ZSTR_VAL(str), ZSTR_LEN(str));

	zend_tmp_string_release(tmp_str);
}
static void from_zval_write_iov_array(const zval *arr, char *msghdr_c, ser_context *ctx)
{
	int				num_elem;
	struct msghdr	*msg = (struct msghdr*)msghdr_c;

	if (Z_TYPE_P(arr) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
		return;
	}

	num_elem = zend_hash_num_elements(Z_ARRVAL_P(arr));
	if (num_elem == 0) {
		return;
	}

	msg->msg_iov = accounted_safe_ecalloc(num_elem, sizeof *msg->msg_iov, 0, ctx);
	msg->msg_iovlen = (size_t)num_elem;

	from_array_iterate(arr, from_zval_write_iov_array_aux, (void**)&msg, ctx);
}
static void from_zval_write_controllen(const zval *elem, char *msghdr_c, ser_context *ctx)
{
	struct msghdr *msghdr = (struct msghdr *)msghdr_c;
	uint32_t len;

	/* controllen should be an unsigned with at least 32-bit. Let's assume
	 * this least common denominator
	 */
	from_zval_write_uint32(elem, (char*)&len, ctx);
	if (ctx->err.has_error) {
		return;
	}
	if (len == 0) {
		do_from_zval_err(ctx, "controllen cannot be 0");
		return;
	}
	msghdr->msg_control = accounted_emalloc(len, ctx);
	msghdr->msg_controllen = len;
}
void from_zval_write_msghdr_send(const zval *container, char *msghdr_c, ser_context *ctx)
{
	static const field_descriptor descriptors[] = {
			{"name", sizeof("name"), 0, 0, from_zval_write_name, 0},
			{"iov", sizeof("iov"), 0, 0, from_zval_write_iov_array, 0},
			{"control", sizeof("control"), 0, 0, from_zval_write_control_array, 0},
			{0}
	};

	from_zval_write_aggregation(container, msghdr_c, descriptors, ctx);
}
void from_zval_write_msghdr_recv(const zval *container, char *msghdr_c, ser_context *ctx)
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

	if (zend_hash_str_add_ptr(&ctx->params, KEY_FILL_SOCKADDR, sizeof(KEY_FILL_SOCKADDR) - 1, (void *)falsevp) == NULL) {
		do_from_zval_err(ctx, "could not add fill_sockaddr; this is a bug");
		return;
	}

	from_zval_write_aggregation(container, msghdr_c, descriptors, ctx);

	zend_hash_str_del(&ctx->params, KEY_FILL_SOCKADDR, sizeof(KEY_FILL_SOCKADDR) - 1);
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
	ssize_t				*recvmsg_ret,
						bytes_left;
	uint32_t			i;

	if (iovlen > UINT_MAX) {
		do_to_zval_err(ctx, "unexpectedly large value for iov_len: %lu",
				(unsigned long)iovlen);
	}
	array_init_size(zv, (uint32_t)iovlen);

	if ((recvmsg_ret = zend_hash_str_find_ptr(&ctx->params, KEY_RECVMSG_RET, sizeof(KEY_RECVMSG_RET) - 1)) == NULL) {
		do_to_zval_err(ctx, "recvmsg_ret not found in params. This is a bug");
		return;
	}
	bytes_left = *recvmsg_ret;

	for (i = 0; bytes_left > 0 && i < (uint32_t)iovlen; i++) {
		zval elem;
		size_t len = MIN(msghdr->msg_iov[i].iov_len, (size_t)bytes_left);
		zend_string	*buf = zend_string_alloc(len, 0);

		memcpy(ZSTR_VAL(buf), msghdr->msg_iov[i].iov_base, ZSTR_LEN(buf));
		ZSTR_VAL(buf)[ZSTR_LEN(buf)] = '\0';

		ZVAL_NEW_STR(&elem, buf);
		add_next_index_zval(zv, &elem);
		bytes_left -= len;
	}
}
void to_zval_read_msghdr(const char *msghdr_c, zval *zv, res_context *ctx)
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

#if defined(IPV6_PKTINFO) && HAVE_IPV6
/* CONVERSIONS for if_index */
static void from_zval_write_ifindex(const zval *zv, char *uinteger, ser_context *ctx)
{
	unsigned ret = 0;

	if (Z_TYPE_P(zv) == IS_LONG) {
		if (Z_LVAL_P(zv) < 0 || (zend_ulong)Z_LVAL_P(zv) > UINT_MAX) { /* allow 0 (unspecified interface) */
			do_from_zval_err(ctx, "the interface index cannot be negative or "
					"larger than %u; given " ZEND_LONG_FMT, UINT_MAX, Z_LVAL_P(zv));
		} else {
			ret = (unsigned)Z_LVAL_P(zv);
		}
	} else {
		zend_string *str, *tmp_str;

		str = zval_get_tmp_string((zval *) zv, &tmp_str);

#if HAVE_IF_NAMETOINDEX
		ret = if_nametoindex(ZSTR_VAL(str));
		if (ret == 0) {
			do_from_zval_err(ctx, "no interface with name \"%s\" could be found", ZSTR_VAL(str));
		}
#elif defined(SIOCGIFINDEX)
		{
			struct ifreq ifr;
			if (strlcpy(ifr.ifr_name, ZSTR_VAL(str), sizeof(ifr.ifr_name))
					>= sizeof(ifr.ifr_name)) {
				do_from_zval_err(ctx, "the interface name \"%s\" is too large ", ZSTR_VAL(str));
			} else if (ioctl(ctx->sock->bsd_socket, SIOCGIFINDEX, &ifr) < 0) {
				if (errno == ENODEV) {
					do_from_zval_err(ctx, "no interface with name \"%s\" could be "
							"found", ZSTR_VAL(str));
				} else {
					do_from_zval_err(ctx, "error fetching interface index for "
							"interface with name \"%s\" (errno %d)",
							ZSTR_VAL(str), errno);
				}
			} else {
				ret = (unsigned)ifr.ifr_ifindex;
			}
		}
#else
		do_from_zval_err(ctx,
				"this platform does not support looking up an interface by "
				"name, an integer interface index must be supplied instead");
#endif

		zend_tmp_string_release(tmp_str);
	}

	if (!ctx->err.has_error) {
		memcpy(uinteger, &ret, sizeof(ret));
	}
}

/* CONVERSIONS for struct in6_pktinfo */
static const field_descriptor descriptors_in6_pktinfo[] = {
		{"addr", sizeof("addr"), 1, offsetof(struct in6_pktinfo, ipi6_addr), from_zval_write_sin6_addr, to_zval_read_sin6_addr},
		{"ifindex", sizeof("ifindex"), 1, offsetof(struct in6_pktinfo, ipi6_ifindex), from_zval_write_ifindex, to_zval_read_unsigned},
		{0}
};
void from_zval_write_in6_pktinfo(const zval *container, char *in6_pktinfo_c, ser_context *ctx)
{
	from_zval_write_aggregation(container, in6_pktinfo_c, descriptors_in6_pktinfo, ctx);
}
void to_zval_read_in6_pktinfo(const char *data, zval *zv, res_context *ctx)
{
	array_init_size(zv, 2);

	to_zval_read_aggregation(data, zv, descriptors_in6_pktinfo, ctx);
}
#endif

/* CONVERSIONS for struct ucred */
#if defined(SO_PASSCRED) || defined(LOCAL_CREDS_PERSISTENT) || defined(LOCAL_CREDS)
static const field_descriptor descriptors_ucred[] = {
#if defined(LOCAL_CREDS_PERSISTENT)
		{"pid", sizeof("pid"), 1, offsetof(struct sockcred2, sc_pid), from_zval_write_pid_t, to_zval_read_pid_t},
		{"uid", sizeof("uid"), 1, offsetof(struct sockcred2, sc_euid), from_zval_write_uid_t, to_zval_read_uid_t},
		/* the type gid_t is the same as uid_t: */
		{"gid", sizeof("gid"), 1, offsetof(struct sockcred2, sc_egid), from_zval_write_uid_t, to_zval_read_uid_t},
#elif defined(LOCAL_CREDS)
		{"pid", sizeof("pid"), 1, offsetof(struct sockcred, sc_pid), from_zval_write_pid_t, to_zval_read_pid_t},
		{"uid", sizeof("uid"), 1, offsetof(struct sockcred, sc_euid), from_zval_write_uid_t, to_zval_read_uid_t},
		/* the type gid_t is the same as uid_t: */
		{"gid", sizeof("gid"), 1, offsetof(struct sockcred, sc_egid), from_zval_write_uid_t, to_zval_read_uid_t},
#elif defined(ANC_CREDS_CMSGCRED)
		{"pid", sizeof("pid"), 1, offsetof(struct cmsgcred, cmcred_pid), from_zval_write_pid_t, to_zval_read_pid_t},
		{"uid", sizeof("uid"), 1, offsetof(struct cmsgcred, cmcred_uid), from_zval_write_uid_t, to_zval_read_uid_t},
		/* assume the type gid_t is the same as uid_t: */
		{"gid", sizeof("gid"), 1, offsetof(struct cmsgcred, cmcred_gid), from_zval_write_uid_t, to_zval_read_uid_t},
#elif defined(SO_PASSCRED)
		{"pid", sizeof("pid"), 1, offsetof(struct ucred, pid), from_zval_write_pid_t, to_zval_read_pid_t},
		{"uid", sizeof("uid"), 1, offsetof(struct ucred, uid), from_zval_write_uid_t, to_zval_read_uid_t},
		/* assume the type gid_t is the same as uid_t: */
		{"gid", sizeof("gid"), 1, offsetof(struct ucred, gid), from_zval_write_uid_t, to_zval_read_uid_t},
#endif
		{0}
};
void from_zval_write_ucred(const zval *container, char *ucred_c, ser_context *ctx)
{
	from_zval_write_aggregation(container, ucred_c, descriptors_ucred, ctx);
}
void to_zval_read_ucred(const char *data, zval *zv, res_context *ctx)
{
	array_init_size(zv, 3);

	to_zval_read_aggregation(data, zv, descriptors_ucred, ctx);
}
#endif

/* CONVERSIONS for SCM_RIGHTS */
#ifdef SCM_RIGHTS
size_t calculate_scm_rights_space(const zval *arr, ser_context *ctx)
{
	int num_elems;

	if (Z_TYPE_P(arr) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
		return (size_t)-1;
	}

	num_elems = zend_hash_num_elements(Z_ARRVAL_P(arr));
	if (num_elems == 0) {
		do_from_zval_err(ctx, "%s", "expected at least one element in this array");
		return (size_t)-1;
	}

	return zend_hash_num_elements(Z_ARRVAL_P(arr)) * sizeof(int);
}
static void from_zval_write_fd_array_aux(zval *elem, unsigned i, void **args, ser_context *ctx)
{
	int *iarr = args[0];

	if (Z_TYPE_P(elem) == IS_OBJECT && Z_OBJCE_P(elem) == socket_ce) {
		php_socket *sock = Z_SOCKET_P(elem);
		if (IS_INVALID_SOCKET(sock)) {
			do_from_zval_err(ctx, "socket is already closed");
			return;
		}

		iarr[i] = sock->bsd_socket;
		return;
	}

	if (Z_TYPE_P(elem) == IS_RESOURCE) {
		php_stream *stream;

		stream = (php_stream *)zend_fetch_resource2_ex(elem, NULL, php_file_le_stream(), php_file_le_pstream());
		if (stream == NULL) {
			do_from_zval_err(ctx, "resource is not a stream");
			return;
		}

		if (php_stream_cast(stream, PHP_STREAM_AS_FD, (void **)&iarr[i - 1], REPORT_ERRORS) == FAILURE) {
			do_from_zval_err(ctx, "cast stream to file descriptor failed");
			return;
		}
	} else {
		do_from_zval_err(ctx, "expected a Socket object or a stream resource");
	}
}
void from_zval_write_fd_array(const zval *arr, char *int_arr, ser_context *ctx)
{
	if (Z_TYPE_P(arr) != IS_ARRAY) {
		do_from_zval_err(ctx, "%s", "expected an array here");
		return;
	}

	from_array_iterate(arr, &from_zval_write_fd_array_aux, (void**)&int_arr, ctx);
}
void to_zval_read_fd_array(const char *data, zval *zv, res_context *ctx)
{
	size_t			*cmsg_len;
	int				num_elems,
					i;
	struct cmsghdr	*dummy_cmsg = 0;
	size_t			data_offset;

	data_offset = (unsigned char *)CMSG_DATA(dummy_cmsg)
			- (unsigned char *)dummy_cmsg;

	if ((cmsg_len = zend_hash_str_find_ptr(&ctx->params, KEY_CMSG_LEN, sizeof(KEY_CMSG_LEN) - 1)) == NULL) {
		do_to_zval_err(ctx, "could not get value of parameter " KEY_CMSG_LEN);
		return;
	}

	if (*cmsg_len < data_offset) {
		do_to_zval_err(ctx, "length of cmsg is smaller than its data member "
				"offset (" ZEND_LONG_FMT " vs " ZEND_LONG_FMT ")", (zend_long)*cmsg_len, (zend_long)data_offset);
		return;
	}
	num_elems = (*cmsg_len - data_offset) / sizeof(int);

	array_init_size(zv, num_elems);

	for (i = 0; i < num_elems; i++) {
		zval		elem;
		int			fd;
		struct stat	statbuf;

		fd = *((int *)data + i);

		/* determine whether we have a socket */
		if (fstat(fd, &statbuf) == -1) {
			do_to_zval_err(ctx, "error creating resource for received file "
					"descriptor %d: fstat() call failed with errno %d", fd, errno);
			return;
		}
		if (S_ISSOCK(statbuf.st_mode)) {
			object_init_ex(&elem, socket_ce);
			php_socket *sock = Z_SOCKET_P(&elem);

			socket_import_file_descriptor(fd, sock);
		} else {
			php_stream *stream = php_stream_fopen_from_fd(fd, "rw", NULL);
			php_stream_to_zval(stream, &elem);
		}

		add_next_index_zval(zv, &elem);
	}
}
#endif

/* ENTRY POINT for conversions */
static void free_from_zval_allocation(void *alloc_ptr_ptr)
{
	efree(*(void**)alloc_ptr_ptr);
}
void *from_zval_run_conversions(const zval			*container,
								php_socket			*sock,
								from_zval_write_field	*writer,
								size_t				struct_size,
								const char			*top_name,
								zend_llist			**allocations /* out */,
								struct err_s			*err /* in/out */)
{
	ser_context ctx;
	char *structure;

	*allocations = NULL;

	if (err->has_error) {
		return NULL;
	}

	memset(&ctx, 0, sizeof(ctx));
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
zval *to_zval_run_conversions(const char *structure,
							  to_zval_read_field *reader,
							  const char *top_name,
							  const struct key_value *key_value_pairs,
							  struct err_s *err, zval *zv)
{
	res_context				ctx;
	const struct key_value	*kv;

	if (err->has_error) {
		return NULL;
	}

	memset(&ctx, 0, sizeof(ctx));
	zend_llist_init(&ctx.keys, sizeof(const char *), NULL, 0);
	zend_llist_add_element(&ctx.keys, &top_name);

	zend_hash_init(&ctx.params, 8, NULL, NULL, 0);
	for (kv = key_value_pairs; kv->key != NULL; kv++) {
		zend_hash_str_update_ptr(&ctx.params, kv->key, kv->key_size - 1, kv->value);
	}

	ZVAL_NULL(zv);
	/* main call */
	reader(structure, zv, &ctx);

	if (ctx.err.has_error) {
		zval_ptr_dtor(zv);
		ZVAL_UNDEF(zv);
		*err = ctx.err;
	}

	zend_llist_destroy(&ctx.keys);
	zend_hash_destroy(&ctx.params);

	return Z_ISUNDEF_P(zv)? NULL : zv;
}
