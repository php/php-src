--TEST--
Lazy objects: destructor of lazy objets is not called if not initialized
--FILE--
<?php

class C {
    public $a;
    public function __destruct() {
        var_dump(__METHOD__);
    }
}

print "# Ghost:\n";

print "In makeLazy\n";
$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function () {
    var_dump("initializer");
});
print "After makeLazy\n";

// Does not call destructor
$obj = null;

print "# Virtual:\n";

print "In makeLazy\n";
$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function () {
    var_dump("initializer");
});
print "After makeLazy\n";

// Does not call destructor
$obj = null;

--EXPECT--
# Ghost:
In makeLazy
string(13) "C::__destruct"
After makeLazy
# Virtual:
In makeLazy
string(13) "C::__destruct"
After makeLazy
