--TEST--
PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS - defaults to off (BC preserved)
--DESCRIPTION--
Without explicitly enabling the attribute, the current behavior must be
preserved: commit() and rollBack() throw when there is no active transaction.
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
// NOT setting ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS — defaults to false
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec('DROP TABLE IF EXISTS test_autocommit_aware_do');
$db->exec('CREATE TABLE test_autocommit_aware_do (id INT) ENGINE=InnoDB');

// Execute a DML and commit to enter the gap
$db->exec('INSERT INTO test_autocommit_aware_do VALUES (1)');
$db->commit();

// commit() in the gap should throw (current behavior preserved)
try {
    $db->commit();
    echo "ERROR: should have thrown\n";
} catch (PDOException $e) {
    echo "commit: " . $e->getMessage() . "\n";
}

// rollBack() in the gap should throw (current behavior preserved)
try {
    $db->rollBack();
    echo "ERROR: should have thrown\n";
} catch (PDOException $e) {
    echo "rollBack: " . $e->getMessage() . "\n";
}

// Verify attribute defaults to false
var_dump($db->getAttribute(PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS));

$db->exec('DROP TABLE test_autocommit_aware_do');
?>
--CLEAN--
<?php
require __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS test_autocommit_aware_do');
?>
--EXPECT--
commit: There is no active transaction
rollBack: There is no active transaction
bool(false)
