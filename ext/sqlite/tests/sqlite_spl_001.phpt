--TEST--
sqlite-spl: sqlite / spl integration
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
if (!extension_loaded("spl")) print "skip: SPL is not installed"; 
?>
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

echo "====UNBUFFERED====\n";
$r = $db->unbuffered_query("SELECT a from strings", SQLITE_NUM);
var_dump(class_implements($r));
foreach($r as $row) {
	var_dump($row);
}
echo "====NO-MORE====\n";
foreach($r as $row) {
	var_dump($row);
}
echo "====DIRECT====\n";
foreach($db->unbuffered_query("SELECT a from strings", SQLITE_NUM) as $row) {
	var_dump($row);
}
echo "====BUFFERED====\n";
$r = $db->query("SELECT a from strings", SQLITE_NUM);
var_dump(class_implements($r));
foreach($r as $row) {
	var_dump($row);
}
foreach($r as $row) {
	var_dump($row);
}
echo "DONE!\n";
?>
--EXPECT--
====UNBUFFERED====
array(1) {
  ["spl_forward"]=>
  string(11) "spl_forward"
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
====NO-MORE====
====DIRECT====
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
====BUFFERED====
array(2) {
  ["spl_forward"]=>
  string(11) "spl_forward"
  ["spl_sequence"]=>
  string(12) "spl_sequence"
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
