--TEST--
Bug #79106 (PDO may fetch wrong column indexes with PDO::FETCH_BOTH) - collision
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (!$dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
try {
    $db = PDOTest::factory();
} catch (PDOException $e) {
    die('skip ' . $e->getMessage());
}
if (@$db->query('SELECT 1 as "1"') === false) {
    die('skip driver does not support quoted numeric identifiers');
}
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR=' . dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$stmt = $db->query('SELECT 11111 as "1", 22222 as "2"');
var_dump($stmt->fetchAll());
?>
--EXPECT--
array(1) {
  [0]=>
  array(3) {
    [1]=>
    string(5) "11111"
    [0]=>
    string(5) "11111"
    [2]=>
    string(5) "22222"
  }
}
