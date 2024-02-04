--TEST--
PDO_mysql connect through PDO::connect
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

$db = Pdo::connect(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS);
if (!$db instanceof PdoMysql) {
    echo "Wrong class type. Should be PdoMysql but is " . get_class($db) . "\n";
}

$db->exec('CREATE TABLE pdomysql_002(id INT NOT NULL PRIMARY KEY, name VARCHAR(10))');
$db->exec("INSERT INTO pdomysql_002 VALUES(1, 'A'), (2, 'B'), (3, 'C')");

foreach ($db->query('SELECT name FROM pdomysql_002') as $row) {
    var_dump($row);
}

echo "Fin.";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE pdomysql_002');
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
