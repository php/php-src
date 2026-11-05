--TEST--
PDO_DBLIB: Persistent handle error state does not outlive the request
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--ENV--
USE_ZEND_ALLOC=0
USE_TRACKED_ALLOC=1
--FILE--
<?php
require __DIR__ . '/../../../sapi/cli/tests/php_cli_server.inc';

$code = sprintf(<<<'PHP'
require %s;
[$dsn, $user, $pass] = getCredentials();
$db = new PDO($dsn, $user, $pass, [PDO::ATTR_PERSISTENT => true, PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT]);
$stmt = $db->prepare('SELECT 1');
$stmt->execute();
var_dump($stmt->fetchColumn());
$db->exec('SELECT * FROM ' . $_GET['table']);
echo $db->errorInfo()[2], "\n";
PHP, var_export(__DIR__ . '/config.inc', true));

$args = [];
foreach (['pdo', 'pdo_dblib'] as $ext) {
    if (file_exists(ini_get('extension_dir') . '/' . $ext . '.' . PHP_SHLIB_SUFFIX)) {
        $args[] = '-dextension=' . ini_get('extension_dir') . '/' . $ext . '.' . PHP_SHLIB_SUFFIX;
    }
}
php_cli_server_start($code, null, $args);

foreach (['pdo_dblib_missing_first', 'pdo_dblib_missing_second'] as $table) {
    echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/index.php?table=' . $table);
}
?>
--EXPECTF--
int(1)
Invalid object name 'pdo_dblib_missing_first'. [%d] (severity %d) []
int(1)
Invalid object name 'pdo_dblib_missing_second'. [%d] (severity %d) []
