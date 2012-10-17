--TEST--
sqlite-oo: returned associative column names
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$db->query("CREATE TABLE foo (c1 char, c2 char, c3 char)");
$db->query("CREATE TABLE bar (c1 char, c2 char, c3 char)");
$db->query("INSERT INTO foo VALUES ('1', '2', '3')");
$db->query("INSERT INTO bar VALUES ('4', '5', '6')");
$r = $db->query("SELECT * from foo, bar", SQLITE_ASSOC);
var_dump($r->fetch());
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
