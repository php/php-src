--TEST--
sqlite-oo: sqlite / foreach
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
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
$r = $db->unbufferedQuery("SELECT a from strings", SQLITE_NUM);
//var_dump(class_implements($r));
foreach($r as $row) {
	var_dump($row);
}
echo "====NO-MORE====\n";
foreach($r as $row) {
	var_dump($row);
}
echo "====DIRECT====\n";
foreach($db->unbufferedQuery("SELECT a from strings", SQLITE_NUM) as $row) {
	var_dump($row);
}
echo "====BUFFERED====\n";
$r = $db->query("SELECT a from strings", SQLITE_NUM);
//var_dump(class_implements($r));
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
