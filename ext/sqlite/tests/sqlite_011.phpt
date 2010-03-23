--TEST--
sqlite: returned associative column names
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

sqlite_query("CREATE TABLE foo (c1 char, c2 char, c3 char)", $db);
sqlite_query("CREATE TABLE bar (c1 char, c2 char, c3 char)", $db);
sqlite_query("INSERT INTO foo VALUES ('1', '2', '3')", $db);
sqlite_query("INSERT INTO bar VALUES ('4', '5', '6')", $db);
$r = sqlite_query("SELECT * from foo, bar", $db, SQLITE_ASSOC);
var_dump(sqlite_fetch_array($r));
sqlite_close($db);
?>
--EXPECT--
array(6) {
  ["foo.c1"]=>
  string(1) "1"
  ["foo.c2"]=>
  string(1) "2"
  ["foo.c3"]=>
  string(1) "3"
  ["bar.c1"]=>
  string(1) "4"
  ["bar.c2"]=>
  string(1) "5"
  ["bar.c3"]=>
  string(1) "6"
}
