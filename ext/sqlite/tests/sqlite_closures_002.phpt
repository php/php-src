--TEST--
sqlite: regular functions with closures
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

$data = array(
	array("one", "uno"),
	array("two", "dos"),
	array("three", "tres"),
	);

sqlite_query("CREATE TABLE strings(a,b)", $db);

foreach ($data as $row) {
	sqlite_query("INSERT INTO strings VALUES('" . sqlite_escape_string($row[0]) . "','" . sqlite_escape_string($row[1]) . "')", $db);
}

sqlite_create_function($db, "implode", function () {
	$args = func_get_args();
	$sep = array_shift($args);
	return implode($sep, $args);
});

$r = sqlite_query("SELECT implode('-', a, b) from strings", $db);
while ($row = sqlite_fetch_array($r, SQLITE_NUM)) {
	var_dump($row);
}

sqlite_close($db);

echo "DONE!\n";
?>
--EXPECT--
array(1) {
  [0]=>
  string(7) "one-uno"
}
array(1) {
  [0]=>
  string(7) "two-dos"
}
array(1) {
  [0]=>
  string(10) "three-tres"
}
DONE!
