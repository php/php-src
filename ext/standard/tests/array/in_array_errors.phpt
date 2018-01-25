--TEST--
Test in_array() function : error conditions
--FILE--
<?php
/*
 * Prototype  : bool in_array ( mixed $needle, array $haystack [, bool $strict] )
 * Description: Searches haystack for needle and returns TRUE
 *              if it is found in the array, FALSE otherwise.
 * Source Code: ext/standard/array.c
*/

echo "\n*** Testing error conditions of in_array() ***\n";
/* zero argument */
var_dump( in_array() );

/* unexpected no.of arguments in in_array() */
$var = array("mon", "tues", "wed", "thurs");
var_dump( in_array(1, $var, 0, "test") );
var_dump( in_array("test") );

/* unexpected second argument in in_array() */
$var="test";
var_dump( in_array("test", $var) );
var_dump( in_array(1, 123) );

echo "Done\n";
?>
--EXPECTF--
*** Testing error conditions of in_array() ***

Warning: in_array() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: in_array() expects at most 3 parameters, 4 given in %s on line %d
NULL

Warning: in_array() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: in_array() expects parameter 2 to be array, string given in %s on line %d
NULL

Warning: in_array() expects parameter 2 to be array, integer given in %s on line %d
NULL
Done
