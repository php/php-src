--TEST--
Test if socket_set_option() works, option:TCP_DEFER_ACCEPT
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (strpos(PHP_OS, 'Linux') === false) {
	die('SKIP on non Linux');
}
?>
--FILE--
<?php
echo "*** Test with TCP_DEFER_ACCEPT with initial SYN/ACK 'timeout' value to rounded up one ***\n";
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

if (!$socket) {
        die('Unable to create AF_INET socket [socket]');
}
$initial_val = 5;
var_dump(socket_set_option( $socket, SOL_TCP, TCP_DEFER_ACCEPT, $initial_val));
socket_listen($socket);
$rounded_up_val = socket_get_option( $socket, SOL_TCP, TCP_DEFER_ACCEPT);
socket_close($socket);
var_dump($rounded_up_val > $initial_val); // Value rounded up by the kernel, might differ from kernel version/setting
?>
--EXPECT--
*** Test with TCP_DEFER_ACCEPT with initial SYN/ACK 'timeout' value to rounded up one ***
bool(true)
bool(true)
