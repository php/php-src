--TEST--
mb_str_split() throws a catchable MemoryError when the resulting array cannot fit in the memory limit
--EXTENSIONS--
mbstring
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$input = str_repeat('a', 40 * 1024 * 1024);

try {
    mb_str_split($input, 1);
} catch (MemoryError $e) {
    echo 'UTF-8: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    mb_str_split($input, 1, '8bit');
} catch (MemoryError $e) {
    echo '8bit: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(mb_str_split('abcd', 2));

?>
--EXPECT--
UTF-8: MemoryError: The resulting array is too large to fit in the configured memory limit
8bit: MemoryError: The resulting array is too large to fit in the configured memory limit
array(2) {
  [0]=>
  string(2) "ab"
  [1]=>
  string(2) "cd"
}
