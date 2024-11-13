--TEST--
odbc_fetch_object(): Getting data from query
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'CREATE TABLE fetch_object (foo INT)');
odbc_exec($conn, 'INSERT INTO fetch_object VALUES (1), (2)');

$res = odbc_exec($conn, 'SELECT * FROM fetch_object');

var_dump(odbc_fetch_object($res));
var_dump(odbc_fetch_object($res, 0));
var_dump(odbc_fetch_object($res, 2));
var_dump(odbc_fetch_object($res, 4));

?>
--CLEAN--
<?php
require 'config.inc';
$conn = odbc_connect($dsn, $user, $pass);
odbc_exec($conn, 'DROP TABLE fetch_object');
?>
--EXPECTF--
object(stdClass)#%d (%d) {
  ["foo"]=>
  string(1) "1"
}
object(stdClass)#%d (%d) {
  ["foo"]=>
  string(1) "2"
}
object(stdClass)#%d (%d) {
  ["foo"]=>
  string(1) "2"
}
bool(false)
