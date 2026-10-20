--TEST--
Iterating an uninitialized RecursiveIteratorIterator
--FILE--
<?php

$rc = new ReflectionClass(RecursiveIteratorIterator::class);
$it = $rc->newInstanceWithoutConstructor();
try {
    foreach ($it as $v) {}
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Object is not initialized
