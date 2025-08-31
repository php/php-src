--TEST--
Test TCP_CONGESTION constant with allowed algos for unprivileged accounts.
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('TCP_CONGESTION')) {
    die('skip TCP_CONGESTION test');
}
?>
--FILE--
<?php

if (str_contains(PHP_OS, 'Linux')) {
	$algo = 'cubic';
} else {
	$algo = 'newreno';
}
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) die ("socket failed");
$r = socket_get_option($socket, SOL_TCP, TCP_CONGESTION);
echo "current tcp congestion algo " . $r['name'] . "\n";
var_dump(socket_set_option($socket, SOL_TCP, TCP_CONGESTION, $algo));
$r = socket_get_option($socket, SOL_TCP, TCP_CONGESTION);
echo "new tcp congestion algo " . $r['name'];
?>
--EXPECTF--
current tcp congestion algo %s
bool(true)
new tcp congestion algo %s
