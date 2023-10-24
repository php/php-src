--TEST--
Bug #63000: Multicast on OSX
--EXTENSIONS--
sockets
--SKIPIF--
<?php

if (str_contains(PHP_OS, 'FreeBSD')) {
    die('skip option not supported on FreeBSD');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($socket, '0.0.0.0', 31057);

$so = socket_set_option($socket, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group" => '224.0.0.251',
    "interface" => 0,
));
var_dump($so);
?>
--EXPECT--
bool(true)
