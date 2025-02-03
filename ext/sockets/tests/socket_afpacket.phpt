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
    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $s_bind  = socket_bind($s_c, 'lo');
    var_dump($s_bind);

    // sock_getsockname in this context gets the interface rather than the address.
    $s_conn  = socket_getsockname($s_c, $istr, $iindex);
    var_dump($s_conn);
    var_dump($istr);
    var_dump($iindex);

    socket_getpeername($s_c, $istr2, $iindex2);

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $v_bind  = socket_bind($s_s, 'lo');

    $buf = str_repeat("0", ETH_FRAME_LEN) .
	   str_repeat("\xFF", 6) .
	   str_repeat("\x11", 6) .
	   "\x08\x00" .
	   str_pad(str_repeat("test", 512), 2048, "\x00");

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));

    socket_close($s_s);
    socket_close($s_c);
?>
--EXPECTF--
bool(true)
bool(true)
string(2) "lo"
int(%i)

Warning: socket_getpeername(): unable to retrieve peer name [95]: %sot supported in %s on line %d
int(3576)
