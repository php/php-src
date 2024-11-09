--TEST--
GHSA-4w77-75f9-2c8w (Heap-Use-After-Free in sapi_read_post_data Processing in CLI SAPI Interface)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

$serverCode = <<<'CODE'
var_dump(file_get_contents('php://input'));
CODE;

php_cli_server_start($serverCode, null, []);

$options = [
    "http" => [
        "method" => "POST",
        "header" => "Content-Type: application/x-www-form-urlencoded",
        "content" => "AAAAA",
    ],
];
$context = stream_context_create($options);

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/", context: $context);

$options = [
    "http" => [
        "method" => "POST",
    ],
];
$context = stream_context_create($options);

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/", context: $context);
?>
--EXPECT--
string(5) "AAAAA"
string(0) ""
