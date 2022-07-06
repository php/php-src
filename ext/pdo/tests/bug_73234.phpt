--TEST--
PDO Common: Bug #73234 (Emulated statements let value dictate parameter type)
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
$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$db->exec('CREATE TABLE test(id INT NULL)');

$stmt = $db->prepare('INSERT INTO test VALUES(:value)');

$stmt->bindValue(':value', 0, PDO::PARAM_NULL);
$stmt->execute();

$stmt->bindValue(':value', null, PDO::PARAM_NULL);
$stmt->execute();

$stmt = $db->query('SELECT * FROM test');
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
?>
--EXPECT--
array(2) {
  [0]=>
  array(1) {
    ["id"]=>
    NULL
  }
  [1]=>
  array(1) {
    ["id"]=>
    NULL
  }
}
