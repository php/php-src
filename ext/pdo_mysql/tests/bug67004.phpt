--TEST--
Bug #67004: Executing PDOStatement::fetch() more than once prevents releasing resultset
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
$dbh = MySQLPDOTest::factory();

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$dbh->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
$dbh->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, true);
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$stmt = $dbh->prepare("SELECT ?");

$stmt->execute(["foo"]);
var_dump($stmt->fetchColumn(0));

$stmt->execute(["bar"]);
var_dump($stmt->fetchColumn(0));

$stmt = $dbh->prepare("SELECT ?");
$stmt->execute(["baz"]);
var_dump($stmt->fetchColumn(0));

?>
--EXPECT--
string(3) "foo"
string(3) "bar"
string(3) "baz"
