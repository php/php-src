--TEST--
After a RINIT bailout, RSHUTDOWN runs after the output handlers are flushed
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = [
    '-d', 'extension_dir=' . ini_get('extension_dir'),
    '-d', 'zend_test.bailout_in_rinit=1',
    '-d', 'zend_test.rinit_bailout_putenv="ZEND_TEST_RINIT_BAILOUT=1"',
];
$builtin = shell_exec(escapeshellarg(getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY) . ' -n -m');
if (!preg_match("/^zend_test$/m", $builtin)) {
    array_push($args, '-d', 'extension=zend_test');
}

php_cli_server_start('var_dump(getenv("ZEND_TEST_RINIT_BAILOUT"));', null, $args);

function request(): string {
    $fp = stream_socket_client('tcp://' . PHP_CLI_SERVER_ADDRESS, $errno, $errstr, 5);
    stream_set_timeout($fp, 5);
    fwrite($fp, "GET / HTTP/1.0\r\nHost: " . PHP_CLI_SERVER_HOSTNAME . "\r\n\r\n");
    $response = stream_get_contents($fp);
    fclose($fp);

    [, $body] = explode("\r\n\r\n", $response, 2) + ['', ''];

    return "[$body]";
}

// zend_test's RINIT bails out, its output handler calls putenv() during the request shutdown
echo request(), "\n";
// ext/standard's RSHUTDOWN restored the environment
echo request(), "\n";
?>
--EXPECT--
[]
[bool(false)
]
