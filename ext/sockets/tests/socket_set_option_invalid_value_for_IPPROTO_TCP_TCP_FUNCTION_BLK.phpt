--TEST--
socket_set_option($socket, IPPROTO_TCP, TCP_FUNCTION_BLK, INVALID_TYPE_FOR_OPTION)
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined('IPPROTO_TCP')) {
    die('skip IPPROTO_TCP not available.');
}
if (!defined('TCP_FUNCTION_BLK')) {
    die('skip TCP_FUNCTION_BLK not available.');
}

?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
    die('Unable to create AF_INET socket [socket]');
}

try {
    $ret = socket_set_option($socket, IPPROTO_TCP, TCP_FUNCTION_BLK, new stdClass());
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $ret = socket_set_option($socket, IPPROTO_TCP, TCP_FUNCTION_BLK, "string\0with\0null\0bytes");
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

socket_close($socket);
?>
--EXPECT--
TypeError: socket_set_option(): Argument #4 ($value) must be of type string when argument #3 ($option) is TCP_FUNCTION_BLK, stdClass given
ValueError: socket_set_option(): Argument #4 ($value) must not contain null bytes when argument #3 ($option) is TCP_FUNCTION_BLK
