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
fclose($fd);

?>
===DONE===
--EXPECT--
===DONE===
