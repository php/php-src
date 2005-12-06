--TEST--
sqlite: binary encoding
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

$strings = array(
	"hello",
	"hello\x01o",
	"\x01hello there",
	"hello\x00there",
	""
);

sqlite_query("CREATE TABLE strings(a)", $db);

foreach ($strings as $str) {
	sqlite_query("INSERT INTO strings VALUES('" . sqlite_escape_string($str) . "')", $db);
}

$i = 0;
$r = sqlite_query("SELECT * from strings", $db);
while ($row = sqlite_fetch_array($r, SQLITE_NUM)) {
	if ($row[0] !== $strings[$i]) {
		echo "FAIL!\n";
		var_dump($row[0]);
		var_dump($strings[$i]);
	} else {
		echo "OK!\n";
	}
	$i++;
}
echo "DONE!\n";
?>
--EXPECT--
OK!
OK!
OK!
OK!
OK!
DONE!
