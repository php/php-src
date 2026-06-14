--TEST--
socket_create with IPPROTO_ICMP
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (!defined("IPPROTO_ICMP")) die("skip IPPROTO_ICMP not available");
// IPPROTO_ICMP* functions with raw sockets, thus requiring administrative role.
if (PHP_OS_FAMILY !== "Windows" && (!function_exists("posix_getuid") || posix_getuid() != 0)) die('skip IPPROTO_ICMP requires root permissions.');
?>
--FILE--
<?php

$host = '127.0.0.1';

$type = 8;
$code = 0;
$identifier = 16;
$sequence = 1;
$data = "ECHO";

$socket = socket_create(AF_INET, SOCK_RAW, IPPROTO_ICMP);
if (!$socket) {
    die("Unable to create socket: " . socket_strerror(socket_last_error()) . "\n");
}

$header = pack('C2n3a*', $type, $code, 0, $identifier, $sequence, $data);
$checksum = function($header): int {
    $bit = unpack('n*', $header);
    $sum = array_sum($bit);

    while ($sum >> 16) {
        $sum = ($sum & 0xFFFF) + ($sum >> 16);
    }
    return ~ $sum & 0xFFFF;
};
$header = pack('C2n3a*', $type, $code, $checksum($header), $identifier, $sequence, $data);

if (!socket_sendto($socket, $header, strlen($header), 0, $host, 0)) {
    die("Unable to send packet: " . socket_strerror(socket_last_error($socket)) . "\n");
}

$read = [$socket];
$write = $except = [];
$timeout = ['sec' => 4, 'usec' => 0];
if (socket_select($read, $write, $except, $timeout['sec'], $timeout['usec']) > 0) {
    $response = '';
    socket_recv($socket, $response, 65535, 0);
    var_dump(bin2hex($response));
} else {
    die("Unable to read the response\n");
}

socket_close($socket);
?>
--EXPECTF--
string(64) "%s"
