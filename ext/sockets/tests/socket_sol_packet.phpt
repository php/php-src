--TEST--
socket_setopt() with SOL_PACKET
--EXTENSIONS--
sockets
posix
--SKIPIF--
<?php if (!defined("SOL_PACKET")) die('SKIP SOL_PACKET not supported on this platform.');
if (!function_exists("posix_getuid") || posix_getuid() != 0) die('SKIP AF_PACKET requires root permissions.');
?>
--FILE--
<?php
    $s = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    var_dump($s);
    var_dump(socket_setopt($s, SOL_PACKET, PACKET_FANOUT, PACKET_FANOUT_LB));
    socket_close($s);
?>
--EXPECTF--
object(Socket)#1 (0) {
}
bool(true)

