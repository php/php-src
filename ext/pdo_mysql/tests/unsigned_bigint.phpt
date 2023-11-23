--TEST--
Bug GH-7837 (large bigints may be truncated)
--EXTENSIONS--
pdo_mysql
mysqlnd
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipNotMySQLnd();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();

$tbl = "unsigned_bigint_pdo_mysql";
$pdo->query("DROP TABLE IF EXISTS $tbl");
$pdo->query("CREATE TABLE $tbl (`ubigint` bigint unsigned NOT NULL) ENGINE=InnoDB");
$pdo->query("INSERT INTO $tbl (`ubigint`) VALUES (18446744073709551615)");
$pdo->query("INSERT INTO $tbl (`ubigint`) VALUES (9223372036854775808)");
$pdo->query("INSERT INTO $tbl (`ubigint`) VALUES (1)");
$result = $pdo->query("SELECT ubigint FROM $tbl")->fetchAll(PDO::FETCH_ASSOC);
var_dump($result);
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS unsigned_bigint_pdo_mysql');
?>
--EXPECT--
array(3) {
  [0]=>
  array(1) {
    ["ubigint"]=>
    string(20) "18446744073709551615"
  }
  [1]=>
  array(1) {
    ["ubigint"]=>
    string(19) "9223372036854775808"
  }
  [2]=>
  array(1) {
    ["ubigint"]=>
    int(1)
  }
}
