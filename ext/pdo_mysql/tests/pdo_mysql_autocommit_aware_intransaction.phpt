--TEST--
PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS - inTransaction() behavior unchanged
--DESCRIPTION--
The attribute must NOT change inTransaction() semantics. It should still
reflect the actual SERVER_STATUS_IN_TRANS flag from MySQL.
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

$db->exec('DROP TABLE IF EXISTS test_autocommit_aware_it');
$db->exec('CREATE TABLE test_autocommit_aware_it (id INT) ENGINE=InnoDB');

// 1. After DDL (implicit commit clears flag) — flag not set
var_dump($db->inTransaction()); // false

// 2. After a DML statement — flag set by implicit transaction
$db->exec('INSERT INTO test_autocommit_aware_it VALUES (1)');
var_dump($db->inTransaction()); // true

// 3. After COMMIT — flag cleared
$db->commit();
var_dump($db->inTransaction()); // false — unchanged behavior

// 4. After explicit BEGIN — flag set
$db->beginTransaction();
var_dump($db->inTransaction()); // true

// 5. After explicit COMMIT — flag cleared
$db->commit();
var_dump($db->inTransaction()); // false

echo "inTransaction() behavior is unchanged\n";

$db->exec('DROP TABLE test_autocommit_aware_it');
?>
--CLEAN--
<?php
require __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS test_autocommit_aware_it');
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
inTransaction() behavior is unchanged
