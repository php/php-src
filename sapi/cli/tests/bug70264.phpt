--TEST--
Bug #70264 (CLI server directory traversal)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(null, null);
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/..\\CREDITS");
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/..%5CCREDITS");
?>
--EXPECTF--
Warning: file_get_contents(http://%s/..\CREDITS): Failed to open stream: HTTP request failed! HTTP/1.1 404 Not Found
 in %sbug70264.php on line %d

Warning: file_get_contents(http://%s/..%5CCREDITS): Failed to open stream: HTTP request failed! HTTP/1.1 404 Not Found
 in %sbug70264.php on line %d
