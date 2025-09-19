--TEST--
PDO Common: Bug #63343 (Commit failure for repeated persistent connection)
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$db = PDOTest::factory();
try {
  $db->beginTransaction();
} catch (PDOException $exception) {
  die("skip not relevant for driver - does not permit transactions");
}
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

putenv("PDOTEST_ATTR=" . serialize(array(PDO::ATTR_PERSISTENT => true)));

$db1 = PDOTest::factory('PDO', false);
$db1->beginTransaction();
var_dump($db1->inTransaction());
/* db2 should assume persistent conn, including txn state */
$db2 = PDOTest::factory('PDO', false);
var_dump($db2->inTransaction());
/* destructing db1 should not rollback db2 */
$db1 = null;
var_dump($db2->inTransaction());
$db2 = null;
/* db3 should assume persistent conn, despite no remaining PDOs */
$db3 = PDOTest::factory('PDO', false);
var_dump($db3->inTransaction());
var_dump($db3->commit());

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
