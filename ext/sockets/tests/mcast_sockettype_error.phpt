--TEST--
Multicast attempt on unsupported socket type
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not for Windows!');
}
?>
--FILE--
<?php
$sock_path = sprintf("/tmp/%s.sock", uniqid());

if (file_exists($sock_path))
	die('Temporary socket already exists.');
$sock = socket_create(AF_UNIX, SOCK_DGRAM, 0);
socket_bind($sock, $sock_path);

try {
    socket_set_option($sock, IPPROTO_IP, MCAST_JOIN_GROUP, array(
        "group"	=> '127.0.0.1',
        "interface" => "lo",
    ));
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IP address used in the context of an unexpected type of socket
