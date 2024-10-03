--TEST--
Test ReflectionProperty::isReadable()
--FILE--
<?php

class A {}

class B extends A {
    public $a;
    protected $b;
    private $c;
    public protected(set) int $d;
    public $e { get => 42; }
    public $f { set {} }
}

class C extends B {}

$test = static function ($scope) {
    $rc = new ReflectionClass(B::class);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from ' . ($scope ?? 'global') . ': ';
        var_dump($rp->isReadable($scope));
    }
};

foreach (['A', 'B', 'C'] as $scope) {
    $test($scope);
    $test->bindTo(null, $scope)('static');
}

$test(null);
$test->bindTo(null, null)('static');

?>
--EXPECT--
a from A: bool(true)
b from A: bool(true)
c from A: bool(false)
d from A: bool(true)
e from A: bool(true)
f from A: bool(false)
a from static: bool(true)
b from static: bool(true)
c from static: bool(false)
d from static: bool(true)
e from static: bool(true)
f from static: bool(false)
a from B: bool(true)
b from B: bool(true)
c from B: bool(true)
d from B: bool(true)
e from B: bool(true)
f from B: bool(false)
a from static: bool(true)
b from static: bool(true)
c from static: bool(true)
d from static: bool(true)
e from static: bool(true)
f from static: bool(false)
a from C: bool(true)
b from C: bool(true)
c from C: bool(false)
d from C: bool(true)
e from C: bool(true)
f from C: bool(false)
a from static: bool(true)
b from static: bool(true)
c from static: bool(false)
d from static: bool(true)
e from static: bool(true)
f from static: bool(false)
a from global: bool(true)
b from global: bool(false)
c from global: bool(false)
d from global: bool(true)
e from global: bool(true)
f from global: bool(false)
a from static: bool(true)
b from static: bool(false)
c from static: bool(false)
d from static: bool(true)
e from static: bool(true)
f from static: bool(false)
