--TEST--
sqlite-oo: Simple insert/select
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
require_once('blankdb_oo.inc');
var_dump($db);

var_dump($db->query("CREATE TABLE foo(c1 date, c2 time, c3 varchar(64))"));
var_dump($db->query("INSERT INTO foo VALUES ('2002-01-02', '12:49:00', NULL)"));
$r = $db->query("SELECT * from foo");
var_dump($r);
var_dump($r->fetch());
?>
--EXPECTF--
object(SQLiteDatabase)#%d (0) {
}
object(SQLiteResult)#%d (0) {
}
object(SQLiteResult)#%d (0) {
}
object(SQLiteResult)#%d (0) {
}
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
