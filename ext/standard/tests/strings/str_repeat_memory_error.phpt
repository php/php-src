--TEST--
str_repeat() throws a catchable MemoryError when the result cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    str_repeat('ab', PHP_INT_MAX);
} catch (MemoryError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

try {
    str_repeat('ab', -1);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
str_repeat(): Argument #2 ($times) must be greater than or equal to 0
