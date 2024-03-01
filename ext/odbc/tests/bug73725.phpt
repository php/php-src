--TEST--
Bug #73725 Unable to retrieve value of varchar(max) type
--EXTENSIONS--
odbc
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include __DIR__ . "/config.inc";

$conn = odbc_connect($dsn, $user, $pass);

odbc_do($conn, "CREATE TABLE bug73725(i int, txt varchar(max), k int)");

odbc_do($conn, "INSERT INTO bug73725 VALUES(101,'Any text', 33), (102,'Lorem ipsum dolor', 34)");

$rc = odbc_do($conn, "SELECT i, txt, k FROM bug73725");

$r = odbc_fetch_array($rc);
var_dump($r);

$r = odbc_fetch_array($rc);
var_dump($r);

?>
--CLEAN--
<?php
include 'config.inc';

$conn = odbc_connect($dsn, $user, $pass);

odbc_exec($conn, 'DROP TABLE bug73725');

odbc_close($conn);

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
  string(17) "Lorem ipsum dolor"
  ["k"]=>
  string(2) "34"
}
