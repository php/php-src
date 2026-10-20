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
function report(PDO $pdo, PDOStatement $stmt): void
{
    $stmt->fetch();

    printf(
        "  connection: %s, statement: %s, rows delivered at once: %d\n",
        var_export($pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE), true),
        var_export($stmt->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE), true),
        $stmt->rowCount(),
    );
}

echo "=== nothing set ===\n";
report($pdo, $pdo->query("SELECT * FROM generate_series(1, 10)"));

echo "=== query() inherits a lazy ATTR_PREFETCH ===\n";
$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);
report($pdo, $pdo->query("SELECT * FROM generate_series(1, 10)"));

echo "=== the statement overrides an inherited ATTR_PREFETCH ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [PDO::ATTR_PREFETCH => 1]);
$stmt->execute();
report($pdo, $stmt);
unset($stmt);
$pdo->setAttribute(PDO::ATTR_PREFETCH, 1);

echo "=== set on the statement only ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [Pdo\Pgsql::ATTR_CHUNK_SIZE => 4]);
$stmt->execute();
report($pdo, $stmt);
unset($stmt);

echo "=== set on the connection ===\n";
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 3);
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)");
$stmt->execute();
report($pdo, $stmt);
unset($stmt);

echo "=== query() inherits the connection ===\n";
report($pdo, $pdo->query("SELECT * FROM generate_series(1, 10)"));

echo "=== the statement overrides the connection ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [Pdo\Pgsql::ATTR_CHUNK_SIZE => 6]);
$stmt->execute();
report($pdo, $stmt);
unset($stmt);

echo "=== a chunk size wins over a buffering ATTR_PREFETCH ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)",
    [PDO::ATTR_PREFETCH => 1, Pdo\Pgsql::ATTR_CHUNK_SIZE => 2]);
$stmt->execute();
report($pdo, $stmt);
unset($stmt);

echo "=== the statement asks to buffer ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [PDO::ATTR_PREFETCH => 1]);
$stmt->execute();
report($pdo, $stmt);
unset($stmt);

echo "=== the statement asks for lazy ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [PDO::ATTR_PREFETCH => 0]);
$stmt->execute();
report($pdo, $stmt);
unset($stmt);

echo "=== a statement can opt out of an inherited chunk size ===\n";
$stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", [Pdo\Pgsql::ATTR_CHUNK_SIZE => 0]);
$stmt->execute();
report($pdo, $stmt);
unset($stmt);

echo "=== back to 0 ===\n";
$pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 0);
report($pdo, $pdo->query("SELECT * FROM generate_series(1, 10)"));
?>
--EXPECT--
=== nothing set ===
  connection: 0, statement: 0, rows delivered at once: 10
=== query() inherits a lazy ATTR_PREFETCH ===
  connection: 0, statement: 0, rows delivered at once: 1
=== the statement overrides an inherited ATTR_PREFETCH ===
  connection: 0, statement: 0, rows delivered at once: 10
=== set on the statement only ===
  connection: 0, statement: 4, rows delivered at once: 4
=== set on the connection ===
  connection: 3, statement: 3, rows delivered at once: 3
=== query() inherits the connection ===
  connection: 3, statement: 3, rows delivered at once: 3
=== the statement overrides the connection ===
  connection: 3, statement: 6, rows delivered at once: 6
=== a chunk size wins over a buffering ATTR_PREFETCH ===
  connection: 3, statement: 2, rows delivered at once: 2
=== the statement asks to buffer ===
  connection: 3, statement: 0, rows delivered at once: 10
=== the statement asks for lazy ===
  connection: 3, statement: 0, rows delivered at once: 1
=== a statement can opt out of an inherited chunk size ===
  connection: 3, statement: 0, rows delivered at once: 10
=== back to 0 ===
  connection: 0, statement: 0, rows delivered at once: 10
