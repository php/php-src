--TEST--
Test strripos() function : error conditions
--FILE--
<?php
/* Prototype  : int strripos ( string $haystack, string $needle [, int $offset] );
 * Description: Find position of last occurrence of a case-insensitive 'needle' in a 'haystack'
 * Source code: ext/standard/string.c
*/

echo "*** Testing strripos() function: error conditions ***";
echo "\n-- With Zero arguments --";
var_dump( strripos() );

echo "\n-- With less than expected number of arguments --";
var_dump( strripos("String") );

echo "\n-- With more than expected number of arguments --";
var_dump( strripos("string", "String", 1, 'extra_arg') );
?>
===DONE===
--EXPECTF--
*** Testing strripos() function: error conditions ***
-- With Zero arguments --
Warning: strripos() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

-- With less than expected number of arguments --
Warning: strripos() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

-- With more than expected number of arguments --
Warning: strripos() expects at most 3 parameters, 4 given in %s on line %d
bool(false)
===DONE===
