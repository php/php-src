--TEST--
Lazy objects: serialize initializes object
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

var_dump(serialize($obj));

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump(serialize($obj));


--EXPECTF--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
