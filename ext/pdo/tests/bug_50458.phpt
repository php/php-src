--TEST--
PDO Common: Bug #50458 (PDO::FETCH_FUNC fails with Closures)
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
$db->exec("CREATE TABLE test50458 (a VARCHAR(10))");
$db->exec("INSERT INTO test50458 (a) VALUES ('xyz')");
$res = $db->query("SELECT a FROM test50458");
var_dump($res->fetchAll(PDO::FETCH_FUNC, function($a) { return strtoupper($a); }));

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, "test50458");
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "XYZ"
}
