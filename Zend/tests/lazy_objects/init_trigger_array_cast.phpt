--TEST--
Lazy objects: array cast does not initialize object
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

var_dump((array)$obj);

$obj->a = 2;
var_dump((array)$obj);

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump((array)$obj);

$obj->a = 2;
var_dump((array)$obj);

--EXPECTF--
# Ghost:
array(0) {
}
string(11) "initializer"
string(14) "C::__construct"
array(1) {
  ["a"]=>
  int(2)
}
# Proxy:
array(0) {
}
string(11) "initializer"
string(14) "C::__construct"
array(1) {
  ["a"]=>
  int(2)
}
