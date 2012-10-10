--TEST--
sqlite: sqlite_fetch_object in a loop
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

sqlite_query($db, "CREATE TABLE strings(a)");

foreach (array("one", "two", "three") as $str) {
	sqlite_query($db, "INSERT INTO strings VALUES('$str')");
}

$res = sqlite_query("SELECT * FROM strings", $db);

while (($obj = sqlite_fetch_object($res))) {
	var_dump($obj);
}

sqlite_close($db);
?>
--EXPECTF--
object(stdClass)#1 (1) {
  ["a"]=>
  string(3) "one"
}
object(stdClass)#2 (1) {
  ["a"]=>
  string(3) "two"
}
object(stdClass)#1 (1) {
  ["a"]=>
  string(5) "three"
}