--TEST--
ext/sockets - socket_read- test with incorrect parameter
--CREDITS--
Florian Anderiasch
fa@php.net
--EXTENSIONS--
sockets
--FILE--
<?php
    $s_c_l = socket_create_listen(0);
    $s_c = socket_read($s_c_l, 25);
    socket_close($s_c_l);
?>
--EXPECTF--
Warning: socket_read(): unable to read from socket [%i]: %a in %s on line %d
