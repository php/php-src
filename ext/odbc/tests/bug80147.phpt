--TEST--
Bug #80147 (BINARY strings may not be properly zero-terminated)
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, "CREATE TABLE bug80147 (id INT, whatever VARBINARY(50))");
odbc_exec($conn, "INSERT INTO bug80147 VALUES (1, CONVERT(VARBINARY(50), 'whatever'))");

$res = odbc_exec($conn, "SELECT * FROM bug80147");
odbc_binmode($res, ODBC_BINMODE_RETURN);
odbc_fetch_row($res);
var_dump(odbc_result($res, 'whatever'));
?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, "DROP TABLE bug80147");
?>
--EXPECT--
string(8) "whatever"
