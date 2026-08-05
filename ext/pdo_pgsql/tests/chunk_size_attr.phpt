--TEST--
PDO PgSQL Pdo\Pgsql::ATTR_CHUNK_SIZE resolution between the connection and the statement
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

// rowCount() reports the size of the chunk being consumed
function report(PDOStatement $stmt): void
{
    $stmt->fetch();

    echo "  rows delivered at once: ";
    var_dump($stmt->rowCount());
}

echo "=== nothing set ===\n";
echo "  connection attribute: ";
var_dump($pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));
report($pdo->query("SELECT * FROM generate_series(1, 10)"));

echo "=== set on the statement only ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [Pdo\Pgsql::ATTR_CHUNK_SIZE => 4]);
$stmt->execute();
echo "  statement attribute: ";
var_dump($stmt->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));
report($stmt);
unset($stmt);

echo "=== set on the connection ===\n";
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 3);
echo "  connection attribute: ";
var_dump($pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)");
$stmt->execute();
report($stmt);
unset($stmt);

echo "=== query() inherits the connection ===\n";
report($pdo->query("SELECT * FROM generate_series(1, 10)"));

echo "=== the statement overrides the connection ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [Pdo\Pgsql::ATTR_CHUNK_SIZE => 6]);
$stmt->execute();
echo "  statement attribute: ";
var_dump($stmt->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));
report($stmt);
unset($stmt);

echo "=== a chunk size wins over a buffering ATTR_PREFETCH ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)",
    [PDO::ATTR_PREFETCH => 1, Pdo\Pgsql::ATTR_CHUNK_SIZE => 2]);
$stmt->execute();
report($stmt);
unset($stmt);

echo "=== a statement can opt out of an inherited chunk size ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [Pdo\Pgsql::ATTR_CHUNK_SIZE => 0]);
$stmt->execute();
report($stmt);
unset($stmt);

echo "=== back to 0 ===\n";
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 0);
echo "  connection attribute: ";
var_dump($pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE));
report($pdo->query("SELECT * FROM generate_series(1, 10)"));
?>
--EXPECT--
=== nothing set ===
  connection attribute: int(0)
  rows delivered at once: int(10)
=== set on the statement only ===
  statement attribute: int(4)
  rows delivered at once: int(4)
=== set on the connection ===
  connection attribute: int(3)
  rows delivered at once: int(3)
=== query() inherits the connection ===
  rows delivered at once: int(3)
=== the statement overrides the connection ===
  statement attribute: int(6)
  rows delivered at once: int(6)
=== a chunk size wins over a buffering ATTR_PREFETCH ===
  rows delivered at once: int(2)
=== a statement can opt out of an inherited chunk size ===
  rows delivered at once: int(10)
=== back to 0 ===
  connection attribute: int(0)
  rows delivered at once: int(10)
