--TEST--
Test floor() - argument errors
--FILE--
<?php

try {
    floor([]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: floor(): Argument #1 ($num) must be of type int|float, array given
