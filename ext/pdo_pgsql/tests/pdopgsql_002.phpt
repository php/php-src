--TEST--
PdoPgsql connect through PDO::connect
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require dirname(__DIR__, 2) . '/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require_once __DIR__ . "/config.inc";

$db =  Pdo::connect($config['ENV']['PDOTEST_DSN']);
if (!$db instanceof PdoPgsql) {
    echo "Wrong class type. Should be PdoPgsql but is " . get_class($db) . "\n";
}

$db->exec('CREATE TABLE pdopgsql_002(id INT NOT NULL PRIMARY KEY, name VARCHAR(10))');
$db->exec("INSERT INTO pdopgsql_002 VALUES(1, 'A'), (2, 'B'), (3, 'C')");

foreach ($db->query('SELECT name FROM pdopgsql_002') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require __DIR__ . '/../../pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->query("DROP TABLE IF EXISTS pdopgsql_002");
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
