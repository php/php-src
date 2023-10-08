--TEST--
Test socket_sendto with MSG_ZEROCOPY
--EXTENSIONS--
sockets
--SKIPIF--
<?php
$arch = php_uname('m');
if (!defined("SO_ZEROCOPY")) {
    die('skip SO_ZEROCOPY');
}
if (strpos($arch, 'ppc') !== false || strpos($arch, 'powerpc') !== false) {
    die('skip not for powerpc arch');
}
if (getenv('CIRRUS_CI') && strpos($arch, 'aarch64') !== false) {
    die('xfail Broken on Cirrus + arm');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
if (!$socket) {
    die('Unable to create AF_UNIX socket');
}
$s = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
if (!$s) {
    die('Unable to create AF_UNIX socket');
}
if (!socket_set_option($socket, SOL_SOCKET, SO_ZEROCOPY, 1)) {
    die("Unable to set the socket option to SO_ZEROCOPY");
}
if (!socket_set_nonblock($s)) {
    die('Unable to set nonblocking mode for socket');
}
$address = '127.0.0.1';
$port = 3001;
if (!socket_bind($s, $address, $port)) {
    die("Unable to bind to $address");
}

$msg = str_repeat("0123456789abcdef", 1024);
$len = strlen($msg);
$bytes_recv = 0;
$bytes_sent = socket_sendto($socket, $msg, $len, MSG_ZEROCOPY, $address, $port);
if (socket_recvfrom($s, $resp, 0, MSG_ERRQUEUE, $address, $port) == -1) die ("recvfrom MSG_ERRQUEUE");
$bytes_recv = socket_recvfrom($s, $resp, 16, 0, $address, $port);
echo "$bytes_sent sent!\n";
echo "$bytes_recv received!\n";
echo "Received $resp!";
socket_close($s);
socket_close($socket);
?>
--EXPECTF--
16384 sent!
16 received!
Received 0123456789abcdef!
