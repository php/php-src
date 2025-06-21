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
if (PHP_INT_SIZE != 4) die("skip this test is for 32-bit only");
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

    socket_getpeername($s_c, $istr, $iindex2);

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_LOOP);
    $v_bind  = socket_bind($s_s, 'lo');

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_LOOP);
    $buf .= str_repeat("A", 46);

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));

    try {
    	socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch(\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
    }

    socket_close($s_s);
    socket_close($s_c);
?>
--EXPECTF--
bool(true)
bool(true)
string(2) "lo"
int(%d)
invalid transport header length
