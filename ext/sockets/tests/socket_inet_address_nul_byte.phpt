--TEST--
socket_bind(), socket_connect() and socket_sendto() reject NUL bytes in AF_INET addresses
--EXTENSIONS--
sockets
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
$address = "127.0.0.1\0.example.com";

try {
    socket_bind($socket, $address);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    socket_connect($socket, $address, 9);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    socket_sendto($socket, "x", 1, 0, $address, 9);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: socket_bind(): Argument #2 ($address) must not contain any null bytes
ValueError: socket_connect(): Argument #2 ($address) must not contain any null bytes
ValueError: socket_sendto(): Argument #5 ($address) must not contain any null bytes
