--TEST--
odbc_free_result(): Basic test
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
var_dump(odbc_free_result($conn));
var_dump(odbc_free_result(NULL));
var_dump(odbc_fetch_row($res));
var_dump(odbc_result($res, 'test'));

odbc_exec($conn, 'DROP TABLE FOO');

odbc_exec($conn, 'DROP DATABASE odbcTEST');

?>
--EXPECTF--
bool(true)
string(1) "1"
bool(true)

Warning: odbc_free_result(): supplied resource is not a valid ODBC result resource in %s on line %d
bool(false)

Warning: odbc_free_result() expects parameter 1 to be resource, null given in %s on line %d
NULL

Warning: odbc_fetch_row(): supplied resource is not a valid ODBC result resource in %s on line %d
bool(false)

Warning: odbc_result(): supplied resource is not a valid ODBC result resource in %s on line %d
bool(false)
