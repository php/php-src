#ifndef PHP_SOCK_CONVERSIONS_H
#define PHP_SOCK_CONVERSIONS_H 1

#include <php.h>

#ifndef PHP_WIN32
# include <netinet/in.h>
# include <sys/socket.h>
# if defined(__FreeBSD__) || defined(__NetBSD__)
#  include <sys/un.h>
#  if defined(__FreeBSD__)
     // we can't fully implement the ancillary data feature with
     // the legacy sockcred/LOCAL_CREDS pair (due to lack of process
     // id handling), so we disable it since only the
     // sockcred2/LOCAL_CREDS_PERSISTENT pair can address it.
#    undef LOCAL_CREDS
#  endif
# endif
#else
# include <Ws2tcpip.h>
#endif

#include "php_sockets.h"

/* TYPE DEFINITIONS */
struct err_s {
	int		has_error;
	char	*msg;
	int		level;
	int		should_free;
};

struct key_value {
	const char	*key;
	unsigned	key_size;
	void		*value;
};

/* the complete types of these two are not relevant to the outside */
typedef struct _ser_context ser_context;
typedef struct _res_context res_context;

#define KEY_RECVMSG_RET "recvmsg_ret"

typedef void (from_zval_write_field)(const zval *arr_value, char *field, ser_context *ctx);
typedef void (to_zval_read_field)(const char *data, zval *zv, res_context *ctx);

/* VARIABLE DECLARATIONS */
extern const struct key_value empty_key_value_list[];

/* AUX FUNCTIONS */
void err_msg_dispose(struct err_s *err);
void allocations_dispose(zend_llist **allocations);

/* CONVERSION FUNCTIONS */
void from_zval_write_int(const zval *arr_value, char *field, ser_context *ctx);
void to_zval_read_int(const char *data, zval *zv, res_context *ctx);

#ifdef IPV6_PKTINFO
void from_zval_write_in6_pktinfo(const zval *container, char *in6_pktinfo_c, ser_context *ctx);
void to_zval_read_in6_pktinfo(const char *data, zval *zv, res_context *ctx);
#endif

#if defined(SO_PASSCRED) || defined(LOCAL_CREDS_PERSISTENT) || defined(LOCAL_CREDS)
void from_zval_write_ucred(const zval *container, char *ucred_c, ser_context *ctx);
void to_zval_read_ucred(const char *data, zval *zv, res_context *ctx);
#endif

#ifdef SCM_RIGHTS
size_t calculate_scm_rights_space(const zval *arr, ser_context *ctx);
void from_zval_write_fd_array(const zval *arr, char *int_arr, ser_context *ctx);
void to_zval_read_fd_array(const char *data, zval *zv, res_context *ctx);
#endif

void from_zval_write_msghdr_send(const zval *container, char *msghdr_c, ser_context *ctx);
void from_zval_write_msghdr_recv(const zval *container, char *msghdr_c, ser_context *ctx);
void to_zval_read_msghdr(const char *msghdr_c, zval *zv, res_context *ctx);

/* ENTRY POINTS FOR CONVERSIONS */
void *from_zval_run_conversions(const zval				*container,
								php_socket				*sock,
								from_zval_write_field	*writer,
								size_t					struct_size,
								const char				*top_name,
								zend_llist				**allocations /* out */,
								struct err_s			*err /* in/out */);

zval *to_zval_run_conversions(const char				*structure,
							  to_zval_read_field		*reader,
							  const char				*top_name,
							  const struct key_value	*key_value_pairs,
							  struct err_s				*err, zval *zv);

#endif
