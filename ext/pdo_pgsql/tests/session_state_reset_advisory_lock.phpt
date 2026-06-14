--TEST--
Persistent connections: session state reset when performing disconnect-equivalent processing (advisory lock case)
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

$lockResult1 = (bool)$pdo1
    ->query('select pg_try_advisory_lock(42)::int;')
    ->fetchColumn(0);

$pdo1 = null;

$dsn = getenv('PDO_PGSQL_TEST_DSN');
$dsn .= ';';
putenv('PDO_PGSQL_TEST_DSN='.$dsn);

$pdo2 = PDOTest::test_factory(__DIR__ . '/common.phpt');

$pid2 = (int)$pdo2
    ->query('select pg_backend_pid()::int;')
    ->fetchColumn(0);

assert($pid1 !== $pid2);

$lockResult2 = (bool)$pdo2
    ->query('select pg_try_advisory_lock(42)::int;')
    ->fetchColumn(0);

echo "lock1: " . ($lockResult1 ? 'success' : 'failure') . "\n";
echo "lock2: " . ($lockResult2 ? 'success' : 'failure') . "\n";

?>
--EXPECT--
lock1: success
lock2: success
