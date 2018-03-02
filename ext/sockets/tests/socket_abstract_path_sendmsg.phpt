--TEST--
Support for paths in the abstract namespace (bind, sendmsg, recvmsg)
--SKIPIF--
<?php
if (!extension_loaded('sockets'))
	die('skip sockets extension not available.');

if (PHP_OS != 'Linux') {
	die('skip For Linux only');
}
?>
--FILE--
<?php
include __DIR__."/mcast_helpers.php.inc";

$path = "\x00/bar_foo";

echo "creating send socket\n";
$sends1 = socket_create(AF_UNIX, SOCK_DGRAM, 0) or die("err");
socket_set_nonblock($sends1) or die("Could not put in non-blocking mode");

echo "creating receive socket\n";
$s = socket_create(AF_UNIX, SOCK_DGRAM, 0) or die("err");
socket_bind($s, $path) or die("err");

$r = socket_sendmsg($sends1, [
	"name" => [ "path" => $path],
	"iov" => ["test ", "thing", "\n"],
], 0);
var_dump($r);
checktimeout($s, 500);

if (!socket_recv($s, $buf, 20, 0)) die("recv");
print_r($buf);
?>
--EXPECT--
creating send socket
creating receive socket
int(11)
test thing
