--TEST--
Bug #67465 (NULL Pointer dereference in odbc_handle_preparer)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
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
