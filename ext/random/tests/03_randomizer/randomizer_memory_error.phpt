--TEST--
Random\Randomizer::getBytes() and getBytesFromString() throw a catchable MemoryError
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$randomizer = new Random\Randomizer();

try {
    $randomizer->getBytes(PHP_INT_MAX);
} catch (MemoryError $e) {
    echo 'getBytes: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

try {
    $randomizer->getBytesFromString('abc', PHP_INT_MAX);
} catch (MemoryError $e) {
    echo 'getBytesFromString: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(strlen($randomizer->getBytes(16)));
var_dump(strlen($randomizer->getBytesFromString('abc', 16)));

?>
--EXPECT--
getBytes: MemoryError: The resulting string is too large to fit in the configured memory limit
getBytesFromString: MemoryError: The resulting string is too large to fit in the configured memory limit
int(16)
int(16)
