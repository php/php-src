--TEST--
Bug #70895 null ptr deref and segfault with crafted callable
--FILE--
<?php

try {
    array_map("%n", 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    array_map("%n %i", 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    array_map("%n %i aoeu %f aoeu %p", 0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: array_map(): Argument #1 ($callback) must be a valid callback or null, function "%n" not found or invalid function name
TypeError: array_map(): Argument #1 ($callback) must be a valid callback or null, function "%n %i" not found or invalid function name
TypeError: array_map(): Argument #1 ($callback) must be a valid callback or null, function "%n %i aoeu %f aoeu %p" not found or invalid function name
