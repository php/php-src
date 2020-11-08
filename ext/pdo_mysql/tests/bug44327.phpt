--TEST--
Bug #44327 (PDORow::queryString property & numeric offsets / Crash)
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();

    $stmt = $db->prepare("SELECT 1 AS \"one\"");
    $stmt->execute();
    $row = $stmt->fetch(PDO::FETCH_LAZY);
    var_dump($row);
    var_dump($row->{0});
    var_dump($row->one);
    var_dump($row->queryString);

    print "----------------------------------\n";

    @$db->exec("DROP TABLE test");
    $db->exec("CREATE TABLE test (id INT)");
    $db->exec("INSERT INTO test(id) VALUES (1)");
    $stmt = $db->prepare("SELECT id FROM test");
    $stmt->execute();
    $row = $stmt->fetch(PDO::FETCH_LAZY);
    var_dump($row);
    var_dump($row->queryString);
    @$db->exec("DROP TABLE test");

    print "----------------------------------\n";

    $stmt = $db->prepare('foo');
    @$stmt->execute();
    $row = $stmt->fetch();
    var_dump($row->queryString);

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
object(PDORow)#%d (2) {
  ["queryString"]=>
  string(19) "SELECT id FROM test"
  ["id"]=>
  string(1) "1"
}
string(19) "SELECT id FROM test"
----------------------------------

Warning: Attempt to read property "queryString" on bool in %s on line %d
NULL
