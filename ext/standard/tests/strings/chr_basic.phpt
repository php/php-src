--TEST--
Test chr() function : basic functionality 
--FILE--
<?php

/* Prototype  : string chr  ( int $ascii  )
 * Description: Return a specific character
 * Source code: ext/standard/string.c
*/

echo "*** Testing chr() : basic functionality ***\n";

echo chr(72);
echo chr(101);
echo chr(108);
echo chr(108);
echo chr(111);
echo chr(10);
echo "World";
echo "\n";
?>
===DONE===
--EXPECTF--
*** Testing chr() : basic functionality ***
Hello
World
===DONE===
