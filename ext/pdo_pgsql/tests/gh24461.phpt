--TEST--
GH-24461 (Persistent connections: session reset keeps prepared statements)
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
$pid1 = (int) $pdo1->query('SELECT pg_backend_pid()')->fetchColumn();

$pdo1->exec("SET application_name = 'gh24461'");
$pdo1->exec('CREATE TEMP TABLE gh24461_tmp (i int)');
$pdo1->exec('PREPARE gh24461_stmt AS SELECT 42');

$pdo1 = null;

$pdo2 = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pid2 = (int) $pdo2->query('SELECT pg_backend_pid()')->fetchColumn();
var_dump($pid1 === $pid2);

var_dump($pdo2->query('SHOW application_name')->fetchColumn());
var_dump($pdo2->query("SELECT to_regclass('pg_temp.gh24461_tmp')")->fetchColumn());
var_dump($pdo2->query('EXECUTE gh24461_stmt')->fetchColumn());

$pdo2->exec('DEALLOCATE gh24461_stmt');
?>
--EXPECT--
bool(true)
string(0) ""
NULL
string(2) "42"
