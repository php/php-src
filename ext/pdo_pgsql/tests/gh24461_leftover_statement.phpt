--TEST--
GH-24461 (Persistent connections: statements outliving their PDO object are closed on reset)
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

class HoldingPDO extends PDO {
    public $stmt;
}

$pdo1 = PDOTest::test_factory(__DIR__ . '/common.phpt', HoldingPDO::class);
$pid1 = (int) $pdo1->query('SELECT pg_backend_pid()')->fetchColumn();
$pdo1->stmt = $pdo1->prepare("SELECT 'gh24461'");
$pdo1->stmt->execute();

unset($pdo1);
var_dump(gc_collect_cycles() > 0);

$pdo2 = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pid2 = (int) $pdo2->query('SELECT pg_backend_pid()')->fetchColumn();
var_dump($pid1 === $pid2);
var_dump($pdo2->query("SELECT count(*) FROM pg_prepared_statements WHERE statement = 'SELECT ''gh24461'''")->fetchColumn());
?>
--EXPECT--
bool(true)
bool(true)
string(1) "0"
