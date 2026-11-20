--TEST--
pcntl_signal() handlers are replaced by the previous handler, not SIG_DFL, at request shutdown
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
include __DIR__ . "/../../../sapi/cli/tests/skipif.inc";
$php = getenv('TEST_PHP_EXECUTABLE');
if (trim((string) shell_exec(escapeshellarg($php) . ' -n -r "echo (int) extension_loaded(\'pcntl\');"')) !== '1') {
    die('skip pcntl must be built statically for the server started with -n');
}
?>
--FILE--
<?php
include __DIR__ . "/../../../sapi/cli/tests/php_cli_server.inc";

// The built-in server ignores SIGPIPE
$server = php_cli_server_start(<<<'PHP'
if (isset($_GET['install'])) {
    pcntl_signal(SIGPIPE, function () {});
}
echo "ok\n";
PHP);

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/?install");

proc_terminate($server->processHandle, SIGPIPE);
usleep(200000);

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/");
?>
--EXPECT--
ok
ok
