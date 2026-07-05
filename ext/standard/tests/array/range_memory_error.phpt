--TEST--
range() throws a catchable MemoryError when the resulting array cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    range(0, 50000000);
} catch (MemoryError $e) {
    echo 'int: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    range(0.5, 50000000.5);
} catch (MemoryError $e) {
    echo 'float: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(range(1, 3));

?>
--EXPECT--
int: MemoryError: The resulting array is too large to fit in the configured memory limit
float: MemoryError: The resulting array is too large to fit in the configured memory limit
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
