--TEST--
odbc_fetch_array(): Getting data from query
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE fetch_array (foo INT)');
odbc_exec($conn, 'INSERT INTO fetch_array VALUES (1), (2)');

$res = odbc_exec($conn, 'SELECT * FROM fetch_array');

var_dump(odbc_fetch_array($res));
var_dump(odbc_fetch_array($res, 0));
var_dump(odbc_fetch_array($res, 2));
var_dump(odbc_fetch_array($res, 4));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE fetch_array');
?>
--EXPECTF--
array(1) {
  ["foo"]=>
  string(1) "1"
}
array(1) {
  ["foo"]=>
  string(1) "2"
}
array(1) {
  ["foo"]=>
  string(1) "2"
}
bool(false)
