--TEST--
Lazy objects: serialize during hook initializes object by default
--FILE--
<?php

class C {
    public $a {
        get { return serialize($this); }
        set ($value) { $this->a = $value; }
    }
    public function __construct() {
        var_dump(__METHOD__);
        $this->a = 1;
    }
}

function test(string $name, object $obj) {
    printf("# %s:\n", $name);

    var_dump($obj->a);
}

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(function ($obj) {
    var_dump("initializer");
    $obj->__construct();
});

test('Ghost', $obj);

$obj = $reflector->newLazyProxy(function ($obj) {
    var_dump("initializer");
    return new C();
});

test('Proxy', $obj);

--EXPECT--
# Ghost:
string(11) "initializer"
string(14) "C::__construct"
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
# Proxy:
string(11) "initializer"
string(14) "C::__construct"
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
