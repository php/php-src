--TEST--
Bug #77931: Warning for array_map mentions wrong type
--FILE--
<?php

try {
    array_map('trim', array(), 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    array_map('trim', array(), array(), true);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    array_map('trim', array(), array(), array(), null);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: array_map(): Argument #3 must be of type array, int given
TypeError: array_map(): Argument #4 must be of type array, true given
TypeError: array_map(): Argument #5 must be of type array, null given
