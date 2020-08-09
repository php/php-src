--TEST--
recvmsg(): basic test
--SKIPIF--
<?php
if (!extension_loaded('sockets')) {
    die('skip sockets extension not available.');
}

require 'ipv6_skipif.inc';

if (!defined('IPPROTO_IPV6')) {
    die('skip IPv6 not available.');
}
if (!defined('IPV6_RECVPKTINFO')) {
    die('skip IPV6_RECVPKTINFO not available.');
}
?>
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";
$addr = '::1';

echo "creating send socket\n";
$sends1 = socket_create(AF_INET6, SOCK_DGRAM, SOL_UDP) or die("err");
var_dump($sends1);
$br = socket_bind($sends1, '::', 7001) or die("err");
var_dump($br);
socket_set_nonblock($sends1) or die("Could not put in non-blocking mode");

echo "creating receive socket\n";
$s = socket_create(AF_INET6, SOCK_DGRAM, SOL_UDP) or die("err");
var_dump($s);
$br = socket_bind($s, '::0', 3001) or die("err");
var_dump($br);

socket_set_option($s, IPPROTO_IPV6, IPV6_RECVPKTINFO, 1) or die("err");

$r = socket_sendto($sends1, $m = "testing packet", strlen($m), 0, $addr, 3001);
var_dump($r);
if ($r < 12) die;
checktimeout($s, 500);

$data = [
    "name" => ["family" => AF_INET6, "addr" => "::1"],
    "buffer_size" => 2000,
    "controllen" => socket_cmsg_space(IPPROTO_IPV6, IPV6_PKTINFO),
];
if (!socket_recvmsg($s, $data, 0)) die("recvmsg");
print_r($data);
?>
--EXPECTF--
creating send socket
object(Socket)#%d (0) {
}
bool(true)
creating receive socket
object(Socket)#%d (0) {
}
bool(true)
int(14)
Array
(
    [name] => Array
        (
            [family] => %d
            [addr] => ::1
            [port] => 7001
            [flowinfo] => 0
            [scope_id] => 0
        )

    [control] => Array
        (
            [0] => Array
                (
                    [level] => %d
                    [type] => %d
                    [data] => Array
                        (
                            [addr] => ::1
                            [ifindex] => %d
                        )

                )

        )

    [iov] => Array
        (
            [0] => testing packet
        )

    [flags] => 0
)
