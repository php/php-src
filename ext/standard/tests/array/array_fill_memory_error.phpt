--TEST--
array_fill() throws a catchable MemoryError when the resulting array cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    array_fill(0, 50000000, 'x');
} catch (MemoryError $e) {
    echo 'packed: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    array_fill(-5, 50000000, 'x');
} catch (MemoryError $e) {
    echo 'hash: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(array_fill(0, 3, 'x'));

?>
--EXPECT--
packed: MemoryError: The resulting array is too large to fit in the configured memory limit
hash: MemoryError: The resulting array is too large to fit in the configured memory limit
array(3) {
  [0]=>
  string(1) "x"
  [1]=>
  string(1) "x"
  [2]=>
  string(1) "x"
}
