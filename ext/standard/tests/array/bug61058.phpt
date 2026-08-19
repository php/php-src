--TEST--
Bug #61058 (array_fill leaks if start index is PHP_INT_MAX)
--FILE--
<?php

try {
    array_fill(PHP_INT_MAX, 2, '*');
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot add element to the array as the next element is already occupied
