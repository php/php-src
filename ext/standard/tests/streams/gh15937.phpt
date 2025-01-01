--TEST--
GH-15937 (stream overflow on timeout setting)
--FILE--
<?php
$serverCode = <<<'CODE'
echo 1;
CODE;

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$uri = "http://" . PHP_CLI_SERVER_ADDRESS . '/test';
$config = [
    'http' => [
        'timeout' => PHP_INT_MAX,
    ],
];
$ctx = stream_context_create($config);
var_dump(fopen("http://" . PHP_CLI_SERVER_ADDRESS . "/test", "r", false, $ctx));
?>
--EXPECTF--
Warning: fopen(http://%s): Failed to open stream: timeout must be lower than %d in %s on line %d
bool(false)
