--TEST--
PDO Common: bindValue
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
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

$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val1 VARCHAR(10), val2 VARCHAR(10), val3 VARCHAR(10))');
$stmt = $db->prepare('INSERT INTO test values (1, ?, ?, ?)');

$data = array("one", "two", "three");

foreach ($data as $i => $v) {
    $stmt->bindValue($i+1, $v);
}
$stmt->execute();

$stmt = $db->prepare('SELECT * from test');
$stmt->execute();

var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
?>
--EXPECT--
array(1) {
  [0]=>
  array(4) {
    ["id"]=>
    string(1) "1"
    ["val1"]=>
    string(3) "one"
    ["val2"]=>
    string(3) "two"
    ["val3"]=>
    string(5) "three"
  }
}
