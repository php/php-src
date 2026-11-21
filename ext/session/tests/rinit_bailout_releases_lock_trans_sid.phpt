--TEST--
A request whose startup bails out after session.auto_start with session.use_trans_sid
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
--INI--
default_socket_timeout=5
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

$args = [
    '-d', 'extension_dir=' . ini_get('extension_dir'),
    '-d', 'session.auto_start=1',
    '-d', 'session.use_trans_sid=1',
    '-d', 'session.use_cookies=0',
    '-d', 'session.use_only_cookies=0',
    '-d', 'session.save_path=' . __DIR__,
    '-d', 'zend_test.bailout_in_rinit=1',
];
$builtin = shell_exec(escapeshellarg(getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY) . ' -n -m');
foreach (['session', 'zend_test'] as $ext) {
    if (!preg_match("/^$ext$/m", $builtin)) {
        array_push($args, '-d', "extension=$ext");
    }
}

php_cli_server_start('echo session_id();', null, $args);

$url = 'http://' . PHP_CLI_SERVER_ADDRESS . '/?PHPSESSID=rinitbailouttranssid';
var_dump(@file_get_contents($url));
var_dump(@file_get_contents($url));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/sess_rinitbailouttranssid');
?>
--EXPECT--
string(0) ""
string(20) "rinitbailouttranssid"
