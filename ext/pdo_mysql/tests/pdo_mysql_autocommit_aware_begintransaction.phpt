--TEST--
PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS - beginTransaction() still works in gap
--DESCRIPTION--
beginTransaction() must remain fully functional. In the gap after COMMIT
(when SERVER_STATUS_IN_TRANS is cleared), beginTransaction() should succeed
and start an explicit transaction.
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
if (!defined('PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS')) {
    die('skip PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS not available');
}
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$db = MySQLPDOTest::factory();
$db->setAttribute(PDO::ATTR_AUTOCOMMIT, false);
$db->setAttribute(PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS, true);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec('DROP TABLE IF EXISTS test_autocommit_aware_bt');
$db->exec('CREATE TABLE test_autocommit_aware_bt (id INT PRIMARY KEY) ENGINE=InnoDB');

// 1. Commit to enter the gap
$db->exec('INSERT INTO test_autocommit_aware_bt VALUES (1)');
$db->commit();
var_dump($db->inTransaction()); // false — in the gap

// 2. beginTransaction() works in the gap
$db->beginTransaction();
var_dump($db->inTransaction()); // true — explicit transaction started

// 3. Operations within explicit transaction
$db->exec('INSERT INTO test_autocommit_aware_bt VALUES (2)');
$db->commit();

// 4. beginTransaction() still throws when already in a transaction
$db->beginTransaction();
try {
    $db->beginTransaction();
    echo "ERROR: should have thrown\n";
} catch (PDOException $e) {
    echo $e->getMessage() . "\n";
}
$db->rollBack();

// 5. Verify data
$stmt = $db->query('SELECT id FROM test_autocommit_aware_bt ORDER BY id');
var_dump($stmt->fetchAll(PDO::FETCH_COLUMN));

$db->exec('DROP TABLE test_autocommit_aware_bt');
?>
--CLEAN--
<?php
require __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS test_autocommit_aware_bt');
?>
--EXPECT--
bool(false)
bool(true)
There is already an active transaction
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
