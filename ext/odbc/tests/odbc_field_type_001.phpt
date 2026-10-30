--TEST--
odbc_field_type(): Getting the type of the field
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE field_type (foo INT, bar TEXT, baz VARBINARY(50))');

$res = odbc_exec($conn, 'SELECT * FROM field_type');
try {
    odbc_field_type($res, 0);
} catch (ValueError $error) {
    echo $error->getMessage() . "\n";
}
var_dump(odbc_field_type($res, 1));
var_dump(odbc_field_type($res, 2));
var_dump(odbc_field_type($res, 3));
var_dump(odbc_field_type($res, 4));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE field_type');
?>
--EXPECTF--
odbc_field_type(): Argument #2 ($field) must be greater than 0
string(3) "int"
string(4) "text"
string(9) "varbinary"

Warning: odbc_field_type(): Field index larger than number of fields in %s on line %d
bool(false)
