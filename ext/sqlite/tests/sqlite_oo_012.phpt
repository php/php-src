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
for($i=0; $i<$r->num_fields(); $i++) {
	var_dump($r->field_name($i));
}
echo "Unbuffered\n";
$r = $db->unbuffered_query("SELECT * from strings");
for($i=0; $i<$r->num_fields(); $i++) {
	var_dump($r->field_name($i));
}
echo "DONE!\n";
?>
--EXPECT--
Buffered
string(3) "foo"
string(3) "bar"
string(3) "baz"
Unbuffered
string(3) "foo"
string(3) "bar"
string(3) "baz"
DONE!
