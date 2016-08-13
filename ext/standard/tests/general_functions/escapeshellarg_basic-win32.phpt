--TEST--
Test escapeshellarg() function: basic test
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) != "WIN" )
  die("skip.. only for Windows");
?>
--FILE--
<?php
/* Prototype  : string escapeshellarg  ( string $arg  )
 * Description: Escape a string to be used as a shell argument.
 * Source code: ext/standard/exec.c
 * Alias to functions: 
 */

echo "Simple testcase for escapeshellarg() function\n";

var_dump(escapeshellarg("Mr O'Neil"));
var_dump(escapeshellarg("Mr O\'Neil"));
var_dump(escapeshellarg("%FILENAME"));
var_dump(escapeshellarg("!FILENAME"));
var_dump(escapeshellarg(""));

echo "Done\n";
?>
--EXPECT--
Simple testcase for escapeshellarg() function
string(11) ""Mr O'Neil""
string(12) ""Mr O\'Neil""
string(11) "" FILENAME""
string(11) "" FILENAME""
string(2) """"
Done