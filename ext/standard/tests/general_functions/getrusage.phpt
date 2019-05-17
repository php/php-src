--TEST--
getrusage() tests
--SKIPIF--
<?php if (!function_exists("getrusage")) print "skip"; ?>
--FILE--
<?php

var_dump(gettype(getrusage()));
var_dump(gettype(getrusage(1)));
var_dump(gettype(getrusage(-1)));


echo "Done\n";
?>
--EXPECT--
string(5) "array"
string(5) "array"
string(5) "array"
Done
