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

$reflector = new ReflectionClass(C::class);

print "# Ghost:\n";

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

foreach ($obj as $prop => $value) {
    var_dump($prop, $value);
}

print "# Proxy:\n";

$obj = $reflector->newLazyProxy(function ($obj) {
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
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
string(1) "a"
int(1)
