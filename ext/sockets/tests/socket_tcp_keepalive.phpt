--TEST--
Test SO_KEEPALIVE and TCP keepalive constants
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined('TCP_KEEPIDLE') && !defined('TCP_KEEPALIVE')) {
    die('skip TCP_KEEPIDLE/TCP_KEEPALIVE not available');
}
if (!defined('TCP_KEEPINTVL')) {
    die('skip TCP_KEEPINTVL not available');
}
if (!defined('TCP_KEEPCNT')) {
    die('skip TCP_KEEPCNT not available');
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if (!$socket) {
    die("socket failed");
}

socket_set_option($socket, SOL_SOCKET, SO_KEEPALIVE, 1);
$keepalive = socket_get_option($socket, SOL_SOCKET, SO_KEEPALIVE);
echo "SO_KEEPALIVE: " . ($keepalive ? "enabled" : "disabled") . "\n";

if (defined('TCP_KEEPIDLE')) {
    socket_set_option($socket, SOL_TCP, TCP_KEEPIDLE, 60);
    $idle = socket_get_option($socket, SOL_TCP, TCP_KEEPIDLE);
    echo "TCP_KEEPIDLE: $idle\n";
} else {
    socket_set_option($socket, SOL_TCP, TCP_KEEPALIVE, 60);
    $idle = socket_get_option($socket, SOL_TCP, TCP_KEEPALIVE);
    echo "TCP_KEEPIDLE: $idle\n";
}

socket_set_option($socket, SOL_TCP, TCP_KEEPINTVL, 10);
$intvl = socket_get_option($socket, SOL_TCP, TCP_KEEPINTVL);
echo "TCP_KEEPINTVL: $intvl\n";

socket_set_option($socket, SOL_TCP, TCP_KEEPCNT, 5);
$cnt = socket_get_option($socket, SOL_TCP, TCP_KEEPCNT);
echo "TCP_KEEPCNT: $cnt\n";

socket_close($socket);
?>
--EXPECT--
SO_KEEPALIVE: enabled
TCP_KEEPIDLE: 60
TCP_KEEPINTVL: 10
TCP_KEEPCNT: 5
