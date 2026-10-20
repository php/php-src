--TEST--
Test socket_set_nonblock return values
--EXTENSIONS--
sockets
--FILE--
<?php

$socket = socket_create_listen(0);
var_dump(socket_set_nonblock($socket));
socket_close($socket);

$socket2 = socket_create_listen(0);
socket_close($socket2);
try {
    var_dump(socket_set_nonblock($socket2));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(true)
Error: socket_set_nonblock(): Argument #1 ($socket) has already been closed
