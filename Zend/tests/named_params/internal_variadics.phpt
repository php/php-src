--TEST--
Named params on internal functions: Variadic functions that don't support extra named args
--FILE--
<?php

try {
    array_merge([1, 2], a: [3, 4]);
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    array_diff_key([1, 2], [3, 4], a: [5, 6]);
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(array_intersect(array: [1, 2]) === [1, 2]);

try {
    array_intersect([1, 2], arrays: [2]);
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $array = [1, 2];
    array_push($array, ...['values' => 3]);
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ArgumentCountError: Internal function array_merge() does not accept named variadic arguments
ArgumentCountError: Internal function array_diff_key() does not accept named variadic arguments
bool(true)
ArgumentCountError: Internal function array_intersect() does not accept named variadic arguments
ArgumentCountError: Internal function array_push() does not accept named variadic arguments
