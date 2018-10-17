--TEST--
Test escapeshellarg() function: basic test with UTF-8 strings
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

var_dump(escapeshellarg("テストマルチバイ'ト・パス"));
var_dump(escapeshellarg("測試多字\'節路徑"));
var_dump(escapeshellarg("%füße"));
var_dump(escapeshellarg("!шницель"));

echo "Done\n";
?>
--EXPECT--
Simple testcase for escapeshellarg() function
string(39) ""テストマルチバイ'ト・パス""
string(25) ""測試多字\'節路徑""
string(9) "" füße""
string(17) "" шницель""
Done
