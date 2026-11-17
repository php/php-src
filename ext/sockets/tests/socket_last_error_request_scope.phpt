--TEST--
socket_last_error() does not return an error from a previous request
--EXTENSIONS--
sockets
--CONFLICTS--
server
--FILE--
<?php
include __DIR__ . '/../../../sapi/cli/tests/php_cli_server.inc';

$code = <<<'PHP'
if (isset($_GET['fail'])) {
    $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
    // 192.0.2.1 is reserved for documentation, so it isn't a local address
    var_dump(@socket_bind($socket, '192.0.2.1', 0));
    var_dump(socket_last_error() !== 0);
} else {
    var_dump(socket_last_error());
}
PHP;

$args = ['-d', 'extension_dir=' . ini_get('extension_dir')];
$args[] = '-d';
$args[] = 'extension=' . (PHP_OS_FAMILY === 'Windows' ? 'php_' : '') . 'sockets.' . PHP_SHLIB_SUFFIX;
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
