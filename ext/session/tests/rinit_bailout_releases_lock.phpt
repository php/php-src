--TEST--
A request whose startup bails out after session.auto_start releases the session lock
--EXTENSIONS--
session
zend_test
--SKIPIF--
<?php
include 'skipif.inc';
if (!file_exists(__DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc")) {
    echo "skip sapi/cli/tests/php_cli_server.inc required but not found";
}
?>
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$savePath = __DIR__ . '/rinit_bailout_releases_lock_sessions';
@mkdir($savePath);

$args = [
    '-d', 'extension_dir=' . ini_get('extension_dir'),
    '-d', 'session.auto_start=1',
    '-d', 'session.save_handler=files',
    '-d', 'session.save_path=' . $savePath,
    '-d', 'zend_test.bailout_in_rinit=1',
];
$builtin = shell_exec(escapeshellarg(getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY) . ' -n -m');
foreach (['session', 'zend_test'] as $ext) {
    if (!preg_match("/^$ext$/m", $builtin)) {
        array_push($args, '-d', "extension=$ext");
    }
}

php_cli_server_start('echo session_id();', null, $args);

function request(): string {
    $fp = stream_socket_client('tcp://' . PHP_CLI_SERVER_ADDRESS, $errno, $errstr, 5);
    stream_set_timeout($fp, 5);
    fwrite($fp, "GET / HTTP/1.0\r\nHost: " . PHP_CLI_SERVER_HOSTNAME . "\r\nCookie: PHPSESSID=rinitbailout\r\n\r\n");
    $response = stream_get_contents($fp);
    fclose($fp);

    [, $body] = explode("\r\n\r\n", $response, 2) + ['', ''];

    return "[$body]";
}

// the first request fails in zend_test's RINIT, after the session module locked the session
echo request(), "\n";
// blocks if the session is still locked
echo request(), "\n";
?>
--CLEAN--
<?php
$savePath = __DIR__ . '/rinit_bailout_releases_lock_sessions';
foreach (glob("$savePath/*") as $file) {
    unlink($file);
}
@rmdir($savePath);
?>
--EXPECT--
[]
[rinitbailout]
