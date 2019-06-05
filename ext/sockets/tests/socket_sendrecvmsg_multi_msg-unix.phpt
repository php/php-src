--TEST--
sendmsg()/recvmsg(): test ability to receive multiple messages
--SKIPIF--
<?php
if (!extension_loaded('sockets'))
	die('skip sockets extension not available.');

require 'ipv6_skipif.inc';

if (!defined('IPPROTO_IPV6'))
	die('skip IPv6 not available.');
if (substr(PHP_OS, 0, 3) == 'WIN')
	die('skip Not for the Windows!');
/* Windows supports IPV6_RECVTCLASS and is able to receive the tclass via
 * WSARecvMsg (though only the top 6 bits seem to reported), but WSASendMsg
 * does not accept IPV6_TCLASS messages */

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
$br = socket_bind($sends1, '::', 7002) or die("err");
var_dump($br);
socket_set_nonblock($sends1) or die("Could not put in non-blocking mode");

echo "creating receive socket\n";
$s = socket_create(AF_INET6, SOCK_DGRAM, SOL_UDP) or die("err");
var_dump($s);
$br = socket_bind($s, '::0', 3002) or die("err");
var_dump($br);

socket_set_option($s, IPPROTO_IPV6, IPV6_RECVPKTINFO, 1) or die("err");
socket_set_option($s, IPPROTO_IPV6, IPV6_RECVTCLASS, 1) or die("err");

$r = socket_sendmsg($sends1, [
	"name" => [ "addr" => "::1", "port" => 3002],
	"iov" => ["test ", "thing", "\n"],
	"control" => [[
		"level" => IPPROTO_IPV6,
		"type" => IPV6_TCLASS,
		"data" => 40,
	]]
], 0);
var_dump($r);
checktimeout($s, 500);

$data = [
    "name" => ["family" => AF_INET6, "addr" => "::1"],
    "buffer_size" => 2000,
    "controllen" => socket_cmsg_space(IPPROTO_IPV6, IPV6_PKTINFO) +
			socket_cmsg_space(IPPROTO_IPV6, IPV6_TCLASS),
];
if (!socket_recvmsg($s, $data, 0)) die("recvmsg");
print_r($data);
--EXPECTF--
creating send socket
resource(5) of type (Socket)
bool(true)
creating receive socket
resource(6) of type (Socket)
bool(true)
int(11)
Array
(
    [name] => Array
        (
            [family] => %d
            [addr] => ::1
            [port] => 7002
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

            [1] => Array
                (
                    [level] => %d
                    [type] => %d
                    [data] => 40
                )

        )

    [iov] => Array
        (
            [0] => test thing

        )

    [flags] => 0
)
