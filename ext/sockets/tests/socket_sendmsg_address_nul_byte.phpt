--TEST--
socket_sendmsg() rejects NUL bytes in addresses
--EXTENSIONS--
sockets
--SKIPIF--
<?php
require "ipv6_skipif.inc";
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
var_dump(socket_sendmsg($socket, [
    "name" => ["addr" => "127.0.0.1\0.example.com", "port" => 9],
    "iov" => ["x"],
], 0));

$socket = socket_create(AF_INET6, SOCK_DGRAM, SOL_UDP);
var_dump(socket_sendmsg($socket, [
    "name" => ["addr" => "::1\0.example.com", "port" => 9],
    "iov" => ["x"],
], 0));
?>
--EXPECTF--
Warning: socket_sendmsg(): error converting user data (path: msghdr > name > addr): must not contain any null bytes in %s on line %d
bool(false)

Warning: socket_sendmsg(): error converting user data (path: msghdr > name > addr): must not contain any null bytes in %s on line %d
bool(false)
