--TEST--
GMP string rendering throws a catchable MemoryError when the result cannot fit in the memory limit
--EXTENSIONS--
gmp
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$huge = gmp_pow(2, 2000000000);

try {
    gmp_strval($huge);
} catch (MemoryError $e) {
    echo 'gmp_strval: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    (string) $huge;
} catch (MemoryError $e) {
    echo 'cast: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    gmp_export($huge);
} catch (MemoryError $e) {
    echo 'gmp_export: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(gmp_strval(gmp_init(255)));

?>
--EXPECT--
gmp_strval: MemoryError: The resulting string is too large to fit in the configured memory limit
cast: MemoryError: The resulting string is too large to fit in the configured memory limit
gmp_export: MemoryError: The resulting string is too large to fit in the configured memory limit
string(3) "255"
