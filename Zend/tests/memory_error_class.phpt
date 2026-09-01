--TEST--
MemoryError is a catchable Error
--FILE--
<?php

$e = new MemoryError('boom');
var_dump($e instanceof Error);
var_dump($e instanceof Throwable);
var_dump($e->getMessage());

try {
    throw new MemoryError('caught as Error');
} catch (Error $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
bool(true)
bool(true)
string(4) "boom"
MemoryError: caught as Error
