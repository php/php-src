--TEST--
GH-20657 002: GC during zend_lazy_object_realize() - reset as lazy during realize()
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
        echo __METHOD__, "\n";
        $reflector->resetAsLazyGhost($obj, function () {});
    }
}

// A cycle, dtor is called on next GC
new D();

$reflector = new ReflectionClass(C::class);

$obj = $reflector->newLazyGhost(new class {
    function __invoke () {}
    function __destruct() {
        gc_collect_cycles();
    }
});

// Add to roots
$obj2 = $obj;
unset($obj2);

// Initialize all props to mark object non-lazy. Also create a cycle.
echo "Will realize lazy object\n";
$reflector->getProperty('a')->setRawValueWithoutLazyInitialization($obj, $obj);
echo "Realized lazy object\n";

var_dump($obj);

?>
--EXPECTF--
Will realize lazy object
D::__destruct
Realized lazy object
lazy ghost object(C)#%d (0) {
}
