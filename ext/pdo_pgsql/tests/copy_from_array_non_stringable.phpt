--TEST--
PDO PgSQL pgsqlCopyFromArray()/copyFromArray() with a non-stringable row throws without polluting errorInfo
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec('CREATE TABLE test_copy_non_stringable (v text)');

try {
    $db->pgsqlCopyFromArray('test_copy_non_stringable', [new stdClass()]);
} catch (\Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

var_dump($db->errorInfo()[0]);

$pgsql = PDOTest::test_factory(__DIR__ . '/common.phpt', Pdo\Pgsql::class, true);
$pgsql->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

try {
    $pgsql->copyFromArray('test_copy_non_stringable', [new stdClass()]);
} catch (\Error $e) {
    echo $e->getMessage(), PHP_EOL;
}

var_dump($pgsql->errorInfo()[0]);
?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->query('DROP TABLE IF EXISTS test_copy_non_stringable CASCADE');
?>
--EXPECTF--
Deprecated: Method PDO::pgsqlCopyFromArray() is deprecated since 8.5, use Pdo\Pgsql::copyFromArray() instead in %s on line %d
Object of class stdClass could not be converted to string
string(5) "00000"
Object of class stdClass could not be converted to string
string(5) "00000"
