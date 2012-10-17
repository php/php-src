--TEST--
sqlite: fetch all (array_query)
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

$res = sqlite_array_query("SELECT a from strings", $db, SQLITE_NUM);
var_dump($res);

$db = null;

echo "DONE!\n";
?>
--EXPECTF--
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(3) "one"
  }
  [1]=>
  array(1) {
    [0]=>
    string(3) "two"
  }
  [2]=>
  array(1) {
    [0]=>
    string(5) "three"
  }
}
DONE!
