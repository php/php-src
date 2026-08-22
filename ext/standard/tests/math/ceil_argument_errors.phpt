--TEST--
Test ceil() - argument errors
--FILE--
<?php

try {
    ceil([]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: ceil(): Argument #1 ($num) must be of type int|float, array given
