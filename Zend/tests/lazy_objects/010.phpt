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

print "# Ghost:\n";

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

var_dump(json_encode($obj));

print "# Virtual:\n";

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump(json_encode($obj));

--EXPECTF--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(7) "{"a":1}"
# Virtual:
string(11) "initializer"
string(14) "C::__construct"
string(7) "{"a":1}"
