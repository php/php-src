--TEST--
PDO Common: PDO::FETCH_COLUMN
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10), val2 VARCHAR(20))');
$db->exec('INSERT INTO test VALUES(1, \'A\', \'A2\')');
$db->exec('INSERT INTO test VALUES(2, \'A\', \'B2\')');

$select1 = $db->prepare('SELECT id, val, val2 FROM test');
$select2 = $db->prepare('SELECT val, val2 FROM test');

$select1->execute();
var_dump($select1->fetchAll(PDO::FETCH_COLUMN));
$select1->execute();
var_dump($select1->fetchAll(PDO::FETCH_COLUMN, 2));
$select1->execute();
var_dump($select1->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP));
$select1->execute();
var_dump($select1->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE));
$select1->execute();
var_dump($select1->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 0));
$select1->execute();
var_dump($select1->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 1));
$select1->execute();
var_dump($select1->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_UNIQUE, 2));

$select2->execute();
var_dump($select2->fetchAll(PDO::FETCH_COLUMN|PDO::FETCH_GROUP));

?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
array(2) {
  [0]=>
  string(2) "A2"
  [1]=>
  string(2) "B2"
}
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
array(2) {
  [1]=>
  string(1) "A"
  [2]=>
  string(1) "A"
}
array(2) {
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
}
array(2) {
  [1]=>
  string(1) "A"
  [2]=>
  string(1) "A"
}
array(2) {
  [1]=>
  string(2) "A2"
  [2]=>
  string(2) "B2"
}
array(1) {
  ["A"]=>
  array(2) {
    [0]=>
    string(2) "A2"
    [1]=>
    string(2) "B2"
  }
}
