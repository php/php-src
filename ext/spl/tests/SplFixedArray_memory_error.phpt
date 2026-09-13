--TEST--
SplFixedArray throws a catchable MemoryError when the requested size cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

try {
    new SplFixedArray(2000000000);
} catch (MemoryError $e) {
    echo 'construct: ' . $e::class . ': ' . $e->getMessage() . "\n";
}

$a = new SplFixedArray(2);
$a[0] = 'kept';
try {
    $a->setSize(2000000000);
} catch (MemoryError $e) {
    echo 'setSize: ' . $e::class . ': ' . $e->getMessage() . "\n";
}
var_dump($a->getSize());
var_dump($a[0]);

?>
--EXPECT--
construct: MemoryError: The resulting SplFixedArray is too large to fit in the configured memory limit
setSize: MemoryError: The resulting SplFixedArray is too large to fit in the configured memory limit
int(2)
string(4) "kept"
