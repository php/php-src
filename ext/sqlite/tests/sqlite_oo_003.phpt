--TEST--
sqlite-oo: Simple insert/select, different result representation
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$db->query("CREATE TABLE foo(c1 date, c2 time, c3 varchar(64))");
$db->query("INSERT INTO foo VALUES ('2002-01-02', '12:49:00', NULL)");
$r = $db->query("SELECT * from foo");
var_dump($r->fetch(SQLITE_BOTH));
$r = $db->query("SELECT * from foo");
var_dump($r->fetch(SQLITE_NUM));
$r = $db->query("SELECT * from foo");
var_dump($r->fetch(SQLITE_ASSOC));
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
