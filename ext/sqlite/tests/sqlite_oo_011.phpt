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
var_dump($r->fetch_array());
?>
--EXPECT--
array(3) {
  ["c1"]=>
  string(1) "4"
  ["c2"]=>
  string(1) "5"
  ["c3"]=>
  string(1) "6"
}
