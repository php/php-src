--TEST--
socket_set_option($socket, SOL_SOCKET, SO_ACCEPTFILTER, INVALID_TYPE_FOR_OPTION)
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined('SOL_SOCKET')) {
    die('skip SOL_SOCKET not available.');
}
if (!defined('SO_ACCEPTFILTER')) {
    die('skip SO_ACCEPTFILTER not available.');
}

?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
    die('Unable to create AF_INET socket [socket]');
}

try {
    $ret = socket_set_option($socket, SOL_SOCKET, SO_ACCEPTFILTER, new stdClass());
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $ret = socket_set_option($socket, SOL_SOCKET, SO_ACCEPTFILTER, "string\0with\0null\0bytes");
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $ret = socket_set_option($socket, SOL_SOCKET, SO_ACCEPTFILTER, str_repeat("a", 2048));
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

socket_close($socket);
?>
--EXPECTF--
TypeError: socket_set_option(): Argument #4 ($value) must be of type string when argument #3 ($option) is SO_ACCEPTFILTER, stdClass given
ValueError: socket_set_option(): Argument #4 ($value) must not contain null bytes when argument #3 ($option) is SO_ACCEPTFILTER
ValueError: socket_set_option(): Argument #4 ($value) must be less than %d bytes when argument #3 ($option) is SO_ACCEPTFILTER
