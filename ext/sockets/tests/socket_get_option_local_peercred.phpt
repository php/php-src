--TEST--
socket_get_option() LOCAL_PEERCRED
--EXTENSIONS--
sockets
posix
--SKIPIF--
<?php
if (!defined('LOCAL_PEERCRED')) die('skip LOCAL_PEERCRED not available');
?>
--FILE--
<?php
if (!socket_create_pair(AF_UNIX, SOCK_STREAM, 0, $pair)) {
    die('Unable to create AF_UNIX socket pair');
}

$cred = socket_get_option($pair[0], SOL_LOCAL, LOCAL_PEERCRED);
var_dump($cred['uid'] === posix_geteuid());
var_dump($cred['gid'] === posix_getegid());

socket_close($pair[0]);
socket_close($pair[1]);

$inet = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
try {
    socket_get_option($inet, SOL_LOCAL, LOCAL_PEERCRED);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
socket_close($inet);
?>
--EXPECT--
bool(true)
bool(true)
ValueError: socket_get_option(): Argument #1 ($socket) must be used with an AF_UNIX socket
