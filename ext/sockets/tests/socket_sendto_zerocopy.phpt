--TEST--
Test socket_sendto with MSG_ZEROCOPY
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined("SO_ZEROCOPY")) {
    die('skip SO_ZEROCOPY');
}
--FILE--
<?php
    $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    if (!$socket) {
        die('Unable to create AF_UNIX socket');
    }
    if (!socket_set_option($socket, SOL_SOCKET, SO_ZEROCOPY, 1)) {
        die("Unable to set the socket option to SO_ZEROCOPY");
    }
    if (!socket_set_nonblock($socket)) {
        die('Unable to set nonblocking mode for socket');
    }
    $address = '127.0.0.1';
    if (!socket_bind($socket, $address, 0)) {
        die("Unable to bind to $address");
    }

    $msg = str_repeat("0123456789abcdef", 1024);
    $len = strlen($msg);
    $bytes_sent = socket_sendto($socket, $msg, $len, MSG_ZEROCOPY, $address, 3001);
    echo "$bytes_sent sent!";
    socket_close($socket);
?>
--EXPECTF--
16384 sent!
