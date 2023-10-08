--TEST--
odbc_fetch_row(): Getting data from query
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE fetch_row (test INT)');

odbc_exec($conn, 'INSERT INTO fetch_row VALUES (1), (2)');

$res = odbc_exec($conn, 'SELECT * FROM fetch_row');

var_dump(odbc_fetch_row($res, 0));

var_dump(odbc_fetch_row($res, null));
var_dump(odbc_result($res, 'test'));

var_dump(odbc_fetch_row($res, null));
var_dump(odbc_result($res, 'test'));

var_dump(odbc_fetch_row($res, 2));
var_dump(odbc_result($res, 'test'));

var_dump(odbc_fetch_row($res, 4));

odbc_free_result($res);
odbc_close($conn);
?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE fetch_row');
?>
--EXPECTF--
bool(false)
bool(true)
string(1) "1"
bool(true)
string(1) "2"
bool(true)
string(1) "2"
bool(false)
