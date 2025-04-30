--TEST--
Bug #53551 (PDOStatement execute segfaults for pdo_mysql driver)
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
$db = MySQLPDOTest::factory();

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);

$createSql = "CREATE TABLE `bug53551` (
  `count` BIGINT(20) UNSIGNED NOT NULL DEFAULT '0'
)";

$db->exec($createSql);
$db->exec("INSERT INTO bug53551 SET `count` = 1 ");
$db->exec("SET sql_mode = 'Traditional'");
$sql = 'UPDATE bug53551 SET `count` = :count';
$stmt = $db->prepare($sql);

$values = [
    'count' => null,
];

echo "1\n";
$stmt->execute($values);
var_dump($stmt->errorInfo());

echo "2\n";
$stmt->execute($values);
var_dump($stmt->errorInfo());

echo "\ndone\n";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS bug53551');
?>
--EXPECTF--
1

Warning: PDOStatement::execute(): SQLSTATE[23000]: Integrity constraint violation: 1048 Column 'count' cannot be null in %s on line %d
array(3) {
  [0]=>
  string(5) "23000"
  [1]=>
  int(1048)
  [2]=>
  string(29) "Column 'count' cannot be null"
}
2

Warning: PDOStatement::execute(): SQLSTATE[23000]: Integrity constraint violation: 1048 Column 'count' cannot be null in %s on line %d
array(3) {
  [0]=>
  string(5) "23000"
  [1]=>
  int(1048)
  [2]=>
  string(29) "Column 'count' cannot be null"
}

done
