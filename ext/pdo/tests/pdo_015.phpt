--TEST--
PDO Common: PDO::FETCH_COLUMN
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

$db->exec('CREATE TABLE test015(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(20))');
$db->exec("INSERT INTO test015 VALUES(1, 'A', 'A2')");
$db->exec("INSERT INTO test015 VALUES(2, 'A', 'B2')");

$selectIdVal1Val2 = $db->prepare('SELECT id, val, val2 FROM test015');

$selectIdVal1Val2->execute();
echo '$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN)', PHP_EOL;
var_dump($selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN));

$selectIdVal1Val2->execute();
echo '$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN, 2)', PHP_EOL;
var_dump($selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN, 2));

$selectIdVal1Val2->execute();
echo '$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP)', PHP_EOL;
var_dump($selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP));

$selectIdVal1Val2->execute();
echo '$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE)', PHP_EOL;
var_dump($selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE));

$selectIdVal1Val2->execute();
echo '$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 0)', PHP_EOL;
var_dump($selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 0));

$selectIdVal1Val2->execute();
echo '$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 1)', PHP_EOL;
var_dump($selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 1));

$selectIdVal1Val2->execute();
echo '$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 2)', PHP_EOL;
var_dump($selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 2));

$selectVal1Val2 = $db->prepare('SELECT val, val2 FROM test015');
$selectVal1Val2->execute();
echo '$selectVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP)', PHP_EOL;
var_dump($selectVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test015");
?>
--EXPECT--
$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN)
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN, 2)
array(2) {
  [0]=>
  string(2) "A2"
  [1]=>
  string(2) "B2"
}
$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP)
array(2) {
  [1]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
  [2]=>
  array(1) {
    [0]=>
    string(1) "A"
  }
}
$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE)
array(2) {
  [1]=>
  string(1) "A"
  [2]=>
  string(1) "A"
}
$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 0)
array(2) {
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
}
$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 1)
array(2) {
  [1]=>
  string(1) "A"
  [2]=>
  string(1) "A"
}
$selectIdVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 2)
array(2) {
  [1]=>
  string(2) "A2"
  [2]=>
  string(2) "B2"
}
$selectVal1Val2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP)
array(1) {
  ["A"]=>
  array(2) {
    [0]=>
    string(2) "A2"
    [1]=>
    string(2) "B2"
  }
}
