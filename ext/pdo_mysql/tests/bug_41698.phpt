--TEST--
PDO MySQL Bug #41698 (float parameters truncated to integer in prepared statements)
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

setlocale(LC_ALL, "de","de_DE","de_DE.ISO8859-1","de_DE.ISO_8859-1","de_DE.UTF-8");

$db->exec('CREATE TABLE test(floatval DECIMAL(8,6))');
$db->exec('INSERT INTO test VALUES(2.34)');
$value=4.56;
$stmt = $db->prepare('INSERT INTO test VALUES(?)');
$stmt->execute(array($value));
var_dump($db->query('SELECT * from test')->fetchAll(PDO::FETCH_ASSOC));
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["floatval"]=>
    string(8) "2.340000"
  }
  [1]=>
  array(1) {
    ["floatval"]=>
    string(8) "4.560000"
  }
}
