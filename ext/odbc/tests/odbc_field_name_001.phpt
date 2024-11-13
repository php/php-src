--TEST--
odbc_field_name(): Getting the name of the field
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE field_name (foo INT, bar INT, baz INT)');

$res = odbc_exec($conn, 'SELECT * FROM field_name');
try {
    odbc_field_name($res, 0);
} catch (ValueError $error) {
    echo $error->getMessage() . "\n";
}
var_dump(odbc_field_name($res, 1));
var_dump(odbc_field_name($res, 2));
var_dump(odbc_field_name($res, 3));
var_dump(odbc_field_name($res, 4));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE field_name');
?>
--EXPECTF--
odbc_field_name(): Argument #2 ($field) must be greater than 0
string(3) "foo"
string(3) "bar"
string(3) "baz"

Warning: odbc_field_name(): Field index larger than number of fields in %s on line %d
bool(false)
