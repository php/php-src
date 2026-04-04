--TEST--
AF_PACKET socket_recvfrom() without optional port argument
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
$s_send = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
$s_recv = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);

socket_bind($s_send, 'lo');
socket_bind($s_recv, 'lo');

$dst_mac = "\xff\xff\xff\xff\xff\xff";
$src_mac = "\x00\x00\x00\x00\x00\x00";
$ethertype = pack("n", 0x9000);
$payload = "no port test";
$frame = str_pad($dst_mac . $src_mac . $ethertype . $payload, 60, "\x00");

socket_sendto($s_send, $frame, strlen($frame), 0, "lo", 1);

// recvfrom without the optional 6th argument (port/ifindex).
$bytes = socket_recvfrom($s_recv, $buf, 65536, 0, $addr);
var_dump($bytes >= 60);
var_dump($addr === 'lo');

socket_close($s_send);
socket_close($s_recv);
?>
--EXPECT--
bool(true)
bool(true)
