--TEST--
socket_set_option($socket, SOL_SOCKET, SO_BINDTODEVICE, INVALID_TYPE_FOR_OPTION)
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (!defined('SOL_FILTER')) {
    die('skip SOL_FILTER not available.');
}
if (!defined('FIL_ATTACH')) {
    die('skip FIL_ATTACH not available.');
}
if (!defined('FIL_DETACH')) {
    die('skip FIL_DETACH not available.');
}

?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
    die('Unable to create AF_INET socket [socket]');
}

// TODO Warning when using FIL_ATTACH/FIL_DETACH option when level is not SOL_FILTER
try {
    $ret = socket_set_option($socket, SOL_FILTER, FIL_ATTACH, new stdClass());
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    $ret = socket_set_option($socket, SOL_FILTER, FIL_DETACH, new stdClass());
    var_dump($ret);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

socket_close($socket);
?>
--EXPECT--
TypeError: socket_set_option(): Argument #4 ($value) must be of type string when argument #3 ($option) is FIL_ATTACH or FIL_DETACH, stdClass given
TypeError: socket_set_option(): Argument #4 ($value) must be of type string when argument #3 ($option) is FIL_ATTACH or FIL_DETACH, stdClass given
