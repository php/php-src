--TEST--
sqlite-oo: fetch single
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
	var_dump($r->fetchSingle());
}
echo "====UNBUFFERED====\n";
$r = $db->unbufferedQuery("SELECT a, b from strings");
while ($r->valid()) {
	var_dump($r->fetchSingle());
}
echo "DONE!\n";
?>
--EXPECT--
====BUFFERED====
string(3) "one"
string(5) "three"
====UNBUFFERED====
string(3) "one"
string(5) "three"
DONE!
