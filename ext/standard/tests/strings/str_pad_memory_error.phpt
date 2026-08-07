--TEST--
str_pad() throws a catchable MemoryError when the result cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    str_pad('x', PHP_INT_MAX);
} catch (MemoryError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

var_dump(str_pad('x', 5, '-'));

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
string(5) "x----"
