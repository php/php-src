--TEST--
PDO PgSQL a lazy fetch left in a COPY does not hang the connection cleanup
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

require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);
$pdo->exec("CREATE TEMPORARY TABLE lazy_fetch_copy (i int)");

foreach ([
    'COPY OUT' => "COPY (SELECT 1) TO STDOUT",
    'COPY IN' => "COPY lazy_fetch_copy FROM STDIN",
] as $label => $sql) {
    $copy = $pdo->prepare($sql);
    $copy->execute();

    $stmt = $pdo->prepare("VALUES (1), (2)");
    $stmt->execute();
    echo "$label: ";
    var_dump((bool) $stmt->fetchAll());
}
?>
--EXPECT--
COPY OUT: bool(true)
COPY IN: bool(true)
