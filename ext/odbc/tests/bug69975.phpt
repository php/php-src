--TEST--
Bug #69975 (PHP segfaults when accessing nvarchar(max) defined columns)
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
@odbc_exec($conn, 'CREATE DATABASE odbcTEST');
odbc_exec($conn, 'CREATE TABLE FOO (ID INT, VARCHAR_COL NVARCHAR(MAX))');
odbc_exec($conn, "INSERT INTO FOO VALUES (1, 'foo')");

$result = odbc_exec($conn, "SELECT VARCHAR_COL FROM FOO");
var_dump(odbc_fetch_array($result));

echo "ready";
?>
--EXPECT--
array(1) {
  ["VARCHAR_COL"]=>
  string(3) "foo"
}
ready
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE FOO');
odbc_exec($conn, 'DROP DATABASE odbcTEST');
?>
