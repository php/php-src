--TEST--
PDO Common: Bug #65946 (pdo_sql_parser.c permanently converts values bound to strings)
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
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$db = PDOTest::factory();

$limitStatement = ' LIMIT :limit';
if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'firebird') {
    $limitStatement = ' ROWS :limit';
} else if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'oci') {
    $limitStatement = ' where ROWNUM <= :limit';
} else if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'dblib') {
    $limitStatement = ' ORDER BY id OFFSET 0 ROWS FETCH NEXT :limit ROWS ONLY';
}

// dblib does not support attributes.
if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) != 'dblib') {
    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
}

$db->exec('CREATE TABLE test(id int)');
$db->exec('INSERT INTO test VALUES(1)');
$stmt = $db->prepare('SELECT * FROM test' . $limitStatement);
$stmt->bindValue('limit', 1, PDO::PARAM_INT);
if(!($res = $stmt->execute())) var_dump($stmt->errorInfo());
if(!($res = $stmt->execute())) var_dump($stmt->errorInfo());
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
?>
--EXPECTF--
array(1) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
}