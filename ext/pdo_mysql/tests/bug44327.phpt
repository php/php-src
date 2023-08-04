--TEST--
Bug #44327 (PDORow::queryString property & numeric offsets / Crash)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    $stmt = $db->prepare("SELECT 1 AS \"one\"");
    $stmt->execute();
    $row = $stmt->fetch(PDO::FETCH_LAZY);
    var_dump($row);
    var_dump($row->{0});
    var_dump($row->one);
    var_dump($row->queryString);

    print "----------------------------------\n";

    $db->exec("CREATE TABLE test_44327 (id INT)");
    $db->exec("INSERT INTO test_44327(id) VALUES (1)");
    $stmt = $db->prepare("SELECT id FROM test_44327");
    $stmt->execute();
    $row = $stmt->fetch(PDO::FETCH_LAZY);
    var_dump($row);
    var_dump($row->queryString);

    print "----------------------------------\n";

    $stmt = $db->prepare('foo');
    @$stmt->execute();
    $row = $stmt->fetch();
    var_dump($row->queryString);

?>
--CLEAN--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
$db = MySQLPDOTest::factory();
$db->exec("DROP TABLE test_44327");
?>
--EXPECTF--
object(PDORow)#%d (2) {
  ["queryString"]=>
  string(17) "SELECT 1 AS "one""
  ["one"]=>
  string(1) "1"
}
string(1) "1"
string(1) "1"
string(17) "SELECT 1 AS "one""
----------------------------------
object(PDORow)#5 (2) {
  ["queryString"]=>
  string(25) "SELECT id FROM test_44327"
  ["id"]=>
  string(1) "1"
}
string(25) "SELECT id FROM test_44327"
----------------------------------

Warning: Attempt to read property "queryString" on false in %s on line %d
NULL
