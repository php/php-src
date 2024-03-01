--TEST--
odbc_field_precision(): Getting the precision of the field
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE field_precision (foo INT, bar REAL, baz VARBINARY(50))');

$res = odbc_exec($conn, 'SELECT * FROM field_precision');
try {
    odbc_field_precision($res, 0);
} catch (ValueError $error) {
    echo $error->getMessage() . "\n";
}
var_dump(odbc_field_precision($res, 1));
var_dump(odbc_field_precision($res, 2));
var_dump(odbc_field_precision($res, 3));
var_dump(odbc_field_precision($res, 4));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE field_precision');
?>
--EXPECTF--
odbc_field_precision(): Argument #2 ($field) must be greater than 0
int(10)
int(7)
int(50)

Warning: odbc_field_precision(): Field index larger than number of fields in %s on line %d
bool(false)
