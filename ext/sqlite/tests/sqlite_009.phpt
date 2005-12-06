--TEST--
sqlite: fetch all (unbuffered)
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
while ($row = sqlite_fetch_array($r, SQLITE_NUM)) {
	var_dump($row);
}
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
DONE!
