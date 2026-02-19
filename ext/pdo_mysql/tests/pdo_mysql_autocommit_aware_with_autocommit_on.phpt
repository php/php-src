--TEST--
PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS - no effect when autocommit is on
--DESCRIPTION--
When autocommit is ON (default), the attribute must have no effect.
commit() and rollBack() should throw as usual when no transaction is active.
The attribute only changes behavior when BOTH autocommit is off AND the
attribute is enabled.
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
// autocommit is ON (default), but attribute is enabled
$db->setAttribute(PDO::ATTR_AUTOCOMMIT_AWARE_TRANSACTIONS, true);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

// With autocommit on, there is no implicit transaction — commit should throw
try {
    $db->commit();
    echo "ERROR: should have thrown\n";
} catch (PDOException $e) {
    echo "commit: " . $e->getMessage() . "\n";
}

try {
    $db->rollBack();
    echo "ERROR: should have thrown\n";
} catch (PDOException $e) {
    echo "rollBack: " . $e->getMessage() . "\n";
}

// Normal transaction flow still works
$db->beginTransaction();
$db->commit();
echo "Normal transaction flow OK\n";
?>
--EXPECT--
commit: There is no active transaction
rollBack: There is no active transaction
Normal transaction flow OK
