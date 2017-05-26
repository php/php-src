--TEST--
PDO OCI Bug #44301 (Segfault when an exception is thrown on persistent connections)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require dirname(__FILE__).'/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
putenv("PDO_OCI_TEST_ATTR=" . serialize(array(PDO::ATTR_PERSISTENT => true)));
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

try {
    $stmt = $db->prepare('SELECT * FROM no_table');
    $stmt->execute();
} catch (PDOException $e) {
    print $e->getMessage();
}
$db = null;
--EXPECTF--
SQLSTATE[HY000]: General error: 942 OCIStmtExecute: ORA-00942: table or view does not exist
 (%s%epdo_oci%eoci_statement.c:%d)
