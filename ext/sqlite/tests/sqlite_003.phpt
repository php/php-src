--TEST--
sqlite: Simple insert/select, different result represenatation
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

sqlite_query("CREATE TABLE foo(c1 date, c2 time, c3 varchar(64))", $db);
sqlite_query("INSERT INTO foo VALUES ('2002-01-02', '12:49:00', NULL)", $db);
$r = sqlite_query("SELECT * from foo", $db);
var_dump(sqlite_fetch_array($r, SQLITE_BOTH));
$r = sqlite_query("SELECT * from foo", $db);
var_dump(sqlite_fetch_array($r, SQLITE_NUM));
$r = sqlite_query("SELECT * from foo", $db);
var_dump(sqlite_fetch_array($r, SQLITE_ASSOC));
sqlite_close($db);
?>
--EXPECT--
array(6) {
  [0]=>
  string(10) "2002-01-02"
  ["c1"]=>
  string(10) "2002-01-02"
  [1]=>
  string(8) "12:49:00"
  ["c2"]=>
  string(8) "12:49:00"
  [2]=>
  NULL
  ["c3"]=>
  NULL
}
array(3) {
  [0]=>
  string(10) "2002-01-02"
  [1]=>
  string(8) "12:49:00"
  [2]=>
  NULL
}
array(3) {
  ["c1"]=>
  string(10) "2002-01-02"
  ["c2"]=>
  string(8) "12:49:00"
  ["c3"]=>
  NULL
}
