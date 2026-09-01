--TEST--
chunk_split() throws a catchable MemoryError when the result cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$input = str_repeat('a', 1000);
$end = str_repeat('-', 1024 * 1024);

try {
    chunk_split($input, 1, $end);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(chunk_split('abcd', 2, '|'));

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
string(6) "ab|cd|"
