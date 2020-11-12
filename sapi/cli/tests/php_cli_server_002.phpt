--TEST--
$_SERVER variable
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["DOCUMENT_ROOT"], $_SERVER["SERVER_SOFTWARE"], $_SERVER["SERVER_NAME"], $_SERVER["SERVER_PORT"]);');
var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS));
?>
--EXPECTF--
string(%d) "string(%d) "%sphp_cli_server_002"
string(%d) "PHP %s Development Server"
string(%d) "localhost"
string(%d) "%s"
"
