--TEST--
Lazy objects: resetAsLazy*() calls destructor of pre-existing object, unless SKIP_DESTRUCTOR flag is used
--FILE--
<?php

class C {
    public readonly int $a;

    public function __construct() {
        $this->a = 1;
    }

    public function __destruct() {
        var_dump(__METHOD__);
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = new C();
print "In makeLazy\n";
$reflector->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
}, ReflectionClass::SKIP_DESTRUCTOR);
print "After makeLazy\n";

var_dump($obj->a);
$obj = null;

print "# Proxy:\n";

$obj = new C();
print "In makeLazy\n";
$reflector->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
}, ReflectionClass::SKIP_DESTRUCTOR);
print "After makeLazy\n";

var_dump($obj->a);
$obj = null;

?>
--EXPECT--
# Ghost:
In makeLazy
After makeLazy
string(11) "initializer"
int(1)
string(13) "C::__destruct"
# Proxy:
In makeLazy
After makeLazy
string(11) "initializer"
int(1)
string(13) "C::__destruct"
