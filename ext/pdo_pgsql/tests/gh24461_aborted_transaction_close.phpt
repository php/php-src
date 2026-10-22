--TEST--
GH-24461 (Persistent connections: statements closed inside an aborted transaction are closed on reset)
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
putenv('PDOTEST_ATTR='.serialize([PDO::ATTR_PERSISTENT => true, PDO::ATTR_EMULATE_PREPARES => false]));

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';

$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$pid1 = (int) $pdo->query('SELECT pg_backend_pid()')->fetchColumn();
$pdo->beginTransaction();
$stmt = $pdo->prepare("SELECT 'gh24461_aborted'");
$stmt->execute();
try {
    $pdo->exec('SELECT 1/0');
} catch (PDOException $e) {
}
$stmt = null;
$pdo->rollBack();
$pdo = null;

$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pid2 = (int) $pdo->query('SELECT pg_backend_pid()')->fetchColumn();
var_dump($pid1 === $pid2);
var_dump($pdo->query("SELECT count(*) FROM pg_prepared_statements WHERE statement = 'SELECT ''gh24461_aborted'''")->fetchColumn());
?>
--CREDITS--
Artem Ukrainskiy
--EXPECT--
bool(true)
string(1) "0"
