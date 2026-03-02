--TEST--
Test parameter handling in socket_select().
--DESCRIPTION--
usec > 999999
--EXTENSIONS--
sockets
--FILE--
<?php
$sockets = array();
if (strtolower(substr(PHP_OS, 0, 3)) == 'win') {
    $domain = AF_INET;
} else {
    $domain = AF_UNIX;
}
socket_create_pair($domain, SOCK_STREAM, 0, $sockets);

$write  = null;
$except = null;
$time   = 0;
$usec   = 2000000;
var_dump(socket_select($sockets, $write, $except, $time, $usec));
?>
--EXPECT--
int(0)
--CREDITS--
Till Klampaeckel, till@php.net
Berlin TestFest 2009
