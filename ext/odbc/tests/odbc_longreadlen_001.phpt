--TEST--
Test odbc_longreadlen()
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, "CREATE TABLE longreadlen (id INT, whatever VARBINARY(50))");
odbc_exec($conn, "INSERT INTO longreadlen VALUES (1, CONVERT(VARBINARY(50), 'whatever'))");

$res = odbc_exec($conn, "SELECT * FROM longreadlen");
odbc_longreadlen($res, 4);
odbc_fetch_row($res);
var_dump(odbc_result($res, 'whatever'));

odbc_close($conn);
?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, "DROP TABLE longreadlen");
?>
--EXPECT--
string(4) "what"
