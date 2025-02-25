--TEST--
AF_PACKET sockets handling errors
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
if (PHP_INT_SIZE != 8) {
    die("skip: 64-bit only");
}
?>
--FILE--
<?php
    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $s_bind  = socket_bind($s_c, 'lo');

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $v_bind  = socket_bind($s_s, 'lo');

    $payload = random_bytes(1024);
    $payload .= str_repeat("A", 46);

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", 0x0806);
    $buf .= $payload;

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));

    try {
    	socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch (\ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    socket_close($s_s);
    socket_close($s_c);

    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $s_bind  = socket_bind($s_c, 'lo');

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $v_bind  = socket_bind($s_s, 'lo');

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_IP);
    $buf .= str_repeat("A", 46);

    var_dump(socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1));

    try {
    	socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch (\ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    socket_close($s_s);
    socket_close($s_c);

    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $s_bind  = socket_bind($s_c, 'lo');

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $v_bind  = socket_bind($s_s, 'lo');

    $payload = hex2bin(
	    "4500" .
	    "0028" .
	    "0000" .
	    "4000" .
	    "4006" .
	    "0000" .
	    "7f000001" .
	    "7f000001"
    ) . str_repeat("A", 20);

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_LOOP);
    $buf .= $payload;

    try {
        socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1);
    } catch(\ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }
    socket_close($s_s);
    socket_close($s_c);

    $s_c     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $s_bind  = socket_bind($s_c, 'lo');

    $s_s     = socket_create(AF_PACKET, SOCK_RAW, ETH_P_ALL);
    $v_bind  = socket_bind($s_s, 'lo');

    $payload = random_bytes(1024);
    $payload .= str_repeat("A", 46);

    $buf = pack("H12H12n", "ffffffffffff", "000000000000", ETH_P_LOOP);
    $buf .= $payload;

    socket_sendto($s_s, $buf, strlen($buf), 0, "lo", 1);

    try {
        socket_recvfrom($s_c, $rsp, strlen($buf), 0, $addr);
    } catch (\ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    }

    socket_close($s_s);
    socket_close($s_c);

?>
--EXPECT--
int(1084)
unsupported ethernet protocol
int(60)
invalid transport header length
socket_sendto(): Argument #3 ($length) must be at least 64 for AF_PACKET
invalid ethernet loop header
