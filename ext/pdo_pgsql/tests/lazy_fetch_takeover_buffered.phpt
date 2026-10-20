--TEST--
PDO PgSQL a lazy fetch stale after a buffered query does not read the next statement's rows
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

$first = $pdo->prepare("VALUES (1), (2), (3)", [PDO::ATTR_PREFETCH => 0]);
$first->execute();
$first->fetch();

// a buffered query drains the stream but does not end $first's lazy fetch
$pdo->prepare("VALUES (99)")->execute();

$third = $pdo->prepare("VALUES (777), (888)", [PDO::ATTR_PREFETCH => 0]);
$third->execute();

var_dump($first->fetch(PDO::FETCH_NUM));
var_dump($third->fetchAll(PDO::FETCH_COLUMN));
?>
--EXPECT--
bool(false)
array(2) {
  [0]=>
  string(3) "777"
  [1]=>
  string(3) "888"
}
