--TEST--
Test odbc_cursor()
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, "CREATE TABLE cursor_table (id INT, whatever TEXT)");
odbc_exec($conn, "INSERT INTO cursor_table VALUES (1, 'whatever')");
$res = odbc_exec($conn, 'SELECT * FROM cursor_table');

var_dump(odbc_cursor($res));

odbc_close($conn);
?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, "DROP TABLE cursor_table");
?>
--EXPECTF--
string(%d) "SQL_CUR%s"
