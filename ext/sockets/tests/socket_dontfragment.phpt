--TEST--
IP_MTU_DISCOVER test
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_OS != 'Linux') {
    die('skip For Linux only');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($socket, '127.0.0.1', 5557);
$ret = socket_set_option($socket, IPPROTO_IP, IP_MTU_DISCOVER, IP_PMTUDISC_DO);
var_dump($ret);
?>
--EXPECT--
bool(true)
