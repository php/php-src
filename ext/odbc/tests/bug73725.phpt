--TEST--
Bug #73725 Unable to retrieve value of varchar(max) type
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include __DIR__ . "/config.inc";

$conn = odbc_connect($dsn, $user, $pass);

odbc_do($conn, "CREATE TABLE bug73725(i int, txt varchar(max), k int)");

odbc_do($conn, "INSERT INTO bug73725 VALUES(101,'Any text', 33)");
odbc_do($conn, "INSERT INTO bug73725 VALUES(102,'Müsliriegel', 34)");

$rc = odbc_do($conn, "SELECT i, txt, k FROM bug73725");

$r = odbc_fetch_array($rc);
var_dump($r);

$r = odbc_fetch_array($rc);
var_dump($r);

?>
--EXPECT--
array(3) {
  ["i"]=>
  string(3) "101"
  ["txt"]=>
  string(8) "Any text"
  ["k"]=>
  string(2) "33"
}
array(3) {
  ["i"]=>
  string(3) "102"
  ["txt"]=>
  string(12) "Müsliriegel"
  ["k"]=>
  string(2) "34"
}
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'DROP TABLE bug73725');

odbc_close($conn);

?>
