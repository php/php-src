--TEST--
Bug #80458 PDOStatement::fetchAll() throws for upsert queries
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_mysql')) die('skip not loaded');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');

$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);

$db->query('DROP TABLE IF EXISTS test');
$db->query('CREATE TABLE test (first int) ENGINE = InnoDB');
$res = $db->query('INSERT INTO test(first) VALUES (1),(2),(3),(4),(5),(6),(7),(8),(9)');
var_dump($res->fetchAll());

$stmt = $db->prepare('DELETE FROM test WHERE first=1');
$stmt->execute();
var_dump($stmt->fetchAll());

$stmt2 = $db->prepare('DELETE FROM test WHERE first=2');
$stmt2->execute();
foreach($stmt2 as $row){
    // expect nothing
}

$stmt3 = $db->prepare('DELETE FROM test WHERE first=3');
$stmt3->execute();
if(false !== $stmt3->fetch(PDO::FETCH_ASSOC)) {
    print("Expecting false");
}

$stmt = $db->prepare('SELECT first FROM test WHERE first=4');
$stmt->execute();
var_dump($stmt->fetchAll());

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);

$stmt3 = $db->prepare('DELETE FROM test WHERE first=5');
$stmt3->execute();
if(false !== $stmt3->fetch(PDO::FETCH_ASSOC)) {
    print("Expecting false");
}

$stmt = $db->prepare('SELECT first FROM test WHERE first=6');
$stmt->execute();
var_dump($stmt->fetchAll());

?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  array(2) {
    ["first"]=>
    int(4)
    [0]=>
    int(4)
  }
}
array(1) {
  [0]=>
  array(2) {
    ["first"]=>
    string(1) "6"
    [0]=>
    string(1) "6"
  }
}