--TEST--
Bug #68291 (404 on urls with '+')
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
$docRoot = php_cli_server_start(NULL, NULL)->docRoot;
file_put_contents($docRoot . '/bug68291+test.html', 'Found');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/bug68291+test.html');
@unlink($docRoot . '/bug68291+test.html');
?>
--EXPECT--
Found
