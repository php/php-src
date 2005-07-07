--TEST--
PDO Common: transactions
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo')) print 'skip';
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) print 'skip no driver';
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
/* TODO:
$db = PDOTest::factory();
try {
  $db->beginTransaction();
} catch (PDOException $e) {
  # check sqlstate to see if transactions
  # are not implemented; if so, skip
}
*/
?>
--FILE--
<?php
require getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->exec('CREATE TABLE test(id INT NOT NULL PRIMARY KEY, val VARCHAR(10))');
$db->exec("INSERT INTO test VALUES(1, 'A')"); 
$db->exec("INSERT INTO test VALUES(2, 'B')"); 
$db->exec("INSERT INTO test VALUES(3, 'C')");
$delete = $db->prepare('DELETE FROM test');

function countRows($action) {
    global $db;
	$select = $db->prepare('SELECT COUNT(*) FROM test');
	$select->execute();
    $res = $select->fetchColumn();
    return "Counted $res rows after $action.\n";
}

echo countRows('insert');

$db->beginTransaction();
$delete->execute();
echo countRows('delete');
$db->rollBack();

echo countRows('rollback');

?>
--EXPECT--
Counted 3 rows after insert.
Counted 0 rows after delete.
Counted 3 rows after rollback.
