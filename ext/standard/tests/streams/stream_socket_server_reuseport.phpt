--TEST--
Testing stream_socket_server() SO_REUSEPORT context option
--SKIPIF--
<?php
if (!defined("STREAM_SOCK_REUSEPORT")) die("skip");
?>
--FILE--
<?php

$address = '127.0.0.1:15678';
$flags = STREAM_SERVER_BIND | STREAM_SERVER_LISTEN;
$options = [
    'socket' => [
        'reuseport' => true,
    ],
];
$context = stream_context_create($options);

$a = stream_socket_server($address, $errno, $errstr, $flags, $context);
$b = stream_socket_server($address, $errno, $errstr, $flags, $context);
echo stream_socket_get_name($a, false), "\n", stream_socket_get_name($b, false);

?>
--EXPECT--
127.0.0.1:15678
127.0.0.1:15678
