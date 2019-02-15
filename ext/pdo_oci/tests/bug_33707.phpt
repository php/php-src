--TEST--
PDO OCI Bug #33707 (Errors in select statements not reported)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require dirname(__FILE__).'/../../pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$rs = $db->query('select blah from a_table_that_doesnt_exist');
var_dump($rs);
var_dump($db->errorInfo());
--EXPECTF--
bool(false)
array(3) {
  [0]=>
  string(5) "HY000"
  [1]=>
  int(942)
  [2]=>
  string(%d) "OCIStmtExecute: ORA-00942: table or view does not exist
 (%s:%d)"
}
