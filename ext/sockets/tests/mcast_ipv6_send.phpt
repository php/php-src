--TEST--
Multicast support: IPv6 send options
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('skip sockets extension not available.');
}
if (!defined('IPPROTO_IPV6')) {
	die('skip IPv6 not available.');
}
$level = IPPROTO_IPV6;
$s = socket_create(AF_INET6, SOCK_DGRAM, SOL_UDP) or die("skip Can not create socket");
if (socket_set_option($s, $level, IPV6_MULTICAST_IF, 1) === false) {
	die("skip interface 1 either doesn't exist or has no ipv6 address");
}
--FILE--
<?php
$domain = AF_INET6;
$level = IPPROTO_IPV6;
$s = socket_create($domain, SOCK_DGRAM, SOL_UDP) or die("err");

echo "Setting IPV6_MULTICAST_TTL\n";
$r = socket_set_option($s, $level, IPV6_MULTICAST_HOPS, 9);
var_dump($r);
$r = socket_get_option($s, $level, IPV6_MULTICAST_HOPS);
var_dump($r);
echo "\n";

echo "Setting IPV6_MULTICAST_LOOP\n";
$r = socket_set_option($s, $level, IPV6_MULTICAST_LOOP, 0);
var_dump($r);
$r = socket_get_option($s, $level, IPV6_MULTICAST_LOOP);
var_dump($r);
$r = socket_set_option($s, $level, IPV6_MULTICAST_LOOP, 1);
var_dump($r);
$r = socket_get_option($s, $level, IPV6_MULTICAST_LOOP);
var_dump($r);
echo "\n";

echo "Setting IPV6_MULTICAST_IF\n";
echo "interface 0:\n";
$r = socket_set_option($s, $level, IPV6_MULTICAST_IF, 0);
var_dump($r);
$r = socket_get_option($s, $level, IPV6_MULTICAST_IF);
var_dump($r);
echo "interface 1:\n";
$r = socket_set_option($s, $level, IPV6_MULTICAST_IF, 1);
var_dump($r);
$r = socket_get_option($s, $level, IPV6_MULTICAST_IF);
var_dump($r);
echo "\n";

--EXPECT--
Setting IPV6_MULTICAST_TTL
bool(true)
int(9)

Setting IPV6_MULTICAST_LOOP
bool(true)
int(0)
bool(true)
int(1)

Setting IPV6_MULTICAST_IF
interface 0:
bool(true)
int(0)
interface 1:
bool(true)
int(1)
