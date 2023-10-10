--TEST--
PDO Common: transactions
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
} catch (PDOException $e) {
  die('skip no working transactions: ' . $e->getMessage());
}

if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'mysql') {
    require_once(__DIR__ . DIRECTORY_SEPARATOR . '../../pdo_mysql/tests/mysql_pdo_test.inc');
    if (false === MySQLPDOTest::detect_transactional_mysql_engine($db)) {
        die('skip your mysql configuration does not support working transactions');
    }
}
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'mysql') {
    require_once(__DIR__ . DIRECTORY_SEPARATOR . '../../pdo_mysql/tests/mysql_pdo_test.inc');
    $suf = ' ENGINE=' . MySQLPDOTest::detect_transactional_mysql_engine($db);
} else {
    $suf = '';
}

$db->exec('CREATE TABLE test017(id INT NOT NULL PRIMARY KEY, val VARCHAR(10))'.$suf);
$db->exec("INSERT INTO test017 VALUES(1, 'A')");
$db->exec("INSERT INTO test017 VALUES(2, 'B')");
$db->exec("INSERT INTO test017 VALUES(3, 'C')");
$delete = $db->prepare('DELETE FROM test017');

function countRows($action) {
    global $db;
    $select = $db->prepare('SELECT COUNT(*) FROM test017');
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

$db->beginTransaction();
$delete->execute();
echo countRows('delete');
$db->commit();

echo countRows('commit');

?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->exec("DROP TABLE test017");
?>
--EXPECT--
Counted 3 rows after insert.
Counted 0 rows after delete.
Counted 3 rows after rollback.
Counted 0 rows after delete.
Counted 0 rows after commit.
