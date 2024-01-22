--TEST--
Bug #69975 (PHP segfaults when accessing nvarchar(max) defined columns)
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'CREATE TABLE bug69975 (ID INT, VARCHAR_COL NVARCHAR(MAX))');
odbc_exec($conn, "INSERT INTO bug69975 VALUES (1, 'foo')");

$result = odbc_exec($conn, "SELECT VARCHAR_COL FROM bug69975");
var_dump(odbc_fetch_array($result));

echo "ready";
?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE bug69975');
?>
--EXPECT--
array(1) {
  ["VARCHAR_COL"]=>
  string(3) "foo"
}
ready

