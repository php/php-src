--TEST--
Test array_merge() - argument errors
--FILE--
<?php

try {
    array_merge([], 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: array_merge(): Argument #2 must be of type array, int given
