--TEST--
PDO Common: PDO::FETCH_GROUP
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

$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->exec("INSERT INTO test VALUES(1, 'A')");
$db->exec("INSERT INTO test VALUES(2, 'A')");
$db->exec("INSERT INTO test VALUES(3, 'C')");

$stmt = $db->prepare('SELECT val, id from test');

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_NUM|PDO::FETCH_GROUP));

$stmt->execute();
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC|PDO::FETCH_GROUP));

?>
--EXPECT--
array(2) {
  ["A"]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      string(1) "1"
    }
    [1]=>
    array(1) {
      [0]=>
      string(1) "2"
    }
  }
  ["C"]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      string(1) "3"
    }
  }
}
array(2) {
  ["A"]=>
  array(2) {
    [0]=>
    array(1) {
      ["id"]=>
      string(1) "1"
    }
    [1]=>
    array(1) {
      ["id"]=>
      string(1) "2"
    }
  }
  ["C"]=>
  array(1) {
    [0]=>
    array(1) {
      ["id"]=>
      string(1) "3"
    }
  }
}
