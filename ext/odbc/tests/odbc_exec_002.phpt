--TEST--
odbc_exec(): Getting data from query
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE DATABASE odbcTEST');

odbc_exec($conn, 'CREATE TABLE FOO (TEST INT)');

odbc_exec($conn, 'INSERT INTO FOO VALUES (1)');
odbc_exec($conn, 'INSERT INTO FOO VALUES (2)');

$res = odbc_exec($conn, 'SELECT * FROM FOO');

var_dump(odbc_fetch_row($res));
var_dump(odbc_result($res, 'test'));
var_dump(odbc_fetch_array($res));
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
array(1) {
  ["TEST"]=>
  string(1) "2"
}
