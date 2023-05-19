--TEST--
Lazy objects: newInstanceLazy can not instantiate sub-class of internal classes
--FILE--
<?php

class C extends ReflectionClass {}

foreach (['resetAsLazyGhost', 'resetAsLazyProxy'] as $strategy) {
    $reflector = new ReflectionClass(C::class);
    $obj = $reflector->newInstanceWithoutConstructor();
    try {
        $reflector->$strategy($obj, function ($obj) {
            var_dump("initializer");
        });
    } catch (Error $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
}
--EXPECT--
Error: Cannot make instance of internal class lazy: C inherits internal class ReflectionClass
Error: Cannot make instance of internal class lazy: C inherits internal class ReflectionClass
