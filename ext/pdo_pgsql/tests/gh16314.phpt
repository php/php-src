--TEST--
GH-16314 "Pdo\Pgsql object is uninitialized" when opening a persistent connection
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

$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt', Pdo\Pgsql::class, false);
var_dump($pdo->query('SELECT 1;')->fetchAll());

$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt', Pdo\Pgsql::class, true);
var_dump($pdo->query('SELECT 1;')->fetchAll());

$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt', Pdo::class, false);
var_dump($pdo->query('SELECT 1;')->fetchAll());

$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt', Pdo::class, true);
var_dump($pdo->query('SELECT 1;')->fetchAll());
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    ["?column?"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
}
array(1) {
  [0]=>
  array(2) {
    ["?column?"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
}
array(1) {
  [0]=>
  array(2) {
    ["?column?"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
}
array(1) {
  [0]=>
  array(2) {
    ["?column?"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
  }
}
