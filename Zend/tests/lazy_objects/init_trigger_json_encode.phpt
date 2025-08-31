--TEST--
Lazy objects: json_encode initializes object
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

var_dump(json_encode($obj));

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump(json_encode($obj));

--EXPECTF--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(7) "{"a":1}"
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
string(7) "{"a":1}"
