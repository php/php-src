--TEST--
GH-23443 (infinite loop / 100% CPU when fetching a large nvarchar(max))
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
$db = PDOTest::test_factory('ext/pdo_odbc/tests/common.phpt');

/*
 * Likely depends on ZendMM page size + string overhead that affects long
 * column buffer size 
 */
$n = 4499;
$row = $db->query("SELECT REPLICATE(CAST(N'A' AS nvarchar(max)), $n) AS v")->fetch(PDO::FETCH_ASSOC);
echo "ok\n";
?>
--EXPECT--
ok
