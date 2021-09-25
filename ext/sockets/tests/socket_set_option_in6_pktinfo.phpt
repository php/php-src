--TEST--
socket_set_option() with IPV6_PKTINFO
--EXTENSIONS--
sockets
--SKIPIF--
<?php


require 'ipv6_skipif.inc';

if (!defined('IPPROTO_IPV6')) {
    die('skip IPv6 not available.');
}
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not for Windows!');
}
if (!defined('IPV6_PKTINFO')) {
    die('skip IPV6_PKTINFO not available.');
}
--FILE--
<?php

$s = socket_create(AF_INET6, SOCK_DGRAM, SOL_UDP) or die("err");
var_dump(socket_set_option($s, IPPROTO_IPV6, IPV6_PKTINFO, []));
var_dump(socket_set_option($s, IPPROTO_IPV6, IPV6_PKTINFO, [
    "addr" => '::1',
    "ifindex" => 0
]));
//Oddly, Linux does not support IPV6_PKTINFO in sockgetopt().
//See do_ipv6_getsockopt() on the kernel sources
//A work-around with is sort-of possible (with IPV6_2292PKTOPTIONS),
//but not worth it
//var_dump(socket_get_option($s, IPPROTO_IPV6, IPV6_PKTINFO));
?>
--EXPECTF--
Warning: socket_set_option(): error converting user data (path: in6_pktinfo): The key 'addr' is required in %s on line %d
bool(false)
bool(true)
