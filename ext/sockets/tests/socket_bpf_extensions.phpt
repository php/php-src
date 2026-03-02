--TEST--
SO_BPF_EXTENSIONS socket_getopt test
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined("SO_BPF_EXTENSIONS")) {
    die("skip SO_BOF_EXTENSIONS");
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
    die('Unable to create AF_INET socket [socket]');
}
$flags = socket_getopt( $socket, SOL_SOCKET, SO_BPF_EXTENSIONS);
if ($flags == false) die("socket_getopt");
echo "extensions supported flag $flags";
socket_close($socket);
?>
--EXPECTF--
extensions supported flag %d
