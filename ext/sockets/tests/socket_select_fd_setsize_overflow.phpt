--TEST--
socket_select() reports an overflowing set instead of silently truncating it
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') {
    die('skip Windows only, elsewhere the overflow is caught through max_fd');
}
?>
--FILE--
<?php
$sockets = [];
for ($i = 0; $i < 300; $i++) {
    $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    if ($socket === false) {
        break;
    }
    $sockets[] = $socket;
}

var_dump(count($sockets) > 256);

$read = $sockets;
$write = $except = null;
var_dump(socket_select($read, $write, $except, 0));

$write = $sockets;
$read = $except = null;
var_dump(socket_select($read, $write, $except, 0));

$except = $sockets;
$read = $write = null;
var_dump(socket_select($read, $write, $except, 0));

foreach ($sockets as $socket) {
    socket_close($socket);
}
?>
--EXPECTF--
bool(true)

Warning: socket_select(): PHP needs to be recompiled with a larger value of FD_SETSIZE.%A
bool(false)

Warning: socket_select(): PHP needs to be recompiled with a larger value of FD_SETSIZE.%A
bool(false)

Warning: socket_select(): PHP needs to be recompiled with a larger value of FD_SETSIZE.%A
bool(false)
