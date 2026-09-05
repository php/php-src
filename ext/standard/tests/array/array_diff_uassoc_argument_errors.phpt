--TEST--
Test array_diff_uassoc() - argument errors
--FILE--
<?php

$compare = fn($left, $right) => 0;

foreach ([
    fn() => array_diff_uassoc([], [], 'invalid'),
    fn() => array_diff_uassoc([], 1, $compare),
] as $callback) {
    try {
        $callback();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
TypeError: array_diff_uassoc(): Argument #3 must be a valid callback, function "invalid" not found or invalid function name
TypeError: array_diff_uassoc(): Argument #2 must be of type array, int given
