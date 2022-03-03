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

odbc_exec($conn, 'CREATE DATABASE odbcTEST');

odbc_exec($conn, 'CREATE TABLE FOO (TEST INT NOT NULL)');
odbc_exec($conn, 'ALTER TABLE FOO ADD PRIMARY KEY (TEST)');

odbc_exec($conn, 'INSERT INTO FOO VALUES (1)');
odbc_exec($conn, 'INSERT INTO FOO VALUES (2)');

$res = odbc_exec($conn, 'SELECT * FROM FOO');

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
odbc_exec($conn, 'DROP TABLE FOO');
odbc_exec($conn, 'DROP DATABASE odbcTEST');
?>
--EXPECT--
bool(true)
string(1) "1"
bool(true)
odbc_free_result(): supplied resource is not a valid ODBC result resource
odbc_fetch_row(): supplied resource is not a valid ODBC result resource
odbc_result(): supplied resource is not a valid ODBC result resource
