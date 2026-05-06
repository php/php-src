--TEST--
AF_PACKET socket_sendto() and socket_recvfrom() error cases
--EXTENSIONS--
sockets
posix
--SKIPIF--
<?php
if (!defined("AF_PACKET")) {
    die('SKIP AF_PACKET not supported on this platform.');
}
if (!defined("ETH_P_ALL")) {
    die('SKIP ETH_P_ALL not available on this platform.');
}
if (!function_exists("posix_getuid") || posix_getuid() != 0) {
    die('SKIP AF_PACKET requires root permissions.');
}
?>
--FILE--
<?php

echo "--- sendto without port (ifindex) ---\n";
$s = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s, 'lo');

try {
    socket_sendto($s, str_repeat("\x00", 60), 60, 0, "lo");
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}
socket_close($s);

echo "--- sendto with invalid interface name ---\n";
$s = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s, 'lo');

$ret = @socket_sendto($s, str_repeat("\x00", 60), 60, 0, "lo", 999999);
var_dump($ret === false);
socket_close($s);

echo "--- recvfrom on non-blocking socket with no data ---\n";
$s = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
socket_bind($s, 'lo');
socket_set_nonblock($s);

$ret = @socket_recvfrom($s, $buf, 65536, 0, $addr);
var_dump($ret === false);
socket_close($s);

?>
--EXPECT--
--- sendto without port (ifindex) ---
socket_sendto(): Argument #6 ($port) cannot be null when the socket type is AF_PACKET
--- sendto with invalid interface name ---
bool(true)
--- recvfrom on non-blocking socket with no data ---
bool(true)
