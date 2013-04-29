--TEST--
Bug #63000: Multicast on OSX
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('skip sockets extension not available.');
}
if (PHP_OS !== 'Darwin') {
    die('is not OSX.');
}
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($socket, '0.0.0.0', 31057);

$so = socket_set_option($socket, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group" => '224.0.0.251',
    "interface" => 0,
));
var_dump($so);
--EXPECTF--
bool(true)
