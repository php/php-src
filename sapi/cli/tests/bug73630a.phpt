--TEST--
Bug #73630 (Built-in Weberver - overwrite $_SERVER['request_uri'])
--DESCRIPTION--
Check that too long paths result in invalid request
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
$code = <<<'EOF'
echo "won't happen\n";
EOF;

include "php_cli_server.inc";
php_cli_server_start($code);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();
$path = "/" . str_repeat("x", 16400) . "//example.com";
var_dump(file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "$path"));
?>
--EXPECTF--
Warning: file_get_contents(http://%s//example.com): Failed to open stream: HTTP request failed! in %s on line %d
bool(false)
