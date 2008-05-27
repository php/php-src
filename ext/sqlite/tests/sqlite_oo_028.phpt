--TEST--
sqlite-oo: sqlite_fetch_column_types
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$db->query("CREATE TABLE strings(a, b INTEGER, c VARCHAR(10), d)");
$db->query("INSERT INTO strings VALUES('1', '2', '3', 'abc')");

var_dump($db->fetchColumnTypes("strings"));
?>
--EXPECT--
array(4) {
  [u"a"]=>
  unicode(0) ""
  [u"b"]=>
  unicode(7) "INTEGER"
  [u"c"]=>
  unicode(11) "VARCHAR(10)"
  [u"d"]=>
  unicode(0) ""
}
