--TEST--
PDO Common: PDO::ATTR_CASE
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE test032(id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->exec("INSERT INTO test032 VALUES(1, 'A')");
$db->exec("INSERT INTO test032 VALUES(2, 'B')");
$db->exec("INSERT INTO test032 VALUES(3, 'C')");

// Lower case columns
$db->setAttribute(PDO::ATTR_CASE, PDO::CASE_LOWER);
$stmt = $db->prepare('SELECT * from test032');
$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$stmt->closeCursor();

// Upper case columns
$db->setAttribute(PDO::ATTR_CASE, PDO::CASE_UPPER);
$stmt = $db->prepare('SELECT * from test032');
$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$stmt->closeCursor();

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test032");
?>
--EXPECT--
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
  }
  [2]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
  }
}
array(3) {
  [0]=>
  array(2) {
    ["ID"]=>
    string(1) "1"
    ["VAL"]=>
    string(1) "A"
  }
  [1]=>
  array(2) {
    ["ID"]=>
    string(1) "2"
    ["VAL"]=>
    string(1) "B"
  }
  [2]=>
  array(2) {
    ["ID"]=>
    string(1) "3"
    ["VAL"]=>
    string(1) "C"
  }
}
