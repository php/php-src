--TEST--
Bug #66606 (Sets HTTP_CONTENT_TYPE but not CONTENT_TYPE) - GET request
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["CONTENT_TYPE"], $_SERVER["CONTENT_LENGTH"])');
echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS);
?>
--EXPECT--
NULL
NULL
