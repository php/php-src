--TEST--
Test strrpos() function : error conditions 
--FILE--
<?php
/* Prototype  : int strrpos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of 'needle' in 'haystack'.
 * Source code: ext/standard/string.c
*/

echo "*** Testing strrpos() function: error conditions ***";
echo "\n-- With Zero arguments --";
var_dump( strrpos() );

echo "\n-- With less than expected number of arguments --";
var_dump( strrpos("String") );

echo "\n-- With more than expected number of arguments --";
var_dump( strrpos("string", "String", 1, 'extra_arg') );
echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrpos() function: error conditions ***
-- With Zero arguments --
Warning: strrpos() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

-- With less than expected number of arguments --
Warning: strrpos() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

-- With more than expected number of arguments --
Warning: strrpos() expects at most 3 parameters, 4 given in %s on line %d
bool(false)
*** Done ***
