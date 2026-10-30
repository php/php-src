--TEST--
odbc_field_scale(): Getting the scale of the field
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE field_num (foo INT, bar REAL, baz VARBINARY(50))');

$res = odbc_exec($conn, 'SELECT * FROM field_num');
var_dump(odbc_field_num($res, "foo"));
var_dump(odbc_field_num($res, "bar"));
var_dump(odbc_field_num($res, "baz"));
var_dump(odbc_field_num($res, "nonexistent"));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE field_num');
?>
--EXPECTF--
int(1)
int(2)
int(3)
bool(false)
