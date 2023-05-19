--TEST--
Lazy objects: internal classes can not be initialized lazily
--FILE--
<?php

print "# Ghost:\n";

$obj = (new ReflectionClass(DateTime::class))->newInstanceWithoutConstructor();
try {
    (new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
        var_dump("initializer");
        $obj->__construct();
    });
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

print "# Virtual:\n";

$obj = (new ReflectionClass(DateTime::class))->newInstanceWithoutConstructor();
try {
    (new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
        var_dump("initializer");
        $obj->__construct();
    });
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

--EXPECTF--
# Ghost:
Error: Cannot make instance of internal class lazy: DateTime is internal
# Virtual:
Error: Cannot make instance of internal class lazy: DateTime is internal
