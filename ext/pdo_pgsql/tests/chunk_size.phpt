--TEST--
PDO PgSQL Pdo\Pgsql::ATTR_CHUNK_SIZE splits a result set into chunks of rows
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
function run(PDO $pdo, string $label, array $options): void
{
    $stmt = $pdo->prepare("SELECT * FROM generate_series(1, 10)", $options);
    $stmt->execute();

    $values = [];
    $sizes = [];

    while (($row = $stmt->fetch(PDO::FETCH_NUM))) {
        $values[] = $row[0];
        $sizes[] = $stmt->rowCount();
    }

    printf("%s\n  values=%s\n  chunk sizes=%s\n",
        $label, implode(',', $values), implode(',', $sizes));
}

run($pdo, 'buffered (default)', []);
run($pdo, 'ATTR_PREFETCH => 0', [PDO::ATTR_PREFETCH => 0]);
run($pdo, 'ATTR_CHUNK_SIZE => 1', [Pdo\Pgsql::ATTR_CHUNK_SIZE => 1]);
run($pdo, 'ATTR_CHUNK_SIZE => 4 (last chunk is partial)', [Pdo\Pgsql::ATTR_CHUNK_SIZE => 4]);
run($pdo, 'ATTR_CHUNK_SIZE => 5 (divides evenly)', [Pdo\Pgsql::ATTR_CHUNK_SIZE => 5]);
run($pdo, 'ATTR_CHUNK_SIZE => 99 (larger than the result)', [Pdo\Pgsql::ATTR_CHUNK_SIZE => 99]);

?>
--EXPECT--
buffered (default)
  values=1,2,3,4,5,6,7,8,9,10
  chunk sizes=10,10,10,10,10,10,10,10,10,10
ATTR_PREFETCH => 0
  values=1,2,3,4,5,6,7,8,9,10
  chunk sizes=1,1,1,1,1,1,1,1,1,1
ATTR_CHUNK_SIZE => 1
  values=1,2,3,4,5,6,7,8,9,10
  chunk sizes=1,1,1,1,1,1,1,1,1,1
ATTR_CHUNK_SIZE => 4 (last chunk is partial)
  values=1,2,3,4,5,6,7,8,9,10
  chunk sizes=4,4,4,4,4,4,4,4,2,2
ATTR_CHUNK_SIZE => 5 (divides evenly)
  values=1,2,3,4,5,6,7,8,9,10
  chunk sizes=5,5,5,5,5,5,5,5,5,5
ATTR_CHUNK_SIZE => 99 (larger than the result)
  values=1,2,3,4,5,6,7,8,9,10
  chunk sizes=10,10,10,10,10,10,10,10,10,10
