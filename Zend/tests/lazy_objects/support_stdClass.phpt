--TEST--
Lazy objects: stdClass can be initialized lazily
--FILE--
<?php

$reflector = new ReflectionClass(stdClass::class);

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
object(stdClass)#%d (0) {
}
# Proxy:
object(stdClass)#%d (0) {
}
