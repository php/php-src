--TEST--
Test array_diff_key() - argument errors
--FILE--
<?php

try {
    array_diff_key([], 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: array_diff_key(): Argument #2 must be of type array, int given
