--TEST--
sqlite: fetch all (iterator)
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

$data = array(
	"one",
	"two",
	"three"
	);

sqlite_query("CREATE TABLE strings(a VARCHAR)", $db);

foreach ($data as $str) {
	sqlite_query("INSERT INTO strings VALUES('$str')", $db);
}

$r = sqlite_unbuffered_query("SELECT a from strings", $db);
while (sqlite_valid($r)) {
	var_dump(sqlite_current($r, SQLITE_NUM));
	sqlite_next($r);
}
$r = sqlite_query("SELECT a from strings", $db);
while (sqlite_valid($r)) {
	var_dump(sqlite_current($r, SQLITE_NUM));
	sqlite_next($r);
}
sqlite_rewind($r);
while (sqlite_valid($r)) {
	var_dump(sqlite_current($r, SQLITE_NUM));
	sqlite_next($r);
}

sqlite_close($db);

echo "DONE!\n";
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "one"
}
array(1) {
  [0]=>
  string(3) "two"
}
array(1) {
  [0]=>
  string(5) "three"
}
array(1) {
  [0]=>
  string(3) "one"
}
array(1) {
  [0]=>
  string(3) "two"
}
array(1) {
  [0]=>
  string(5) "three"
}
array(1) {
  [0]=>
  string(3) "one"
}
array(1) {
  [0]=>
  string(3) "two"
}
array(1) {
  [0]=>
  string(5) "three"
}
DONE!
