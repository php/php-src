--TEST--
Test ReflectionProperty::isReadable() visibility
--FILE--
<?php

class A {}

class B extends A {
    public $a;
    protected $b;
    private $c;
    public protected(set) int $d = 42;
}

class C extends B {}

class D extends C {
    public function __get($name) {}
}

class E extends D {
    private $f;

    public function __isset($name) {
        return $name === 'f';
    }
}

function test($scope) {
    $rc = new ReflectionClass(B::class);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from ' . ($scope ?? 'global') . ': ';
        var_dump($rp->isReadable($scope, $scope && $scope !== 'A' ? new $scope : null));
    }
}

foreach (['A', 'B', 'C', 'D', 'E'] as $scope) {
    test($scope);
}
test(null);

?>
--EXPECT--
a from A: bool(true)
b from A: bool(true)
c from A: bool(false)
d from A: bool(true)
a from B: bool(true)
b from B: bool(true)
c from B: bool(true)
d from B: bool(true)
a from C: bool(true)
b from C: bool(true)
c from C: bool(false)
d from C: bool(true)
a from D: bool(true)
b from D: bool(true)
c from D: bool(true)
d from D: bool(true)
a from E: bool(true)
b from E: bool(true)
c from E: bool(false)
d from E: bool(true)
a from global: bool(true)
b from global: bool(false)
c from global: bool(false)
d from global: bool(true)
