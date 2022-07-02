--TEST--
Winsock export/import socket, basic test
--EXTENSIONS--
sockets
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip.. Windows only test');
}

?>
--FILE--
<?php
    $address = 'localhost';
    $port = 10000;

    if (($sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
        fprintf(STDERR, "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n");
    }

    if (socket_bind($sock, $address, $port) === false) {
        fprintf(STDERR, "socket_bind() failed: reason: " . socket_strerror(socket_last_error($sock)) . "\n");
    }

    if (socket_listen($sock, 5) === false) {
        fprintf(STDERR, "socket_listen() failed: reason: " . socket_strerror(socket_last_error($sock)) . "\n");
    }

    /* Duplicate socket in the same process. */
    $pid = getmypid();
    $info = socket_wsaprotocol_info_export($sock, $pid);
    $sock2 = socket_wsaprotocol_info_import($info);
    var_dump(socket_wsaprotocol_info_release($info));

    var_dump($sock, $sock2);

    /* Close duplicated socket, the original is still valid. */
    socket_close($sock2);
    var_dump($sock, $sock2);

    /* Using invalid PID. */
    $info = socket_wsaprotocol_info_export($sock, 123412341);

    socket_close($sock);

    /* Importing with invalid identifier. */
    $sock2 = socket_wsaprotocol_info_import("garbage");
?>
--EXPECTF--
bool(true)
object(Socket)#%d (0) {
}
object(Socket)#%d (0) {
}
object(Socket)#%d (0) {
}
object(Socket)#%d (0) {
}

Warning: socket_wsaprotocol_info_export(): Unable to export WSA protocol info [0x00002726]: %s in %s on line %d

Warning: socket_wsaprotocol_info_import(): Unable to open file mapping [0x00000002] in %s on line %d
