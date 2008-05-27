--TEST--
sqlite-oo: read field names
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$db->query("CREATE TABLE strings(foo VARCHAR, bar VARCHAR, baz VARCHAR)");

echo "Buffered\n";
$r = $db->query("SELECT * from strings");
for($i=0; $i<$r->numFields(); $i++) {
	var_dump($r->fieldName($i));
}
echo "Unbuffered\n";
$r = $db->unbufferedQuery("SELECT * from strings");
for($i=0; $i<$r->numFields(); $i++) {
	var_dump($r->fieldName($i));
}
echo "DONE!\n";
?>
--EXPECT--
Buffered
unicode(3) "foo"
unicode(3) "bar"
unicode(3) "baz"
Unbuffered
unicode(3) "foo"
unicode(3) "bar"
unicode(3) "baz"
DONE!
