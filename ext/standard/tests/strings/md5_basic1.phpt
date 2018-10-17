--TEST--
Test md5() function : basic functionality
--FILE--
<?php
/* Prototype  : string md5  ( string $str  [, bool $raw_output= false  ] )
 * Description: Calculate the md5 hash of a string
 * Source code: ext/standard/md5.c
*/

echo "*** Testing md5() : basic functionality ***\n";
var_dump(md5("apple"));
?>
===DONE===
--EXPECT--
*** Testing md5() : basic functionality ***
string(32) "1f3870be274f6c49b3e31a0c6728957f"
===DONE===
