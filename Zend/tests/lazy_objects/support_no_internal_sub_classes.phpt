--TEST--
Lazy objects: sub-classes of internal classes can not be initialized lazily
--FILE--
<?php

class C extends DateTime {
}

$reflector = new ReflectionClass(C::class);

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
Error: Cannot make instance of internal class lazy: C inherits internal class DateTime
# Proxy:
Error: Cannot make instance of internal class lazy: C inherits internal class DateTime
