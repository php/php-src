--TEST--
socket_accept() failure
--FILE--
<?php

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
var_dump(socket_accept($socket));

?>
--EXPECTF--
Warning: socket_accept(): unable to accept incoming connection [%d]: %s in %s on line %d
bool(false)
