--TEST--
Test substr_replace() function : error conditions
--FILE--
<?php
/* Prototype  : mixed substr_replace  ( mixed $string  , string $replacement  , int $start  [, int $length  ] )
 * Description: Replace text within a portion of a string
 * Source code: ext/standard/string.c
*/

/*
 * Testing substr_replace() for error conditions
*/

echo "*** Testing substr_replace() : error conditions ***\n";

$s1 = "Good morning";

echo "\n-- Testing substr_replace() function with start and length different types --\n";
var_dump(substr_replace($s1, "evening", array(5)));
var_dump(substr_replace($s1, "evening", 5, array(8)));

echo "\n-- Testing substr_replace() function with start and length with a different number of elements --\n";
var_dump(substr_replace($s1, "evening", array(5, 1), array(8)));

echo "\n-- Testing substr_replace() function with start and length as arrays but string not--\n";
var_dump(substr_replace($s1, "evening", array(5), array(8)));

?>
===DONE===
--EXPECTF--
*** Testing substr_replace() : error conditions ***

-- Testing substr_replace() function with start and length different types --

Warning: substr_replace(): 'start' and 'length' should be of same type - numerical or array  in %s on line %d
string(12) "Good morning"

Warning: substr_replace(): 'start' and 'length' should be of same type - numerical or array  in %s on line %d
string(12) "Good morning"

-- Testing substr_replace() function with start and length with a different number of elements --

Warning: substr_replace(): 'start' and 'length' should have the same number of elements in %s on line %d
string(12) "Good morning"

-- Testing substr_replace() function with start and length as arrays but string not--

Warning: substr_replace(): Functionality of 'start' and 'length' as arrays is not implemented in %s on line %d
string(12) "Good morning"
===DONE===
