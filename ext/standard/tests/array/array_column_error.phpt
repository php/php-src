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
try {
    var_dump(array_column(array(), true));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}


echo "\n-- Testing array_column() column key parameter should be a string or integer (testing array) --\n";
try {
    var_dump(array_column(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing array_column() index key parameter should be a string or an integer (testing bool) --\n";
try {
    var_dump(array_column(array(), 'foo', true));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing array_column() index key parameter should be a string or integer (testing array) --\n";
try {
    var_dump(array_column(array(), 'foo', array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>

DONE
--EXPECT--
*** Testing array_column() : error conditions ***

-- Testing array_column() column key parameter should be a string or an integer (testing bool) --
The column key should be either a string or an integer

-- Testing array_column() column key parameter should be a string or integer (testing array) --
The column key should be either a string or an integer

-- Testing array_column() index key parameter should be a string or an integer (testing bool) --
The index key should be either a string or an integer

-- Testing array_column() index key parameter should be a string or integer (testing array) --
The index key should be either a string or an integer

DONE
