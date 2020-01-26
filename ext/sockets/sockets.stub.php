<?php

function socket_select(?array &$read_fds, ?array &$write_fds, ?array &$except_fds, ?int $tv_sec, int $tv_usec = 0): int|false {}

/** @return resource|false */
function socket_create_listen(int $port, int $backlog = 128) {}

/**
 * @param resource $socket
 * @return resource|false
 */
function socket_accept($socket) {}

/** @param resource $socket */
function socket_set_nonblock($socket): bool {}

/** @param resource $socket */
function socket_set_block($socket): bool {}

/** @param resource $socket */
function socket_listen($socket, int $backlog = 0): bool {}

/** @param resource $socket */
function socket_close($socket): void {}

/** @param resource $socket */
function socket_write($socket, string $buf, int $length = 0): int|false {}

/** @param resource $socket */
function socket_read($socket, int $length, int $type = PHP_BINARY_READ): string|false {}

/** @param resource $socket */
function socket_getsockname($socket, &$addr, &$port = UNKNOWN): bool {}

/** @param resource $socket */
function socket_getpeername($socket, &$addr, &$port = UNKNOWN): bool {}

/** @return resource|false */
function socket_create(int $domain, int $type, int $protocol) {}

/** @param resource $socket */
function socket_connect($socket, string $addr, int $port = 0): bool {}

function socket_strerror(int $errno): string {}

/** @param resource $socket */
function socket_bind($socket, string $addr, int $port = 0): bool {}

/** @param resource $socket */
function socket_recv($socket, &$buf, int $len, int $flags): int|false {}

/** @param resource $socket */
function socket_send($socket, string $buf, int $len, int $flags): int|false {}

/** @param resource $socket */
function socket_recvfrom($socket, &$buf, int $len, int $flags, &$name, &$port = UNKNOWN): int|false {}

/** @param resource $socket */
function socket_sendto($socket, string $buf, int $len, int $flags, string $addr, int $port = 0): int|false {}

/** @param resource $socket */
function socket_get_option($socket, int $level, int $optname): array|int|false {}

/** @param resource $socket */
function socket_set_option($socket, int $level, int $optname, $optval): bool {}

#ifdef HAVE_SOCKETPAIR
function socket_create_pair(int $domain, int $type, int $protocol, &$fd): bool|null {}
#endif

#ifdef HAVE_SHUTDOWN
/** @param resource $socket */
function socket_shutdown($socket, int $how = 2): bool {}
#endif

/** @param resource $socket */
function socket_last_error($socket = UNKNOWN): int {}

/** @param resource $socket */
function socket_clear_error($socket = UNKNOWN): void {}

/**
 * @param resource $stream
 * @return resource|false
 */
function socket_import_stream($stream) {}

/** @param resource $socket */
function socket_export_stream($socket) {}

/** @param resource $socket */
function socket_sendmsg($socket, array $msghdr, int $flags = 0): int|false {}

/** @param resource $socket */
function socket_recvmsg($socket, array &$msghdr, int $flags = 0): int|false {}

function socket_cmsg_space(int $level, int $type, int $n = 0): ?int {}

function socket_addrinfo_lookup(string $host, string $service = UNKNOWN, array $hints = UNKNOWN): array|false {}

/**
 * @param resource $addr
 * @return resource|false
 */
function socket_addrinfo_connect($addr) {}

/**
 * @param resource $addr
 * @return resource|false
 */
function socket_addrinfo_bind($addr) {}

/** @param resource $addr */
function socket_addrinfo_explain($addr): array {}

#ifdef PHP_WIN32
/** @param resource $socket */
function socket_wsaprotocol_info_export($socket, int $target_pid): string|false {}

/** @return resource|false */
function socket_wsaprotocol_info_import(string $info_id) {}

function socket_wsaprotocol_info_release(string $info_id): bool {}
#endif
