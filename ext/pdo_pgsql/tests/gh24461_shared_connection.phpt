--TEST--
GH-24461 (Persistent connections: session state is not reset while another PDO object still uses the connection)
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
$pdo2 = PDOTest::test_factory(__DIR__ . '/common.phpt');

$pid1 = (int) $pdo1->query('SELECT pg_backend_pid()')->fetchColumn();
$pid2 = (int) $pdo2->query('SELECT pg_backend_pid()')->fetchColumn();
var_dump($pid1 === $pid2);

$pdo2->exec("SET application_name = 'gh24461'");
$pdo2->exec('CREATE TEMP TABLE gh24461_shared (i int)');
$pdo2->query('SELECT pg_advisory_lock(24461)');
$stmt = $pdo2->prepare('SELECT 42');
$stmt->execute();
$stmt->closeCursor();

$pdo1 = null;

var_dump($pdo2->query('SHOW application_name')->fetchColumn());
var_dump($pdo2->query('SELECT count(*) FROM gh24461_shared')->fetchColumn());
var_dump($pdo2->query("SELECT count(*) FROM pg_locks WHERE locktype = 'advisory' AND objid = 24461 AND pid = pg_backend_pid()")->fetchColumn());
$stmt->execute();
var_dump($stmt->fetchColumn());

$pdo2->query('SELECT pg_advisory_unlock_all()');
?>
--EXPECT--
bool(true)
string(7) "gh24461"
string(1) "0"
string(1) "1"
string(2) "42"
