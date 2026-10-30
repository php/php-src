--TEST--
odbc_num_rows(): Getting the number of rows
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE num_rows (test INT)');
odbc_exec($conn, 'INSERT INTO num_rows VALUES (1), (2), (3)');

$res = odbc_exec($conn, 'SELECT * FROM num_rows');
var_dump(odbc_num_rows($res));
odbc_free_result($res);

$res = odbc_exec($conn, 'SELECT * FROM num_rows WHERE test = 4');
var_dump(odbc_num_rows($res));
odbc_free_result($res);

odbc_close($conn);
?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE num_rows');
?>
--EXPECT--
int(3)
int(0)
