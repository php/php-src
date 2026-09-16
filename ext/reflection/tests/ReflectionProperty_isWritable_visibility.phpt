--TEST--
Test ReflectionProperty::isWritable() visibility
--FILE--
<?php

class A {}

class B extends A {
    public $a;
    protected $b;
    private $c;
    public private(set) int $d;
    public protected(set) int $e;
    public readonly int $f;
}

class C extends B {}

class D extends C {
    public function __set($name, $value) {}
}

function test($scope) {
    $rc = new ReflectionClass(B::class);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from ' . ($scope ?? 'global') . ': ';
        var_dump($rp->isWritable($scope, $scope && $scope !== 'A' ? new $scope : null));
    }
}

test('A');
test('B');
test('C');
test('D');
test(null);

?>
--EXPECT--
a from A: bool(true)
b from A: bool(true)
c from A: bool(false)
d from A: bool(false)
e from A: bool(true)
f from A: bool(true)
a from B: bool(true)
b from B: bool(true)
c from B: bool(true)
d from B: bool(true)
e from B: bool(true)
f from B: bool(true)
a from C: bool(true)
b from C: bool(true)
c from C: bool(false)
d from C: bool(false)
e from C: bool(true)
f from C: bool(true)
a from D: bool(true)
b from D: bool(true)
c from D: bool(true)
d from D: bool(false)
e from D: bool(true)
f from D: bool(true)
a from global: bool(true)
b from global: bool(false)
c from global: bool(false)
d from global: bool(false)
e from global: bool(false)
f from global: bool(false)
