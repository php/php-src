--TEST--
GH-13603 -  socket_getsockname - invalid characters
--EXTENSIONS--
sockets
--FILE--
<?php
        $socket = socket_create(AF_UNIX, SOCK_STREAM, 0);
        socket_bind($socket, 'sn.socp');
        socket_listen($socket);
        socket_getsockname($socket, $address);
        var_dump($address);
        socket_close($socket);
        unlink($address);
--EXPECT--
string(7) "sn.socp"
