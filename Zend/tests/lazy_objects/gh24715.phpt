--TEST--
GH-24715: Object reset as lazy by a destructor triggered when releasing the initializer
--FILE--
<?php

class C {
    public $a;
}

class D {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        global $obj, $reflector;
        try {
            $reflector->resetAsLazyGhost($obj, function () {});
        } catch (Error $e) {
            printf("%s: %s\n", $e::class, $e->getMessage());
        }
    }
}

new D();

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyGhost(new class {
    public function __invoke($obj) {
        $obj->a = 1;
    }
    public function __destruct() {
        gc_collect_cycles();
    }
});

var_dump(serialize($obj));
var_dump($reflector->isUninitializedLazyObject($obj));

?>
--EXPECT--
Error: Can not reset an object while it is being initialized
string(24) "O:1:"C":1:{s:1:"a";i:1;}"
bool(false)
