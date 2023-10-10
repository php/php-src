--TEST--
PDO Common: PDO::FETCH_BOTH
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

$db->exec('CREATE TABLE test003(id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->exec("INSERT INTO test003 VALUES(1, 'A')");
$db->exec("INSERT INTO test003 VALUES(2, 'B')");
$db->exec("INSERT INTO test003 VALUES(3, 'C')");

$stmt = $db->prepare('SELECT * from test003');
$stmt->execute();

var_dump($stmt->fetchAll(PDO::FETCH_BOTH));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test003");
?>
--EXPECT--
array(3) {
  [0]=>
  array(4) {
    ["id"]=>
    string(1) "1"
    [0]=>
    string(1) "1"
    ["val"]=>
    string(1) "A"
    [1]=>
    string(1) "A"
  }
  [1]=>
  array(4) {
    ["id"]=>
    string(1) "2"
    [0]=>
    string(1) "2"
    ["val"]=>
    string(1) "B"
    [1]=>
    string(1) "B"
  }
  [2]=>
  array(4) {
    ["id"]=>
    string(1) "3"
    [0]=>
    string(1) "3"
    ["val"]=>
    string(1) "C"
    [1]=>
    string(1) "C"
  }
}
