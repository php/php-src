--TEST--
Test array_search() function - error conditions
--FILE--
<?php
/*
 * Prototype  : mixed array_search ( mixed $needle, array $haystack [, bool $strict] )
 * Description: Searches haystack for needle and returns the key if it is found in the array, FALSE otherwise
 * Source Code: ext/standard/array.c
*/

echo "*** Testing error conditions of array_search() ***\n";
/* zero argument */
var_dump( array_search() );

/* unexpected no.of arguments in array_search() */
$var = array("mon", "tues", "wed", "thurs");
var_dump( array_search(1, $var, 0, "test") );
var_dump( array_search("test") );

/* unexpected second argument in array_search() */
$var="test";
var_dump( array_search("test", $var) );
var_dump( array_search(1, 123) );

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions of array_search() ***

Warning: Wrong parameter count for array_search() in %s on line %d
NULL

Warning: Wrong parameter count for array_search() in %s on line %d
NULL

Warning: Wrong parameter count for array_search() in %s on line %d
NULL

Warning: array_search(): Wrong datatype for second argument in %s on line %d
bool(false)

Warning: array_search(): Wrong datatype for second argument in %s on line %d
bool(false)
Done
