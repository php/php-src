--TEST--
Lazy objects: newInstanceLazy can not instantiate internal classes
--FILE--
<?php

$reflector = new ReflectionClass(ReflectionClass::class);
$obj = $reflector->newInstanceWithoutConstructor();

foreach (['resetAsLazyGhost', 'resetAsLazyProxy'] as $strategy) {
    try {
        $reflector->$strategy($obj, function ($obj) {
            var_dump("initializer");
        });
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
}
--EXPECT--
Error: Cannot make instance of internal class lazy: ReflectionClass is internal
Error: Cannot make instance of internal class lazy: ReflectionClass is internal
