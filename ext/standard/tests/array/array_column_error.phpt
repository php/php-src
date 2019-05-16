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
