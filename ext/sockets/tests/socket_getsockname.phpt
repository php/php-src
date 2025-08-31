--TEST--
ext/sockets - socket_getsockname - basic test
--CREDITS--
Florian Anderiasch
fa@php.net
--EXTENSIONS--
sockets
--FILE--
<?php
    $s_c     = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    $s_bind  = socket_bind($s_c, '0.0.0.0');
    var_dump($s_bind);

    // Connect to destination address
    $s_conn  = socket_getsockname($s_c, $ip, $port);
    var_dump($s_conn);
    var_dump($ip);
    var_dump($port);
    socket_close($s_c);
?>
--EXPECTF--
bool(true)
bool(true)
string(7) "0.0.0.0"
int(%i)
