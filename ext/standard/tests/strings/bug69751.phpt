--TEST--
Bug #69751: Change Error message of sprintf/printf for missing/typo position specifier.
--FILE--
<?php

try {
    sprintf('%$s, %2$s %1$s', "a", "b");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    sprintf('%3$s, %2$s %1$s', "a", "b");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    sprintf('%2147483648$s, %2$s %1$s', "a", "b");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
ValueError: Argument number specifier must be greater than zero and less than %d
ArgumentCountError: 4 arguments are required, 3 given
ValueError: Argument number specifier must be greater than zero and less than %d
