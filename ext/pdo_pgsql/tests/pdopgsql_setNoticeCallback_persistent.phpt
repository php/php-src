--TEST--
Pdo\Pgsql::setNoticeCallback() does not outlive the request on a persistent connection
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--CONFLICTS--
server
--FILE--
<?php
require __DIR__ . '/config.inc';
include __DIR__ . '/../../../sapi/cli/tests/php_cli_server.inc';

$code = '$db = new Pdo\Pgsql(' . var_export($config['ENV']['PDOTEST_DSN'], true) . ', null, null, [PDO::ATTR_PERSISTENT => true]);' . <<<'PHP'
if (isset($_GET['set'])) {
    $db->setNoticeCallback(function ($message) {
        echo trim($message), "\n";
    });
}
$db->exec("DO $$ BEGIN RAISE NOTICE 'request {$_GET['n']}'; END $$");
echo "request {$_GET['n']} done\n";
PHP;

$args = ['-d', 'extension_dir=' . ini_get('extension_dir')];
foreach (['pdo', 'pdo_pgsql'] as $ext) {
    $args[] = '-d';
    $args[] = 'extension=' . (PHP_OS_FAMILY === 'Windows' ? 'php_' : '') . $ext . '.' . PHP_SHLIB_SUFFIX;
}
if (php_ini_loaded_file()) {
    $args[] = '-c';
    $args[] = php_ini_loaded_file();
}
php_cli_server_start($code, 'index.php', $args);

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/?n=1&set=1');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/?n=2');
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/?n=3&set=1');
?>
--EXPECT--
NOTICE:  request 1
request 1 done
request 2 done
NOTICE:  request 3
request 3 done
