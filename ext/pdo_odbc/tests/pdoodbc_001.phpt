--TEST--
PDO_odbc subclass basic
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require 'ext/pdo/tests/pdo_test.inc';
$config = PDOTest::get_config(__DIR__ . "/common.phpt");
$db = new PdoOdbc($config['ENV']['PDOTEST_DSN'], $config['ENV']['PDOTEST_USER'], $config['ENV']['PDOTEST_PASS']);

$db->query("CREATE TABLE pdoodbc_001 (id INT, name TEXT)");
$db->query("INSERT INTO pdoodbc_001 VALUES (NULL, 'PHP'), (NULL, 'PHP6')");

foreach ($db->query('SELECT name FROM pdoodbc_001') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("DROP TABLE IF EXISTS pdoodbc_001");
?>
--EXPECT--
array(2) {
  ["name"]=>
  string(3) "PHP"
  [0]=>
  string(3) "PHP"
}
array(2) {
  ["name"]=>
  string(4) "PHP6"
  [0]=>
  string(4) "PHP6"
}
Fin.
