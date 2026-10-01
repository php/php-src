--TEST--
array_diff() memory leak with custom type checks
--FILE--
<?php

try {
    array_diff([123], 'x');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: array_diff(): Argument #2 must be of type array, string given
