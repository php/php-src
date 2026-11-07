--TEST--
GitLab bug #14 (Use-after-free in the CSV PECL extension: all methods over-release the borrowed $delimiter / $enclosure / $eolSequence string arguments)
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

// a NON-interned (ref-counted) delimiter
$delim = str_repeat("A", 2);
Csv\buffer_to_collection("aAAb", $delim, '"', "\r\n");
var_dump($delim);

$enclosure = str_repeat("5", 2);
Csv\buffer_to_collection("aAAb", ',', $enclosure, "\r\n");
var_dump($enclosure);

$eol = str_repeat("oooo", 2);
Csv\buffer_to_collection("aAAb", ',', '"', $eol);
var_dump($eol);
?>
--EXPECT--
string(2) "AA"
string(2) "55"
string(8) "oooooooo"
