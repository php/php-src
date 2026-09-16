--TEST--
PDO PgSQL a connection's PDO::ATTR_PREFETCH replaces its Pdo\Pgsql::ATTR_CHUNK_SIZE
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

// rowCount() reports the size of the chunk being consumed while fetching unbuffered
function report(PDO $pdo, int $prefetch): void
{
    $pdo->setAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE, 5);
    $pdo->setAttribute(PDO::ATTR_PREFETCH, $prefetch);

    $stmt = $pdo->query("SELECT * FROM generate_series(1, 10)");
    $stmt->fetch();

    printf("PDO::ATTR_PREFETCH => %d: chunk size %d, rows delivered at once: %d\n",
        $prefetch, $pdo->getAttribute(Pdo\Pgsql::ATTR_CHUNK_SIZE), $stmt->rowCount());
}

report($pdo, 1);
report($pdo, 0);

?>
--EXPECT--
PDO::ATTR_PREFETCH => 1: chunk size 0, rows delivered at once: 10
PDO::ATTR_PREFETCH => 0: chunk size 0, rows delivered at once: 1
