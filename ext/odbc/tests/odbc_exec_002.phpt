--TEST--
odbc_exec(): Getting data from query
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE exec2 (TEST INT)');
odbc_exec($conn, 'INSERT INTO exec2 VALUES (1), (2)');

$res = odbc_exec($conn, 'SELECT * FROM exec2');

var_dump(odbc_fetch_row($res));
var_dump(odbc_result($res, 'test'));
var_dump(odbc_fetch_array($res));
?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE exec2');
?>
--EXPECT--
bool(true)
string(1) "1"
array(1) {
  ["TEST"]=>
  string(1) "2"
}
