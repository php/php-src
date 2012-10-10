--TEST--
sqlite: fetch single
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

$data = array(
	array (0 => 'one', 1 => 'two'),
	array (0 => 'three', 1 => 'four')
	);

sqlite_query("CREATE TABLE strings(a VARCHAR, b VARCHAR)", $db);

foreach ($data as $str) {
	sqlite_query("INSERT INTO strings VALUES('${str[0]}','${str[1]}')", $db);
}

echo "====BUFFERED====\n";
$r = sqlite_query("SELECT a, b from strings", $db);
while (sqlite_valid($r)) {
	var_dump(sqlite_fetch_single($r));
}
echo "====UNBUFFERED====\n";
$r = sqlite_unbuffered_query("SELECT a, b from strings", $db);
while (sqlite_valid($r)) {
	var_dump(sqlite_fetch_single($r));
}

sqlite_close($db);

echo "DONE!\n";
?>
--EXPECT--
====BUFFERED====
string(3) "one"
string(5) "three"
====UNBUFFERED====
string(3) "one"
string(5) "three"
DONE!
