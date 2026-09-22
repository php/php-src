--TEST--
Bug GH-16137 duplicate *Forwarded* HTTP headers values.
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start("echo \$_SERVER['HTTP_X_FORWARDED_FOR'];");
$ctx = stream_context_create(array('http' => array (
    'method' => 'POST',
    'header' => array('x-forwarded-for: 127.0.0.1', 'x-forwarded-for: 192.168.1.254')
)));
var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS, true, $ctx));
?>
--EXPECT--
string(24) "127.0.0.1, 192.168.1.254"
