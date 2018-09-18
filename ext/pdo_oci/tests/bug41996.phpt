--TEST--
PDO OCI Bug #41996 (Problem accessing Oracle ROWID)
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

$stmt = $db->prepare('SELECT rowid FROM dual');
$stmt->execute();
$row = $stmt->fetch();
var_dump(strlen($row[0]) > 0);
--EXPECT--
bool(true)
