--TEST--
Test round() - argument errors
--FILE--
<?php

try {
    round([]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: round(): Argument #1 ($num) must be of type int|float, array given
