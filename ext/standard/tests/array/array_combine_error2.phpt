--TEST--
Test array_combine() function : error conditions - empty array
--FILE--
<?php
/* Prototype  : array array_combine(array $keys, array $values)
 * Description: Creates an array by using the elements of the first parameter as keys
 *              and the elements of the second as the corresponding values
 * Source code: ext/standard/array.c
*/

echo "*** Testing array_combine() : error conditions specific to array_combine() ***\n";

// Testing array_combine by passing empty arrays to $keys and $values arguments
echo "\n-- Testing array_combine() function with empty arrays --\n";
var_dump( array_combine(array(), array()) );

// Testing array_combine by passing empty array to $keys
echo "\n-- Testing array_combine() function with empty array for \$keys argument --\n";
var_dump( array_combine(array(), array(1, 2)) );

// Testing array_combine by passing empty array to $values
echo "\n-- Testing array_combine() function with empty array for \$values argument --\n";
var_dump( array_combine(array(1, 2), array()) );

// Testing array_combine with arrays having unequal number of elements
echo "\n-- Testing array_combine() function by passing array with unequal number of elements --\n";
var_dump( array_combine(array(1, 2), array(1, 2, 3)) );

echo "Done";
?>
--EXPECTF--
*** Testing array_combine() : error conditions specific to array_combine() ***

-- Testing array_combine() function with empty arrays --
array(0) {
}

-- Testing array_combine() function with empty array for $keys argument --

Warning: array_combine(): Both parameters should have an equal number of elements in %s on line %d
bool(false)

-- Testing array_combine() function with empty array for $values argument --

Warning: array_combine(): Both parameters should have an equal number of elements in %s on line %d
bool(false)

-- Testing array_combine() function by passing array with unequal number of elements --

Warning: array_combine(): Both parameters should have an equal number of elements in %s on line %d
bool(false)
Done
