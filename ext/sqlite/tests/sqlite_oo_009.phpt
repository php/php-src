--TEST--
sqlite-oo: fetch all (unbuffered)
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

$db->query("CREATE TABLE strings(a VARCHAR)");

foreach ($data as $str) {
	$db->query("INSERT INTO strings VALUES('$str')");
}

$r = $db->unbufferedQuery("SELECT a from strings");
while ($row = $r->fetch(SQLITE_NUM)) {
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
