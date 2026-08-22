--TEST--
Test array_diff() - argument errors
--FILE--
<?php

try {
    array_diff([], 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: array_diff(): Argument #2 must be of type array, int given
