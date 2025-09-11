--TEST--
Cannot use instance of Directory class constructed via Reflection.
--FILE--
<?php

$rc = new ReflectionClass("Directory");
var_dump($rc->isInstantiable());
try {
    $d = $rc->newInstanceWithoutConstructor();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(false)
ReflectionException: Class Directory is an internal class that cannot be instantiated manually
