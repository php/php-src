--TEST--
Lazy objects: var_export initializes object
--FILE--
<?php

class C {
    public int $a;
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
    }
}

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

var_export($obj);
print "\n";

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

var_export($obj);
print "\n";
--EXPECTF--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
\C::__set_state(array(
   'a' => 1,
))
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
\C::__set_state(array(
   'a' => 1,
))
