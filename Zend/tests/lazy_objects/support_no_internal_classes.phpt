--TEST--
Lazy objects: internal classes can not be initialized lazily
--FILE--
<?php

$reflector = new ReflectionClass(DateTime::class);

print "# Ghost:\n";

try {
    $obj = $reflector->newLazyGhost(function ($obj) {
        var_dump("initializer");
        $obj->__construct();
    });
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

print "# Proxy:\n";

try {
    $obj = $reflector->newLazyProxy(function ($obj) {
        var_dump("initializer");
        $obj->__construct();
    });
} catch (Error $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

--EXPECTF--
# Ghost:
Error: Cannot make instance of internal class lazy: DateTime is internal
# Proxy:
Error: Cannot make instance of internal class lazy: DateTime is internal
