--TEST--
getrusage() tests
--SKIPIF--
<?php if (!function_exists("getrusage")) print "skip"; ?>
--FILE--
<?php

var_dump(gettype(getrusage()));
var_dump(gettype(getrusage(1)));
var_dump(gettype(getrusage(-1)));
var_dump(getrusage(array()));


echo "Done\n";
?>
--EXPECTF--	
string(5) "array"
string(5) "array"
string(5) "array"

Warning: getrusage() expects parameter 1 to be long, array given in %s on line %d
NULL
Done
