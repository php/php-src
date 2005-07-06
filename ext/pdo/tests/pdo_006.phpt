--TEST--
PDO Common: PDO_FETCH_GROUP
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) print 'skip';
if (false == getenv('REDIR_TEST_DIR')) print 'skip no driver'; ?>
--FILE--
<?php
require getenv('REDIR_TEST_DIR') . 'pdo_test.php';
$db = PDOTest::factory();

$db->exec('CREATE TABLE test(id int NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->exec("INSERT INTO test VALUES(1, 'A')");
$db->exec("INSERT INTO test VALUES(2, 'A')");
$db->exec("INSERT INTO test VALUES(3, 'C')");

$stmt = $db->prepare('SELECT val, id from test');

$stmt->execute();
var_dump($stmt->fetchAll(PDO_FETCH_NUM|PDO_FETCH_GROUP));

$stmt->execute();
var_dump($stmt->fetchAll(PDO_FETCH_ASSOC|PDO_FETCH_GROUP));

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
