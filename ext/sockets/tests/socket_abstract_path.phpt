--TEST--
Support for paths in the abstract namespace (bind, connect)
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

$path = "\x00/foo_bar";

echo "creating server socket\n";
$servers = socket_create(AF_UNIX, SOCK_STREAM, 0) or die("err");
socket_bind($servers, $path) or die("Could not bind");
socket_listen($servers) or die("Could not listen");
socket_set_nonblock($servers) or die("Could not put in non-blocking mode");

echo "creating client socket\n";
$clients = socket_create(AF_UNIX, SOCK_STREAM, 0) or die("err");
socket_connect($clients, $path) or die("Error connecting");

$conns = socket_accept($servers) or die("Could not accept connection");

$r = socket_sendmsg($clients, [
    //"name" => [ "addr" => $path, ],
    "iov" => ["test ", "thing", "\n"],
], 0);
var_dump($r);
checktimeout($conns, 500);

if (!socket_recv($conns, $buf, 20, 0)) die("recv");
print_r($buf);
?>
--EXPECT--
creating server socket
creating client socket
int(11)
test thing
