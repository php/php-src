--TEST--
GH-8626: PDOStatement->execute() failed, then execute successfully, errorInfo() information is incorrect
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
$driver = substr(getenv('PDOTEST_DSN'), 0, strpos(getenv('PDOTEST_DSN'), ':'));
if (!in_array($driver, array('mysql', 'pgsql', 'sqlite'))) {
    die('skip not supported');
}
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec('DROP TABLE test');
$db->exec('CREATE TABLE test (x int NOT NULL)');

$stmt = $db->prepare('INSERT INTO test VALUES(?)');

// fail
var_dump($stmt->execute([null]), $stmt->errorCode(), $stmt->errorInfo());

$stmt->closeCursor();

// success
var_dump($stmt->execute([1]), $stmt->errorCode(), $stmt->errorInfo());
?>
===DONE===
--EXPECTREGEX--
bool\(false\)
string\(\d+\) "[^"]+"
array\(3\) {
  \[0\]=>
  string\(\d+\) "[^"]+"
  \[1\]=>
  int\(\d+\)
  \[2\]=>
  string\(\d+\) "[^"]+((.+".+)*[^"]+)?"
}
bool\(true\)
string\(5\) "00000"
array\(3\) {
  \[0\]=>
  string\(5\) "00000"
  \[1\]=>
  NULL
  \[2\]=>
  NULL
}
===DONE===
