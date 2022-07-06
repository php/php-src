--TEST--
Bug #67465 (NULL Pointer dereference in odbc_handle_preparer)
--SKIPIF--
<?php
if (!extension_loaded('pdo_odbc')) die('skip pdo_odbc extension not available');
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->prepare("SELECT 1", [PDO::ATTR_CURSOR => PDO::CURSOR_SCROLL]);
echo "done\n";
?>
--EXPECT--
done
