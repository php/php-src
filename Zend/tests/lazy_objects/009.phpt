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

print "# Ghost:\n";

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

var_dump(serialize($obj));

print "# Virtual:\n";

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump(serialize($obj));


--EXPECTF--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
# Virtual:
string(11) "initializer"
string(14) "C::__construct"
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
