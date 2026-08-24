--TEST--
PDO PgSQL a lazy fetch whose stream was taken over reports no leftover rows
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
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$pdo->setAttribute(PDO::ATTR_PREFETCH, 0);

$first = $pdo->prepare("VALUES (1), (2)");
$first->execute();

$pdo->prepare("VALUES (1), (2)")->execute();

var_dump($first->fetchAll(PDO::FETCH_NUM));
?>
--EXPECT--
array(0) {
}
