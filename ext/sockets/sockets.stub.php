<?php

/** @generate-function-entries */

final class Socket
{
}

final class AddressInfo
{
}

function socket_select(?array &$read, ?array &$write, ?array &$except, ?int $seconds, int $microseconds = 0): int|false {}

function socket_create_listen(int $port, int $backlog = 128): Socket|false {}

function socket_accept(Socket $socket): Socket|false {}

function socket_set_nonblock(Socket $socket): bool {}

function socket_set_block(Socket $socket): bool {}

function socket_listen(Socket $socket, int $backlog = 0): bool {}

function socket_close(Socket $socket): void {}

function socket_write(Socket $socket, string $data, ?int $length = null): int|false {}

function socket_read(Socket $socket, int $length, int $mode = PHP_BINARY_READ): string|false {}

/**
 * @param string $address
 * @param int $port
 */
function socket_getsockname(Socket $socket, &$address, &$port = null): bool {}

/**
 * @param string $address
 * @param int $port
 */
function socket_getpeername(Socket $socket, &$address, &$port = null): bool {}

function socket_create(int $domain, int $type, int $protocol): Socket|false {}

function socket_connect(Socket $socket, string $address, ?int $port = null): bool {}

function socket_strerror(int $error_code): string {}

function socket_bind(Socket $socket, string $address, int $port = 0): bool {}

/** @param string|null $data */
function socket_recv(Socket $socket, &$data, int $length, int $flags): int|false {}

function socket_send(Socket $socket, string $data, int $length, int $flags): int|false {}

/**
 * @param string $data
 * @param string $address
 * @param int $port
 */
function socket_recvfrom(Socket $socket, &$data, int $length, int $flags, &$address, &$port = null): int|false {}

function socket_sendto(Socket $socket, string $data, int $length, int $flags, string $address, ?int $port = null): int|false {}

function socket_get_option(Socket $socket, int $level, int $option): array|int|false {}

/** @alias socket_get_option */
function socket_getopt(Socket $socket, int $level, int $option): array|int|false {}

/** @param array|string|int $value */
function socket_set_option(Socket $socket, int $level, int $option, $value): bool {}

/**
 * @param array|string|int $value
 * @alias socket_set_option
 */
function socket_setopt(Socket $socket, int $level, int $option, $value): bool {}

#ifdef HAVE_SOCKETPAIR
/** @param array $pair */
function socket_create_pair(int $domain, int $type, int $protocol, &$pair): bool {}
#endif

#ifdef HAVE_SHUTDOWN
function socket_shutdown(Socket $socket, int $mode = 2): bool {}
#endif

function socket_last_error(?Socket $socket = null): int {}

function socket_clear_error(?Socket $socket = null): void {}

/** @param resource $stream */
function socket_import_stream($stream): Socket|false {}

/** @return resource|false */
function socket_export_stream(Socket $socket) {}

function socket_sendmsg(Socket $socket, array $message, int $flags = 0): int|false {}

function socket_recvmsg(Socket $socket, array &$message, int $flags = 0): int|false {}

function socket_cmsg_space(int $level, int $type, int $num = 0): ?int {}

function socket_addrinfo_lookup(string $host, ?string $service = null, array $hints = []): array|false {}

function socket_addrinfo_connect(AddressInfo $address): Socket|false {}

function socket_addrinfo_bind(AddressInfo $address): Socket|false {}

function socket_addrinfo_explain(AddressInfo $address): array {}

#ifdef PHP_WIN32
function socket_wsaprotocol_info_export(Socket $socket, int $process_id): string|false {}

function socket_wsaprotocol_info_import(string $info_id): Socket|false {}

function socket_wsaprotocol_info_release(string $info_id): bool {}
#endif
