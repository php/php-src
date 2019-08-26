--TEST--
Test chunk_split() function : usage variations - unexpected large '$end' string argument variation 1
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

echo "*** Testing chunk_split() : unexpected large 'end' string argument variation 1 ***\n";

$a=str_repeat("B", 65535);
$b=1;
$c=str_repeat("B", 65535);
var_dump(chunk_split($a,$b,$c));
?>
--EXPECTF--
*** Testing chunk_split() : unexpected large 'end' string argument variation 1 ***

Fatal error: Out of memory (allocated %d) in %s on line %d
