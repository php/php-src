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
  [u"foo.c1"]=>
  unicode(1) "1"
  [u"foo.c2"]=>
  unicode(1) "2"
  [u"foo.c3"]=>
  unicode(1) "3"
  [u"bar.c1"]=>
  unicode(1) "4"
  [u"bar.c2"]=>
  unicode(1) "5"
  [u"bar.c3"]=>
  unicode(1) "6"
}
