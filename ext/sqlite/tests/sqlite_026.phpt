--TEST--
sqlite: sqlite_fetch_column_types
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

sqlite_query($db, "CREATE TABLE strings(a, b INTEGER, c VARCHAR(10), d)");
sqlite_query($db, "INSERT INTO strings VALUES('1', '2', '3', 'abc')");

var_dump(sqlite_fetch_column_types($db, "strings"));

sqlite_close($db);
?>
--EXPECT--
array(4) {
  ["a"]=>
  string(0) ""
  ["b"]=>
  string(7) "INTEGER"
  ["c"]=>
  string(11) "VARCHAR(10)"
  ["d"]=>
  string(0) ""
}
