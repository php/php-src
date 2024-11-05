--TEST--
GH-16314 "Pdo\Mysql object is uninitialized" when opening a persistent connection
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

$pdo = MySQLPDOTest::factory(Pdo\Mysql::class, null, [PDO::ATTR_PERSISTENT => true], false);
var_dump($pdo->query('SELECT 1;')->fetchAll());

$pdo = MySQLPDOTest::factory(Pdo\Mysql::class, null, [PDO::ATTR_PERSISTENT => true], true);
var_dump($pdo->query('SELECT 1;')->fetchAll());

$pdo = MySQLPDOTest::factory(Pdo::class, null, [PDO::ATTR_PERSISTENT => true], false);
var_dump($pdo->query('SELECT 1;')->fetchAll());

$pdo = MySQLPDOTest::factory(Pdo::class, null, [PDO::ATTR_PERSISTENT => true], true);
var_dump($pdo->query('SELECT 1;')->fetchAll());
?>
--EXPECT--
array(1) {
  [0]=>
  array(2) {
    [1]=>
    int(1)
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(2) {
    [1]=>
    int(1)
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(2) {
    [1]=>
    int(1)
    [0]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(2) {
    [1]=>
    int(1)
    [0]=>
    int(1)
  }
}
