--TEST--
sprintf() throws a catchable MemoryError for a field width too large for the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    sprintf('%2147483000d', 1);
} catch (MemoryError $e) {
    echo 'width: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    sprintf('%-2147483000s', 'x');
} catch (MemoryError $e) {
    echo 'width (string pad): ' . $e::class . "\n";
}

// Precision is capped for floats and truncates for strings, so it never
// drives a large allocation and keeps its existing behavior.
var_dump(strlen(@sprintf('%.2147483000f', 1.5)));

var_dump(sprintf('%10d', 42));

?>
--EXPECT--
width: MemoryError: The resulting string is too large to fit in the configured memory limit
width (string pad): MemoryError
int(55)
string(10) "        42"
