--TEST--
sqlite: read field names
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

sqlite_query("CREATE TABLE strings(foo VARCHAR, bar VARCHAR, baz VARCHAR)", $db);

echo "Buffered\n";
$r = sqlite_query("SELECT * from strings", $db);
for($i=0; $i<sqlite_num_fields($r); $i++) {
	var_dump(sqlite_field_name($r, $i));
}
echo "Unbuffered\n";
$r = sqlite_unbuffered_query("SELECT * from strings", $db);
for($i=0; $i<sqlite_num_fields($r); $i++) {
	var_dump(sqlite_field_name($r, $i));
}

sqlite_close($db);

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
