--TEST--
Lazy objects: get_object_vars initializes object
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

var_dump(get_object_vars($obj));

$obj->a = 2;
var_dump(get_object_vars($obj));

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

var_dump(get_object_vars($obj));

$obj->a = 2;
var_dump(get_object_vars($obj));

--EXPECT--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
array(1) {
  ["a"]=>
  int(1)
}
array(1) {
  ["a"]=>
  int(2)
}
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
array(1) {
  ["a"]=>
  int(1)
}
array(1) {
  ["a"]=>
  int(2)
}
