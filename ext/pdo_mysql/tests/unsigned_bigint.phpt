--TEST--
Bug GH-7837 (large bigints may be truncated)
--EXTENSIONS--
pdo
pdo_mysql
mysqlnd
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
if (!MySQLPDOTest::isPDOMySQLnd()) die('skip only for mysqlnd');
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$pdo = MySQLPDOTest::factory();

$tbl = "test";
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
require dirname(__FILE__) . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
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
