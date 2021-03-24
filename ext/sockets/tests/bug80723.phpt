--TEST--
Bug #80723: Different sockets compare as equal (regression in 8.0)
--FILE--
<?php
$socket_1 = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$socket_2 = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
var_dump($socket_1 == $socket_1);
var_dump($socket_2 == $socket_2);
var_dump($socket_1 == $socket_2);

$vector = array(1 => $socket_1, 2 => $socket_2);
var_dump(array_search($socket_1, $vector));
var_dump(array_search($socket_2, $vector));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
int(1)
int(2)
