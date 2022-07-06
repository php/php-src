--TEST--
Test socket_set_block return values
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('SKIP The sockets extension is not loaded.');
}
?>
--FILE--
<?php

$socket = socket_create_listen(0);
var_dump(socket_set_block($socket));
socket_close($socket);

$socket2 = socket_create_listen(0);
socket_close($socket2);
try {
    var_dump(socket_set_block($socket2));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(true)
socket_set_block(): Argument #1 ($socket) has already been closed
