--TEST--
Test escapeshellarg() function: basic test
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == "WIN" )
  die("skip.. Do not run on Windows");
?>
--FILE--
<?php
echo "Simple testcase for escapeshellarg() function\n";

var_dump(escapeshellarg("Mr O'Neil"));
var_dump(escapeshellarg("Mr O\'Neil"));
var_dump(escapeshellarg("%FILENAME"));
var_dump(escapeshellarg(""));

echo "Done\n";
?>
--EXPECT--
Simple testcase for escapeshellarg() function
string(14) "'Mr O'\''Neil'"
string(15) "'Mr O\'\''Neil'"
string(11) "'%FILENAME'"
string(2) "''"
Done
