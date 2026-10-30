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

odbc_exec($conn, 'CREATE TABLE field_scale (foo INT, bar REAL, baz VARBINARY(50))');

$res = odbc_exec($conn, 'SELECT * FROM field_scale');
try {
    odbc_field_scale($res, 0);
} catch (ValueError $error) {
    echo $error->getMessage() . "\n";
}
var_dump(odbc_field_scale($res, 1));
var_dump(odbc_field_scale($res, 2));
var_dump(odbc_field_scale($res, 3));
var_dump(odbc_field_scale($res, 4));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE field_scale');
?>
--EXPECTF--
odbc_field_scale(): Argument #2 ($field) must be greater than 0
int(0)
int(0)
int(0)

Warning: odbc_field_scale(): Field index larger than number of fields in %s on line %d
bool(false)
