--TEST--
posix_get_last_error() does not return an error from a previous request
--EXTENSIONS--
posix
--CONFLICTS--
server
--FILE--
<?php
include __DIR__ . '/../../../sapi/cli/tests/php_cli_server.inc';

$code = <<<'PHP'
if (isset($_GET['fail'])) {
    var_dump(posix_kill(2147483647, 0));
    var_dump(posix_get_last_error() !== 0);
} else {
    var_dump(posix_get_last_error());
}
PHP;

$args = ['-d', 'extension_dir=' . ini_get('extension_dir')];
$args[] = '-d';
$args[] = 'extension=posix.' . PHP_SHLIB_SUFFIX;
if (php_ini_loaded_file()) {
    $args[] = '-c';
    $args[] = php_ini_loaded_file();
}
php_cli_server_start($code, 'index.php', $args);

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/?fail=1');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/');
?>
--EXPECT--
bool(false)
bool(true)
int(0)
