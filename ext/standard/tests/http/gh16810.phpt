--TEST--
GH-16809 (fopen HTTP wrapper timeout stream context option overflow)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
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
        'timeout' => PHP_INT_MIN,
    ],
];
$ctx = stream_context_create($config);
var_dump(fopen($uri, "r", false, $ctx));

$config['http']['timeout'] = PHP_INT_MAX;
$ctx = stream_context_create($config);
var_dump(fopen($uri, "r", false, $ctx));
?>
--EXPECTF--
resource(%d) of type (stream)

Warning: fopen(http://%s): Failed to open stream: timeout must be lower than %d in %s on line %d
bool(false)
