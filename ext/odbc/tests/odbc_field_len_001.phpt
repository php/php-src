--TEST--
odbc_field_len(): Getting the length of the field
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE field_len (foo INT, bar TEXT, baz VARBINARY(50))');

$res = odbc_exec($conn, 'SELECT * FROM field_len');
try {
    odbc_field_len($res, 0);
} catch (ValueError $error) {
    echo $error->getMessage() . "\n";
}
var_dump(odbc_field_len($res, 1));
var_dump(odbc_field_len($res, 2));
var_dump(odbc_field_len($res, 3));
var_dump(odbc_field_len($res, 4));

odbc_close($conn);
?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE field_len');
?>
--EXPECTF--
odbc_field_len(): Argument #2 ($field) must be greater than 0
int(10)
int(2147483647)
int(50)

Warning: odbc_field_len(): Field index larger than number of fields in %s on line %d
bool(false)
