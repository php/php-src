--TEST--
Test abs() - argument errors
--FILE--
<?php

try {
    abs([]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: abs(): Argument #1 ($num) must be of type int|float, array given
