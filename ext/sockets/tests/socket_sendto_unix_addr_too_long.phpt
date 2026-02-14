--TEST--
socket_sendto() with AF_UNIX rejects address exceeding sun_path limit
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip not valid for Windows');
}
?>
--FILE--
<?php
$socket = socket_create(AF_UNIX, SOCK_DGRAM, 0);
if (!$socket) {
    die('Unable to create AF_UNIX socket');
}

$long_addr = str_repeat('a', 512);

try {
    socket_sendto($socket, "data", 4, 0, $long_addr);
} catch (\ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

socket_close($socket);
?>
--EXPECTF--
socket_sendto(): Argument #5 ($address) must be less than %d
