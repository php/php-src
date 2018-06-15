--TEST--
Test array_column() function: error conditions
--FILE--
<?php
/* Prototype:
 *  array array_column(array $input, mixed $column_key[, mixed $index_key]);
 * Description:
 *  Returns an array containing all the values from
 *  the specified "column" in a two-dimensional array.
 */

echo "*** Testing array_column() : error conditions ***\n";

echo "\n-- Testing array_column() function with Zero arguments --\n";
var_dump(array_column());

echo "\n-- Testing array_column() function with One argument --\n";
var_dump(array_column(array()));

echo "\n-- Testing array_column() function with string as first parameter --\n";
var_dump(array_column('foo', 0));

echo "\n-- Testing array_column() function with int as first parameter --\n";
var_dump(array_column(1, 'foo'));

echo "\n-- Testing array_column() column key parameter should be a string or an integer (testing bool) --\n";
var_dump(array_column(array(), true));

echo "\n-- Testing array_column() column key parameter should be a string or integer (testing array) --\n";
var_dump(array_column(array(), array()));

echo "\n-- Testing array_column() index key parameter should be a string or an integer (testing bool) --\n";
var_dump(array_column(array(), 'foo', true));

echo "\n-- Testing array_column() index key parameter should be a string or integer (testing array) --\n";
var_dump(array_column(array(), 'foo', array()));

echo "Done\n";
?>
--EXPECTF--
*** Testing array_column() : error conditions ***

-- Testing array_column() function with Zero arguments --

Warning: array_column() expects at least 2 parameters, 0 given in %s on line %d
NULL

-- Testing array_column() function with One argument --

Warning: array_column() expects at least 2 parameters, 1 given in %s on line %d
NULL

-- Testing array_column() function with string as first parameter --

Warning: array_column() expects parameter 1 to be array, string given in %s on line %d
NULL

-- Testing array_column() function with int as first parameter --

Warning: array_column() expects parameter 1 to be array, int given in %s on line %d
NULL

-- Testing array_column() column key parameter should be a string or an integer (testing bool) --

Warning: array_column(): The column key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_column() column key parameter should be a string or integer (testing array) --

Warning: array_column(): The column key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_column() index key parameter should be a string or an integer (testing bool) --

Warning: array_column(): The index key should be either a string or an integer in %s on line %d
bool(false)

-- Testing array_column() index key parameter should be a string or integer (testing array) --

Warning: array_column(): The index key should be either a string or an integer in %s on line %d
bool(false)
Done
