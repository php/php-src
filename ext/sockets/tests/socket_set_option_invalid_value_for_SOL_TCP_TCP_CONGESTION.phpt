--TEST--
socket_set_option($socket, SOL_TCP, TCP_CONGESTION, INVALID_TYPE_FOR_OPTION)
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined('TCP_CONGESTION')) {
    die('skip TCP_CONGESTION not available.');
}

?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
    die('Unable to create AF_INET socket [socket]');
}

try {
    $ret = socket_set_option($socket, SOL_TCP, TCP_CONGESTION, new stdClass());
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

socket_close($socket);
?>
--EXPECT--
TypeError: socket_set_option(): Argument #4 ($value) must be of type string when argument #3 ($option) is TCP_CONGESTION, stdClass given
