--TEST--
PdoPgsql subclass basic
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php

require_once __DIR__ . "/config.inc";

$db = new PdoPgsql($config['ENV']['PDOTEST_DSN']);

$db->query('CREATE TABLE IF NOT EXISTS pdopgsql_001 (id INT, name TEXT)');
$db->query("INSERT INTO pdopgsql_001 VALUES (NULL, 'PHP'), (NULL, 'PHP6')");

foreach ($db->query('SELECT name FROM pdopgsql_001') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->query("DROP TABLE IF EXISTS pdopgsql_001");
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
