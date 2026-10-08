<?php

$socketPath = tempnam(sys_get_temp_dir(), 'php_socket_');
if ($socketPath === false) {
    die('skip unable to create a temporary socket path');
}
unlink($socketPath);

$socket = socket_create(AF_UNIX, SOCK_DGRAM, 0);
if ($socket === false || !@socket_bind($socket, $socketPath)) {
    $error = socket_last_error($socket !== false ? $socket : null);
    if ($socket !== false) {
        socket_close($socket);
    }
    @unlink($socketPath);

    $unavailableErrors = array_filter([
        defined('SOCKET_EPERM') ? SOCKET_EPERM : null,
        defined('SOCKET_EACCES') ? SOCKET_EACCES : null,
        defined('SOCKET_EAFNOSUPPORT') ? SOCKET_EAFNOSUPPORT : null,
        defined('SOCKET_EPROTONOSUPPORT') ? SOCKET_EPROTONOSUPPORT : null,
        defined('SOCKET_ESOCKTNOSUPPORT') ? SOCKET_ESOCKTNOSUPPORT : null,
        defined('SOCKET_EOPNOTSUPP') ? SOCKET_EOPNOTSUPP : null,
    ], static fn(?int $error): bool => $error !== null);
    if (in_array($error, $unavailableErrors, true)) {
        die('skip unable to bind an AF_UNIX datagram socket');
    }
    die('unexpected AF_UNIX datagram bind failure: ' . socket_strerror($error));
}
socket_close($socket);
unlink($socketPath);
