--TEST--
sqlite-oo: fetch column
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$data = array(
	array (0 => 'one', 1 => 'two'),
	array (0 => 'three', 1 => 'four')
	);

$db->query("CREATE TABLE strings(a VARCHAR, b VARCHAR)");

foreach ($data as $str) {
	$db->query("INSERT INTO strings VALUES('${str[0]}','${str[1]}')");
}

echo "====BUFFERED====\n";
$r = $db->query("SELECT a, b from strings");
while ($r->valid()) {
	var_dump($r->current(SQLITE_NUM));
	var_dump($r->column(0));
	var_dump($r->column(1));
	var_dump($r->column('a'));
	var_dump($r->column('b'));
	$r->next();
}
echo "====UNBUFFERED====\n";
$r = $db->unbufferedQuery("SELECT a, b from strings");
while ($r->valid()) {
	var_dump($r->column(0));
	var_dump($r->column('b'));
	var_dump($r->column(1));
	var_dump($r->column('a'));
	$r->next();
}
echo "DONE!\n";
?>
--EXPECT--
====BUFFERED====
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
}
string(3) "one"
string(3) "two"
string(3) "one"
string(3) "two"
array(2) {
  [0]=>
  string(5) "three"
  [1]=>
  string(4) "four"
}
string(5) "three"
string(4) "four"
string(5) "three"
string(4) "four"
====UNBUFFERED====
string(3) "one"
string(3) "two"
NULL
NULL
string(5) "three"
string(4) "four"
NULL
NULL
DONE!
