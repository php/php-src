--TEST--
Bug #70895 null ptr deref and segfault with crafted callable
--FILE--
<?php

try {
    array_map("%n", 0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    array_map("%n %i", 0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    array_map("%n %i aoeu %f aoeu %p", 0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
array_map(): Argument #1 ($callback) must be a valid callback, function "%n" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback, function "%n %i" not found or invalid function name
array_map(): Argument #1 ($callback) must be a valid callback, function "%n %i aoeu %f aoeu %p" not found or invalid function name
