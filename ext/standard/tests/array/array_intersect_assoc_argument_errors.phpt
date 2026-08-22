--TEST--
Test array_intersect_assoc() - argument errors
--FILE--
<?php

try {
    array_intersect_assoc([], 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: array_intersect_assoc(): Argument #2 must be of type array, int given
