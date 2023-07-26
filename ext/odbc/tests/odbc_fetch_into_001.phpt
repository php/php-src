--TEST--
odbc_fetch_into(): Getting data from query
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE fetch_into (foo INT)');
odbc_exec($conn, 'INSERT INTO fetch_into VALUES (1)');
odbc_exec($conn, 'INSERT INTO fetch_into VALUES (2)');
odbc_exec($conn, 'INSERT INTO fetch_into VALUES (3)');

$res = odbc_exec($conn, 'SELECT * FROM fetch_into');

$arr = [];
var_dump(odbc_fetch_into($res, $arr, 1));
var_dump($arr);
$arr = [];
var_dump(odbc_fetch_into($res, $arr));
var_dump($arr);
$arr = [];
var_dump(odbc_fetch_into($res, $arr, 0));
var_dump($arr);
$arr = [];
var_dump(odbc_fetch_into($res, $arr, 4));
var_dump($arr);

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE fetch_into');
?>
--EXPECTF--
int(1)
array(1) {
  [0]=>
  string(1) "1"
}
int(1)
array(1) {
  [0]=>
  string(1) "2"
}
int(1)
array(1) {
  [0]=>
  string(1) "3"
}
bool(false)
array(0) {
}
