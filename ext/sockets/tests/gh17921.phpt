--TEST--
GH-16267 - overflow on socket_strerror argument
--EXTENSIONS--
sockets
--FILE--
<?php
$s_c_l = socket_create_listen(0);
var_dump(socket_read($s_c_l, PHP_INT_MAX));
var_dump(socket_read($s_c_l, PHP_INT_MIN));
$a = "";
var_dump(socket_recv($s_c_l, $a, PHP_INT_MAX, 0));
var_dump(socket_recv($s_c_l, $a, PHP_INT_MIN, 0));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
