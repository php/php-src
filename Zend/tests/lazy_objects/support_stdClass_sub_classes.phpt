--TEST--
Lazy objects: sub-classes of stdClass can be initialized lazily
--FILE--
<?php

class C extends stdClass {
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});
var_dump($obj);

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});
var_dump($obj);

--EXPECTF--
# Ghost:
object(C)#%d (0) {
}
# Proxy:
object(C)#%d (0) {
}
