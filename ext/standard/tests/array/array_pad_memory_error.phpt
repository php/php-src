--TEST--
array_pad() throws a catchable MemoryError when the resulting array cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    array_pad([1, 2], 50000000, 0);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    array_pad([1, 2], -50000000, 0);
} catch (MemoryError $e) {
    echo 'negative: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(array_pad([1, 2], 4, 0));

?>
--EXPECT--
MemoryError: The resulting array is too large to fit in the configured memory limit
negative: MemoryError: The resulting array is too large to fit in the configured memory limit
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(0)
  [3]=>
  int(0)
}
