--TEST--
Persistent connections: session state reset when performing disconnect-equivalent processing (general case)
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
putenv('PDOTEST_ATTR='.serialize([PDO::ATTR_PERSISTENT => true]));

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';

$pdo1 = PDOTest::test_factory(__DIR__ . '/common.phpt');

$pid1 = (int)$pdo1
    ->query('select pg_backend_pid()::int;')
    ->fetchColumn(0);

$defaultValue = (int)$pdo1
    ->query('show log_min_duration_statement;')
    ->fetchColumn(0);

$setValue = $defaultValue + 1;

$pdo1->exec("set log_min_duration_statement = {$setValue};");

$pdo1 = null;

$pdo2 = PDOTest::test_factory(__DIR__ . '/common.phpt');

$pid2 = (int)$pdo2
    ->query('select pg_backend_pid()::int;')
    ->fetchColumn(0);

assert($pid1 === $pid2);

$expectedValue = (int)$pdo2
    ->query('show log_min_duration_statement;')
    ->fetchColumn(0);

echo "defaultValue: {$defaultValue}\n";
echo "setValue: {$setValue}\n";
echo "expectedValue: {$expectedValue}\n";
echo "expected value should be reset to default: " . (($expectedValue === $defaultValue) ? 'success' : 'failure') . "\n";

?>
--EXPECTF--
defaultValue: %i
setValue: %d
expectedValue: %i
expected value should be reset to default: success
