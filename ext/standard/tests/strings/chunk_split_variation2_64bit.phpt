--TEST--
Test chunk_split() function : usage variations - unexpected large '$end' string argument variation 2
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

echo "*** Testing chunk_split() : unexpected large 'end' string argument variation 2 ***\n";

$a=str_repeat("B", 65537);
$b=1;
$c=str_repeat("B", 65537);
var_dump(chunk_split($a,$b,$c));
?>
--EXPECTF--
*** Testing chunk_split() : unexpected large 'end' string argument variation 2 ***

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
