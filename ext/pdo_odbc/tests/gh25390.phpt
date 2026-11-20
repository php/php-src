--TEST--
GH-25390 (fetching a long text / ntext value allocates the declared column size when the driver returns SQL_NO_TOTAL)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--INI--
memory_limit=256M
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_odbc/tests/common.phpt');

/*
 * text / ntext are declared as 2 GB / 1 GB by SQL Server drivers; the value
 * must be read block by block when the driver cannot tell the total length
 * (SQL_NO_TOTAL) instead of allocating the declared size.
 */
foreach ([4073, 300000] as $n) {
    $row = $db->query("SELECT CAST(REPLICATE(CAST('A' AS varchar(max)), $n) AS text) AS v")->fetch(PDO::FETCH_ASSOC);
    var_dump(strlen($row['v']));
    $row = $db->query("SELECT CAST(REPLICATE(CAST(N'A' AS nvarchar(max)), $n) AS ntext) AS v")->fetch(PDO::FETCH_ASSOC);
    var_dump(strlen($row['v']));
}
?>
--EXPECT--
int(4073)
int(4073)
int(300000)
int(300000)
