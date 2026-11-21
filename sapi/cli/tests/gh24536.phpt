--TEST--
GH-24536 (Built-in server crashes on a huge chunk size or Content-Length)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start();

$host = PHP_CLI_SERVER_HOSTNAME;

$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: {$host}\r\nTransfer-Encoding: chunked\r\n\r\nFFFFFFFFFFFFFFFF\r\nAAAAAAAA");
stream_socket_shutdown($fp, STREAM_SHUT_WR);
var_dump(stream_get_contents($fp));
fclose($fp);

$fp = php_cli_server_connect();
fwrite($fp, "POST / HTTP/1.1\r\nHost: {$host}\r\nContent-Length: " . PHP_INT_MAX . "\r\n\r\nAAAAAAAA");
stream_socket_shutdown($fp, STREAM_SHUT_WR);
var_dump(stream_get_contents($fp));
fclose($fp);

$fp = php_cli_server_connect();
fwrite($fp, "GET / HTTP/1.1\r\nHost: {$host}\r\nConnection: close\r\n\r\n");
fpassthru($fp);
fclose($fp);
?>
--EXPECTF--
string(0) ""
string(0) ""
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

Hello world
