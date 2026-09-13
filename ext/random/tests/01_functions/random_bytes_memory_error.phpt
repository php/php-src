--TEST--
random_bytes() throws a catchable MemoryError when the requested size cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    random_bytes(PHP_INT_MAX);
} catch (MemoryError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

try {
    random_bytes(0);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
random_bytes(): Argument #1 ($length) must be greater than 0
