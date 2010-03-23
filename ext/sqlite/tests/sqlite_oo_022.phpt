--TEST--
sqlite-oo: sqlite::seek
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$data = array(
	"one",
	"two",
	"three"
	);

$db->query("CREATE TABLE strings(a)");

foreach ($data as $str) {
	$db->query("INSERT INTO strings VALUES('$str')");
}

$res = $db->query("SELECT a FROM strings", SQLITE_NUM);
for ($idx = -1; $idx < 4; $idx++) {
	echo "====SEEK:$idx====\n";
	$res->seek($idx);
	var_dump($res->current());
}
echo "====AGAIN====\n";
for ($idx = -1; $idx < 4; $idx++) {
	echo "====SEEK:$idx====\n";
	$res->seek($idx);
	var_dump($res->current());
}
echo "====DONE!====\n";
?>
--EXPECTF--
====SEEK:-1====

Warning: SQLiteResult::seek(): row -1 out of range in %ssqlite_oo_022.php on line %d
array(1) {
  [0]=>
  string(3) "one"
}
====SEEK:0====
array(1) {
  [0]=>
  string(3) "one"
}
====SEEK:1====
array(1) {
  [0]=>
  string(3) "two"
}
====SEEK:2====
array(1) {
  [0]=>
  string(5) "three"
}
====SEEK:3====

Warning: SQLiteResult::seek(): row 3 out of range in %ssqlite_oo_022.php on line %d
array(1) {
  [0]=>
  string(5) "three"
}
====AGAIN====
====SEEK:-1====

Warning: SQLiteResult::seek(): row -1 out of range in %ssqlite_oo_022.php on line %d
array(1) {
  [0]=>
  string(5) "three"
}
====SEEK:0====
array(1) {
  [0]=>
  string(3) "one"
}
====SEEK:1====
array(1) {
  [0]=>
  string(3) "two"
}
====SEEK:2====
array(1) {
  [0]=>
  string(5) "three"
}
====SEEK:3====

Warning: SQLiteResult::seek(): row 3 out of range in %ssqlite_oo_022.php on line %d
array(1) {
  [0]=>
  string(5) "three"
}
====DONE!====
