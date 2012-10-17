--TEST--
sqlite: aggregate functions with closures
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
	sqlite_query("INSERT INTO strings VALUES('" . sqlite_escape_string($str) . "')", $db);
}

function cat_step(&$context, $string)
{
	$context .= $string;
}

function cat_fin(&$context)
{
	return $context;
}

sqlite_create_aggregate($db, "cat", function (&$context, $string) {
	$context .= $string;
}, function (&$context) {
	return $context;
});

$r = sqlite_query("SELECT cat(a) from strings", $db);
while ($row = sqlite_fetch_array($r, SQLITE_NUM)) {
	var_dump($row);
}

sqlite_close($db);

echo "DONE!\n";
?>
--EXPECT--
array(1) {
  [0]=>
  string(11) "onetwothree"
}
DONE!
