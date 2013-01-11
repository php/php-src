--TEST--
Test array_pluck() function: error conditions
--FILE--
<?php
/* Prototype:
 *  array array_pluck(array $input, mixed $column_key[, mixed $index_key]);
 * Description:
 *  Returns an array containing all the values from
 *  the specified "column" in a two-dimensional array.
 */

echo "*** Testing array_pluck() : error conditions ***\n";

echo "\n-- Testing array_pluck() function with Zero arguments --\n";
var_dump(array_pluck());

echo "\n-- Testing array_pluck() function with One argument --\n";
var_dump(array_pluck(array()));

echo "\n-- Testing array_pluck() function with string as first parameter --\n";
var_dump(array_pluck('foo', 0));

echo "\n-- Testing array_pluck() function with int as first parameter --\n";
var_dump(array_pluck(1, 'foo'));

echo "\n-- Testing array_pluck() column key parameter should be a string or an integer (testing bool) --\n";
var_dump(array_pluck(array(), true));

echo "\n-- Testing array_pluck() column key parameter should be a string or integer (testing float) --\n";
var_dump(array_pluck(array(), 2.3));

echo "\n-- Testing array_pluck() column key parameter should be a string or integer (testing array) --\n";
var_dump(array_pluck(array(), array()));

echo "\n-- Testing array_pluck() index key parameter should be a string or an integer (testing bool) --\n";
var_dump(array_pluck(array(), 'foo', true));

echo "\n-- Testing array_pluck() index key parameter should be a string or integer (testing float) --\n";
var_dump(array_pluck(array(), 'foo', 2.3));

echo "\n-- Testing array_pluck() index key parameter should be a string or integer (testing array) --\n";
var_dump(array_pluck(array(), 'foo', array()));

echo "Done\n";
?>
--EXPECTF--
*** Testing array_pluck() : error conditions ***

-- Testing array_pluck() function with Zero arguments --

Warning: array_pluck() expects at least 2 parameters, 0 given in %s on line %d
NULL

-- Testing array_pluck() function with One argument --

Warning: array_pluck() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing array_pluck() function with string as first parameter --

Warning: array_pluck() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Testing array_pluck() function with int as first parameter --

Warning: array_pluck() expects parameter 1 to be array, integer given in %s on line %d
NULL

-- Testing array_pluck() column key parameter should be a string or an integer (testing bool) --

Warning: array_pluck(): The column key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_pluck() column key parameter should be a string or integer (testing float) --

Warning: array_pluck(): The column key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_pluck() column key parameter should be a string or integer (testing array) --

Warning: array_pluck(): The column key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_pluck() index key parameter should be a string or an integer (testing bool) --

Warning: array_pluck(): The index key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_pluck() index key parameter should be a string or integer (testing float) --

Warning: array_pluck(): The index key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_pluck() index key parameter should be a string or integer (testing array) --

Warning: array_pluck(): The index key should be either a string or an integer in %s on line %d
bool(false)
Done
