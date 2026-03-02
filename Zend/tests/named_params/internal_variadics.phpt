--TEST--
Named params on internal functions: Variadic functions that don't support extra named args
--FILE--
<?php

try {
    array_merge([1, 2], a: [3, 4]);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

try {
    array_diff_key([1, 2], [3, 4], a: [5, 6]);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $array = [1, 2];
    array_push($array, ...['values' => 3]);
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Internal function array_merge() does not accept named variadic arguments
Internal function array_diff_key() does not accept named variadic arguments
Internal function array_push() does not accept named variadic arguments
