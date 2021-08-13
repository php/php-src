--TEST--
PDO Common: PECL Bug #5809 (PDOStatement::execute(array()) changes param)
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

$db->exec("CREATE TABLE test (id int NOT NULL, PRIMARY KEY (id))");
$db->exec("INSERT INTO test (id) VALUES (1)");

$values = array(1);
var_dump($values);
$stmt = $db->prepare('SELECT * FROM test WHERE id = ?');
$stmt->execute($values);
var_dump($values);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(1)
}
