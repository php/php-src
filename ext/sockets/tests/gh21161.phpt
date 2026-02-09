--TEST--
GH-21161 (IPV6_PKTINFO socket option crash with null addr array entry)
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
?>
--FILE--
<?php
$sock = socket_create(AF_INET6, SOCK_DGRAM, 0);
socket_set_option($sock, IPPROTO_IPV6, IPV6_PKTINFO, ['addr' => null, 'ifindex' => 0]);
?>
--EXPECTF--
Warning: socket_set_option(): Host lookup failed [%i]: %s on line %d

Warning: socket_set_option(): error converting user data (path: in6_pktinfo > addr): could not resolve address '' to get an AF_INET6 address in %s on line %d
