--TEST--
socket_set_option($socket, SOL_SOCKET, SO_BINDTODEVICE, INVALID_TYPE_FOR_OPTION)
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined('SOL_SOCKET')) {
    die('skip SOL_SOCKET not available.');
}
if (!defined('SO_BINDTODEVICE')) {
    die('skip SO_BINDTODEVICE not available.');
}

?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
    die('Unable to create AF_INET socket [socket]');
}

try {
    $ret = socket_set_option($socket, SOL_SOCKET, SO_BINDTODEVICE, new stdClass());
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

socket_close($socket);
?>
--EXPECT--
TypeError: socket_set_option(): Argument #4 ($value) must be of type string when argument #3 ($option) is SO_BINDTODEVICE, stdClass given
