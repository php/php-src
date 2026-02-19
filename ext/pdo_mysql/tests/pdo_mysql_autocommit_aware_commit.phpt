--TEST--
PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS - commit() in gap does not throw
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

$db->exec('DROP TABLE IF EXISTS test_autocommit_aware');
$db->exec('CREATE TABLE test_autocommit_aware (id INT PRIMARY KEY) ENGINE=InnoDB');

// 1. Normal commit works
$db->exec('INSERT INTO test_autocommit_aware VALUES (1)');
var_dump($db->inTransaction()); // true — SERVER_STATUS_IN_TRANS set
$result = $db->commit();
var_dump($result); // true
var_dump($db->inTransaction()); // false — flag cleared by COMMIT

// 2. Second commit in the gap: should return true silently (no-op)
$result = $db->commit();
var_dump($result); // true — no exception thrown

// 3. Subsequent operations still work
$db->exec('INSERT INTO test_autocommit_aware VALUES (2)');
var_dump($db->inTransaction()); // true
$db->commit();

// 4. Verify both rows were persisted
$stmt = $db->query('SELECT id FROM test_autocommit_aware ORDER BY id');
$rows = $stmt->fetchAll(PDO::FETCH_COLUMN);
var_dump($rows);

$db->exec('DROP TABLE test_autocommit_aware');
?>
--CLEAN--
<?php
require __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS test_autocommit_aware');
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
