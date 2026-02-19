--TEST--
PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS - rollBack() in gap does not throw
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

$db->exec('DROP TABLE IF EXISTS test_autocommit_aware_rb');
$db->exec('CREATE TABLE test_autocommit_aware_rb (id INT PRIMARY KEY) ENGINE=InnoDB');

// 1. Normal rollback works
$db->exec('INSERT INTO test_autocommit_aware_rb VALUES (1)');
var_dump($db->inTransaction()); // true
$db->rollBack();
var_dump($db->inTransaction()); // false

// 2. Second rollBack in the gap: should return true silently (no-op)
$result = $db->rollBack();
var_dump($result); // true — no exception

// 3. Verify the insert was rolled back
$stmt = $db->query('SELECT COUNT(*) FROM test_autocommit_aware_rb');
var_dump($stmt->fetchColumn()); // 0

// 4. Subsequent operations still work after gap rollback
$db->exec('INSERT INTO test_autocommit_aware_rb VALUES (2)');
$db->commit();
$stmt = $db->query('SELECT id FROM test_autocommit_aware_rb');
var_dump($stmt->fetchAll(PDO::FETCH_COLUMN));

$db->exec('DROP TABLE test_autocommit_aware_rb');
?>
--CLEAN--
<?php
require __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS test_autocommit_aware_rb');
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
int(0)
array(1) {
  [0]=>
  int(2)
}
