--TEST--
socket_getsockname from AF_PACKET socket
--EXTENSIONS--
sockets
posix
--SKIPIF--
<?php

if (!defined("AF_PACKET")) {
    die('SKIP AF_PACKET not supported on this platform.');
}
if (!function_exists("posix_getuid") || posix_getuid() != 0) {
    die('SKIP AF_PACKET requires root permissions.');
}
?>
--FILE--
<?php
    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_IP);
    $s_bind  = socket_bind($s_c, 'lo');
    var_dump($s_bind);

    // sock_getsockname in this context gets the interface rather than the address.
    $s_conn  = socket_getsockname($s_c, $istr, $iindex);
    var_dump($s_conn);
    var_dump($istr);
    var_dump($iindex);

    socket_getpeername($s_c, $istr2, $iindex2);
    socket_close($s_c);
?>
--EXPECTF--
bool(true)
bool(true)
string(2) "lo"
int(%i)

Warning: socket_getpeername(): unable to retrieve peer name [95]: %sot supported in %s on line %d
