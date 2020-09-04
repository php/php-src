<?php

/** @generate-function-entries */

final class Socket
{
}

final class AddressInfo
{
}

function socket_select(?array &$read_fds, ?array &$write_fds, ?array &$except_fds, ?int $tv_sec, int $tv_usec = 0): int|false {}

function socket_create_listen(int $port, int $backlog = 128): Socket|false {}

function socket_accept(Socket $socket): Socket|false {}

function socket_set_nonblock(Socket $socket): bool {}

function socket_set_block(Socket $socket): bool {}

function socket_listen(Socket $socket, int $backlog = 0): bool {}

function socket_close(Socket $socket): void {}

function socket_write(Socket $socket, string $buf, ?int $length = null): int|false {}

function socket_read(Socket $socket, int $length, int $type = PHP_BINARY_READ): string|false {}

/**
 * @param string $addr
 * @param int $port
 */
function socket_getsockname(Socket $socket, &$addr, &$port = null): bool {}

/**
 * @param string $addr
 * @param int $port
 */
function socket_getpeername(Socket $socket, &$addr, &$port = null): bool {}

function socket_create(int $domain, int $type, int $protocol): Socket|false {}

function socket_connect(Socket $socket, string $addr, ?int $port = null): bool {}

function socket_strerror(int $errno): string {}

function socket_bind(Socket $socket, string $addr, int $port = 0): bool {}

/** @param string|null $buf */
function socket_recv(Socket $socket, &$buf, int $len, int $flags): int|false {}

function socket_send(Socket $socket, string $buf, int $len, int $flags): int|false {}

/**
 * @param string $buf
 * @param string $name
 * @param int $port
 */
function socket_recvfrom(Socket $socket, &$buf, int $len, int $flags, &$name, &$port = null): int|false {}

function socket_sendto(Socket $socket, string $buf, int $len, int $flags, string $addr, ?int $port = null): int|false {}

function socket_get_option(Socket $socket, int $level, int $optname): array|int|false {}

/** @alias socket_get_option */
function socket_getopt(Socket $socket, int $level, int $optname): array|int|false {}

/** @param array|string|int $optval */
function socket_set_option(Socket $socket, int $level, int $optname, $optval): bool {}

/**
 * @param array|string|int $optval
 * @alias socket_set_option
 */
function socket_setopt(Socket $socket, int $level, int $optname, $optval): bool {}

#ifdef HAVE_SOCKETPAIR
/** @param array $fd */
function socket_create_pair(int $domain, int $type, int $protocol, &$fd): bool|null {}
#endif

#ifdef HAVE_SHUTDOWN
function socket_shutdown(Socket $socket, int $how = 2): bool {}
#endif

function socket_last_error(?Socket $socket = null): int {}

function socket_clear_error(?Socket $socket = null): void {}

/** @param resource $stream */
function socket_import_stream($stream): Socket|false {}

/** @return resource|false */
function socket_export_stream(Socket $socket) {}

function socket_sendmsg(Socket $socket, array $msghdr, int $flags = 0): int|false {}

function socket_recvmsg(Socket $socket, array &$msghdr, int $flags = 0): int|false {}

function socket_cmsg_space(int $level, int $type, int $n = 0): ?int {}

function socket_addrinfo_lookup(string $host, ?string $service = null, array $hints = []): array|false {}

function socket_addrinfo_connect(AddressInfo $addr): Socket|false {}

function socket_addrinfo_bind(AddressInfo $addr): Socket|false {}

function socket_addrinfo_explain(AddressInfo $addr): array {}

#ifdef PHP_WIN32
function socket_wsaprotocol_info_export(Socket $socket, int $target_pid): string|false {}

function socket_wsaprotocol_info_import(string $info_id): Socket|false {}

function socket_wsaprotocol_info_release(string $info_id): bool {}
#endif
