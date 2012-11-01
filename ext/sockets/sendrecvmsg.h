#include <php.h>

PHP_FUNCTION(socket_sendmsg);
PHP_FUNCTION(socket_recvmsg);
PHP_FUNCTION(socket_cmsg_space);

void _socket_sendrecvmsg_init(INIT_FUNC_ARGS);
void _socket_sendrecvmsg_shutdown(SHUTDOWN_FUNC_ARGS);
