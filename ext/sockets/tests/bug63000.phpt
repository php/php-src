--TEST--
Bug #63000: Multicast on OSX
--EXTENSIONS--
sockets
--SKIPIF--
<?php
$socket = @socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
if ($socket === false) {
    die('skip unable to create IPv4 UDP socket');
}
$so = @socket_set_option($socket, IPPROTO_IP, MCAST_JOIN_GROUP, [
    'group' => '224.0.0.251',
    'interface' => 0,
]);
if ($so === false) {
    $errno = socket_last_error($socket);
    if (in_array($errno, [SOCKET_ENODEV, SOCKET_ENXIO, SOCKET_EADDRNOTAVAIL], true)) {
        die('skip no multicast-capable interface: ' . socket_strerror($errno));
    }
}
?>
--FILE--
<?php
$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
socket_bind($socket, '0.0.0.0', 31057);

$so = socket_set_option($socket, IPPROTO_IP, MCAST_JOIN_GROUP, array(
    "group" => '224.0.0.251',
    "interface" => 0,
));
var_dump($so);
?>
--EXPECT--
bool(true)
