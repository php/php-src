--TEST--
odbc_free_result(): Basic test
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE free_result (TEST INT NOT NULL)');

odbc_exec($conn, 'INSERT INTO free_result VALUES (1), (2)');

$res = odbc_exec($conn, 'SELECT * FROM free_result');

var_dump(odbc_fetch_row($res));
var_dump(odbc_result($res, 'test'));
var_dump(odbc_free_result($res));
try {
    var_dump(odbc_free_result($conn));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(odbc_fetch_row($res));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(odbc_result($res, 'test'));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE free_result');
?>
--EXPECT--
bool(true)
string(1) "1"
bool(true)
odbc_free_result(): supplied resource is not a valid ODBC result resource
odbc_fetch_row(): supplied resource is not a valid ODBC result resource
odbc_result(): supplied resource is not a valid ODBC result resource
