--TEST--
Bug #70389 (PDO constructor changes unrelated variables)
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
$flags = [
    PDO::MYSQL_ATTR_FOUND_ROWS	=> true,
    PDO::MYSQL_ATTR_LOCAL_INFILE	=> true,
    PDO::ATTR_PERSISTENT 		=> true,
];

$std = new stdClass();
$std->flags = $flags;

new PDO(PDO_MYSQL_TEST_DSN, PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS, $flags);
var_dump($flags);
?>
--EXPECTF--
array(3) {
  [%d]=>
  bool(true)
  [%d]=>
  bool(true)
  [%d]=>
  bool(true)
}
