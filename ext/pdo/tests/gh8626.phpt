--TEST--
GH-8626: PDOStatement->execute() failed, then execute successfully, errorInfo() information is incorrect
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();
if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'oci') {
    die("xfail OCI driver errorInfo is inconsistent with other PDO drivers");
}
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec('CREATE TABLE test8626 (x int NOT NULL)');

$stmt = $db->prepare('INSERT INTO test8626 VALUES(?)');

// fail
var_dump($stmt->execute([null]), $stmt->errorCode());
$errorInfo = $stmt->errorInfo();
var_dump(array_slice($errorInfo, 0, 3)); // odbc, dblib

$stmt->closeCursor(); // sqlite

// success
var_dump($stmt->execute([1]), $stmt->errorCode());
$errorInfo = $stmt->errorInfo();
var_dump(array_slice($errorInfo, 0, 3)); // odbc, dblib
?>
===DONE===
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test8626");
?>
--EXPECTF--
bool(false)
string(%d) "%s"
array(3) {
  [0]=>
  string(%d) "%s"
  [1]=>
  int(%i)
  [2]=>
  string(%d) "%s%w%S"
}
bool(true)
string(5) "00000"
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
===DONE===
