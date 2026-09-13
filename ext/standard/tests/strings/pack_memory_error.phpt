--TEST--
pack() throws a catchable MemoryError when the output size cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    pack('x2000000000');
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(bin2hex(pack('x4')));

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
string(8) "00000000"
