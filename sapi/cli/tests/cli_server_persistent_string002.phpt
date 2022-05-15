--TEST--
Close request before server sends a response
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

/* This is needed to check that the Server didn't die */
echo PHP_EOL;
var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS));

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
string(11) "Hello world"
===DONE===
