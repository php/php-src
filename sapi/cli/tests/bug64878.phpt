--TEST--
Bug #64878 (304 responses return Content-Type header)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('header("HTTP/1.1 304 Not Modified")', null);
$headers = get_headers('http://' . PHP_CLI_SERVER_ADDRESS);
echo count(array_filter($headers, function ($value) {
    return stripos($value, 'Content-Type') === 0;
}));
?>
--EXPECT--
0
