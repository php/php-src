--TEST--
PDO PgSQL fetching while Pdo\Pgsql::ATTR_CHUNK_SIZE is in effect
--EXTENSIONS--
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
if (!defined('Pdo\Pgsql::ATTR_CHUNK_SIZE')) die('skip libpq >= 17 required');
?>
--FILE--
<?php

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 3);

$pdo->exec("CREATE TEMP TABLE t AS SELECT g AS n FROM generate_series(1, 10) g");

function values(PDOStatement $stmt): string
{
    $values = [];

    while (($row = $stmt->fetch(PDO::FETCH_NUM))) {
        $values[] = $row[0];
    }

    return implode(',', $values);
}

echo "=== fetch() ===\n";
echo "  values: ", values($pdo->query("SELECT n FROM t ORDER BY n")), "\n";

echo "=== fetchAll() ===\n";
echo "  values: ", implode(',', array_column(
    $pdo->query("SELECT n FROM t ORDER BY n")->fetchAll(PDO::FETCH_NUM), 0)), "\n";

echo "=== empty result set ===\n";
echo "  first fetch() returns: ";
var_dump($pdo->query("SELECT n FROM t WHERE false")->fetch());

echo "=== fewer rows than one chunk ===\n";
echo "  values: ", values($pdo->query("SELECT n FROM t WHERE n <= 2 ORDER BY n")), "\n";

echo "=== re-executing, including a run left unfinished ===\n";
$stmt = $pdo->prepare("SELECT n FROM t WHERE n <= ? ORDER BY n");
$stmt->execute([5]);
echo "  first run, drained: ", values($stmt), "\n";
$stmt->execute([10]);
$stmt->fetch();
$stmt->execute([4]);
echo "  third run, after abandoning the second: ", values($stmt), "\n";
unset($stmt);

echo "=== inside a transaction ===\n";
$pdo->beginTransaction();
echo "  values: ", values($pdo->query("SELECT n FROM t WHERE n <= 7 ORDER BY n")), "\n";
$pdo->commit();

echo "=== DML reports its own row count ===\n";
$pdo->exec("CREATE TEMP TABLE t2 (n int)");
$stmt = $pdo->prepare("INSERT INTO t2 SELECT n FROM t WHERE n <= 7");
$stmt->execute();
echo "  inserted rows: ";
var_dump($stmt->rowCount());

?>
--EXPECT--
=== fetch() ===
  values: 1,2,3,4,5,6,7,8,9,10
=== fetchAll() ===
  values: 1,2,3,4,5,6,7,8,9,10
=== empty result set ===
  first fetch() returns: bool(false)
=== fewer rows than one chunk ===
  values: 1,2
=== re-executing, including a run left unfinished ===
  first run, drained: 1,2,3,4,5
  third run, after abandoning the second: 1,2,3,4
=== inside a transaction ===
  values: 1,2,3,4,5,6,7
=== DML reports its own row count ===
  inserted rows: int(7)
