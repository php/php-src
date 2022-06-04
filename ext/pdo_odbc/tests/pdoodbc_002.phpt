--TEST--
PDO_mysql connect through PDO::connect
--EXTENSIONS--
PDO_odbc
--SKIPIF--
<?php
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require 'ext/pdo/tests/pdo_test.inc';
$config = PDOTest::get_config(__DIR__ . "/common.phpt");
$db = PdoOdbc::connect($config['ENV']['PDOTEST_DSN'], $config['ENV']['PDOTEST_USER'], $config['ENV']['PDOTEST_PASS']);
if (!$db instanceof PdoOdbc) {
    echo "Wrong class type. Should be PdoOdbc but is " . get_class($db) . "\n";
}

$db->exec('CREATE TABLE pdoodbc_002(id int NOT NULL PRIMARY KEY, name VARCHAR(10))');
$db->exec("INSERT INTO pdoodbc_002 VALUES(1, 'A'), (2, 'B'), (3, 'C')");

foreach ($db->query('SELECT name FROM pdoodbc_002') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("DROP TABLE IF EXISTS pdoodbc_002");
?>
--EXPECT--
array(2) {
  ["name"]=>
  string(1) "A"
  [0]=>
  string(1) "A"
}
array(2) {
  ["name"]=>
  string(1) "B"
  [0]=>
  string(1) "B"
}
array(2) {
  ["name"]=>
  string(1) "C"
  [0]=>
  string(1) "C"
}
Fin.
