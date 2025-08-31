--TEST--
Server processing multiple request at the same time
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start();

$fd = stream_socket_client("tcp://" . PHP_CLI_SERVER_ADDRESS);
fwrite($fd, "GET /index.php HTTP/1.0\r\nHost: hello");

$fd2 = stream_socket_client("tcp://" . PHP_CLI_SERVER_ADDRESS);
fwrite($fd2, "GET /index.php HTTP/1.0\r\nConnection: close\r\n\r\n");

stream_copy_to_stream($fd2, STDOUT);

fwrite($fd, "\r\n\r\n");
stream_copy_to_stream($fd, STDOUT);

echo PHP_EOL;

?>
===DONE===
--EXPECTF--
HTTP/1.0 200 OK
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

Hello worldHTTP/1.0 200 OK
Host: hello
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

Hello world
===DONE===
