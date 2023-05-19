--TEST--
Lazy objects: get_mangled_object_vars does not initialize object
--FILE--
<?php

class C {
    public int $a;
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump(get_mangled_object_vars($obj));

    $obj->a = 2;
    var_dump(get_mangled_object_vars($obj));
}

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyGhost($obj, function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Virtual', $obj);
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
# Virtual:
array(0) {
}
string(11) "initializer"
string(14) "C::__construct"
array(1) {
  ["a"]=>
  int(2)
}
