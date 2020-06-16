--TEST--
Test array_column(): Index argument with various types in strict type mode
--FILE--
<?php
declare(strict_types=1);
/* Prototype:
 *  array array_column(array $input, mixed $column_key[, mixed $index_key]);
 * Description:
 *  Returns an array containing all the values from
 *  the specified "column" in a two-dimensional array.
 */

echo "\n-- Testing array_column() column key parameter should be a string or an integer (testing bool) --\n";
try {
    var_dump(array_column([['php7', 'foo'], ['php8', 'bar']], false));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump(array_column([['php7', 'foo'], ['php8', 'bar']], true));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing array_column() column key parameter should be a string or integer (testing array) --\n";
try {
    var_dump(array_column([['php7', 'foo'], ['php8', 'bar']], array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing array_column() index key parameter should be a string or an integer (testing bool) --\n";
try {
    var_dump(array_column([['php' => 7, 'foo'], ['php' => 8, 'bar']], 'php', false));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
try {
    var_dump(array_column([['php' => 7, 'foo'], ['php' => 8, 'bar']], 'php', true));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

echo "\n-- Testing array_column() index key parameter should be a string or integer (testing array) --\n";
try {
    var_dump(array_column([['php' => 7, 'foo'], ['php' => 8, 'bar']], 'php', array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>

DONE
--EXPECT--
-- Testing array_column() column key parameter should be a string or an integer (testing bool) --
array_column(): Argument #2 ($column_key) must be of type string|int|null, bool given
array_column(): Argument #2 ($column_key) must be of type string|int|null, bool given

-- Testing array_column() column key parameter should be a string or integer (testing array) --
array_column(): Argument #2 ($column_key) must be of type string|int|null, array given

-- Testing array_column() index key parameter should be a string or an integer (testing bool) --
array_column(): Argument #3 ($index_key) must be of type string|int|null, bool given
array_column(): Argument #3 ($index_key) must be of type string|int|null, bool given

-- Testing array_column() index key parameter should be a string or integer (testing array) --
array_column(): Argument #3 ($index_key) must be of type string|int|null, array given

DONE
