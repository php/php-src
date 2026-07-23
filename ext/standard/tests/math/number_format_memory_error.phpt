--TEST--
number_format() throws a catchable MemoryError when the decimals cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    number_format(1234.5678, 2147483647);
} catch (MemoryError $e) {
    echo get_class($e) . ': ' . $e->getMessage() . "\n";
}

var_dump(number_format(1234.5678, 2));

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
string(8) "1,234.57"
