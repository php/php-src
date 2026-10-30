--TEST--
Lazy objects: destructor of lazy objects is not called if not initialized
--FILE--
<?php

class C {
    public $a;
    public function __destruct() {
        var_dump(__METHOD__);
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

print "In newLazyGhost\n";
$obj = $reflector->newLazyGhost(function () {
    var_dump("initializer");
});
print "After newLazyGhost\n";

// Does not call destructor
$obj = null;

print "# Proxy:\n";

print "In newLazyProxy\n";
$obj = $reflector->newLazyProxy(function () {
    var_dump("initializer");
});
print "After newLazyGhost\n";

// Does not call destructor
$obj = null;

--EXPECT--
# Ghost:
In newLazyGhost
After newLazyGhost
# Proxy:
In newLazyProxy
After newLazyGhost
