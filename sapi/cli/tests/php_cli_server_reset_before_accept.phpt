--TEST--
Connection reset before it is accepted does not take the server down
--EXTENSIONS--
sockets
--SKIPIF--
<?php
include "skipif.inc";
if (PHP_OS_FAMILY === "Windows") die("skip SO_LINGER reset behaviour differs on Windows");
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start();

$fp = php_cli_server_connect();
socket_set_option(socket_import_stream($fp), SOL_SOCKET, SO_LINGER, ['l_onoff' => 1, 'l_linger' => 0]);
stream_socket_shutdown($fp, STREAM_SHUT_RD);
fclose($fp);

$fp = php_cli_server_connect();
fwrite($fp, "GET / HTTP/1.1\r\nConnection: close\r\n\r\n");
echo fgets($fp);
fclose($fp);
?>
--EXPECT--
HTTP/1.1 200 OK
