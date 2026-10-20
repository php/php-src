--TEST--
Persistent connections: session state reset after backend termination (interrupted case)
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

$pid1 = (int)$pdo1
    ->query('select pg_backend_pid()::int;')
    ->fetchColumn(0);

$dsn = getenv('PDO_PGSQL_TEST_DSN');
$dsn .= ';';
putenv('PDO_PGSQL_TEST_DSN='.$dsn);

$pdo2 = PDOTest::test_factory(__DIR__ . '/common.phpt');

$pid2 = (int)$pdo2
    ->query('select pg_backend_pid()::int;')
    ->fetchColumn(0);

assert($pid1 !== $pid2);

$terminateResult = (bool)$pdo2
    ->query("select pg_terminate_backend({$pid1})::int")
    ->fetchColumn(0);

// Disconnect after being terminated by another connection
$pdo1 = null;

echo 'pid of pdo1: ' . $pid1 . "\n";
echo 'terminate result of pdo1 by pdo2: ' . ($terminateResult ? 'success' : 'failure') . "\n";

?>
--EXPECTF--
pid of pdo1: %d
terminate result of pdo1 by pdo2: success
