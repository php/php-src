--TEST--
sqlite: sqlite_seek
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

sqlite_query("CREATE TABLE strings(a)", $db);

foreach ($data as $str) {
	sqlite_query("INSERT INTO strings VALUES('$str')", $db);
}

$res = sqlite_query("SELECT a FROM strings", $db, SQLITE_NUM);
for ($idx = -1; $idx < 4; $idx++) {
	echo "====SEEK:$idx====\n";
	sqlite_seek($res, $idx);
	var_dump(sqlite_current($res));
}
echo "====AGAIN====\n";
for ($idx = -1; $idx < 4; $idx++) {
	echo "====SEEK:$idx====\n";
	sqlite_seek($res, $idx);
	var_dump(sqlite_current($res));
}

sqlite_close($db);

echo "====DONE!====\n";
?>
--EXPECTF--
====SEEK:-1====

Warning: sqlite_seek(): row -1 out of range in %ssqlite_022.php on line %d
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

Warning: sqlite_seek(): row 3 out of range in %ssqlite_022.php on line %d
array(1) {
  [0]=>
  string(5) "three"
}
====AGAIN====
====SEEK:-1====

Warning: sqlite_seek(): row -1 out of range in %ssqlite_022.php on line %d
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

Warning: sqlite_seek(): row 3 out of range in %ssqlite_022.php on line %d
array(1) {
  [0]=>
  string(5) "three"
}
====DONE!====
