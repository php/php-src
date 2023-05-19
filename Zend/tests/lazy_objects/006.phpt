--TEST--
Lazy objects: Foreach initializes object
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

foreach ($obj as $prop => $value) {
    var_dump($prop, $value);
}

print "# Virtual:\n";

$obj = (new ReflectionClass(C::class))->newInstanceWithoutConstructor();
(new ReflectionClass($obj))->resetAsLazyProxy($obj, function ($obj) {
    var_dump("initializer");
    return new C();
});

foreach ($obj as $prop => $value) {
    var_dump($prop, $value);
}

--EXPECTF--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
# Virtual:
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
