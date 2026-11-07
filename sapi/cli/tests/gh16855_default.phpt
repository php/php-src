--TEST--
GH-16855: built-in server default log level records requests
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
$info = php_cli_server_start('echo "ok";');
file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS);
$out = file_get_contents($info->outputFile);
echo (str_contains($out, 'Accepted') || str_contains($out, ' GET '))
    ? "request logs present\n"
    : "request logs missing\n";
?>
--EXPECT--
request logs present
