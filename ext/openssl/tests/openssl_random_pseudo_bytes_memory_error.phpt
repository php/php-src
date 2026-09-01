--TEST--
openssl_random_pseudo_bytes() throws a catchable MemoryError when the length cannot fit in the memory limit
--EXTENSIONS--
openssl
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    openssl_random_pseudo_bytes(2147483000);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(strlen(openssl_random_pseudo_bytes(16)));

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
int(16)
