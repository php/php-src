--TEST--
str_split() throws a catchable MemoryError when the resulting array cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$input = str_repeat('a', 40 * 1024 * 1024);

try {
    str_split($input, 1);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(str_split('abcd', 2));

?>
--EXPECT--
MemoryError: The resulting array is too large to fit in the configured memory limit
array(2) {
  [0]=>
  string(2) "ab"
  [1]=>
  string(2) "cd"
}
