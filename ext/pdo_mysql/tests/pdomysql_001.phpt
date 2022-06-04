--TEST--
PDO_mysql subclass basic
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php

require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$db = MySQLPDOTest::factory(PdoMysql::class);

$db->query('CREATE TABLE IF NOT EXISTS pdomysql_001 (id INT, name TEXT)');

$db->query('INSERT INTO pdomysql_001 VALUES (NULL, "PHP")');
$db->query('INSERT INTO pdomysql_001 VALUES (NULL, "PHP6")');

foreach ($db->query('SELECT name FROM pdomysql_001') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdomysql_001');
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
