--TEST--
Lazy objects: comparison initializes object
--FILE--
<?php

class C {
    public int $a;
    public $b;

    public function __construct() {
        var_dump(__METHOD__);
    }

    public function __toString() {
        return 'C';
    }
}

$reflector = new ReflectionClass(C::class);

$a = $reflector->newLazyGhost(function ($obj) {
    $obj->__construct();
});

$b = $reflector->newLazyProxy(function ($obj) {
    return new C();
});

var_dump($a > $b);

$a = $reflector->newLazyGhost(function ($obj) {
    $obj->__construct();
});

$b = $reflector->newLazyProxy(function ($obj) {
    return new C();
});

var_dump($a == $b);

$a = $reflector->newLazyGhost(function ($obj) {
    $obj->__construct();
});

var_dump('A' < $a);
?>
--EXPECT--
string(14) "C::__construct"
string(14) "C::__construct"
bool(false)
string(14) "C::__construct"
string(14) "C::__construct"
bool(true)
bool(true)
