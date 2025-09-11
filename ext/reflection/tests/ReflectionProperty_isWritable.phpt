--TEST--
Test ReflectionProperty::isWritable()
--FILE--
<?php

class A {
    public $a;
    protected $b;
    private $c;
    public protected(set) int $d;
    public $e { get => 42; }
    public $f { set {} }
    public readonly int $g;
    public private(set) int $h;

    public function setG($g) {
        $this->g = $g;
    }

    public function __clone() {
        $rp = new ReflectionProperty(A::class, 'g');
        echo $rp->getName() . ' from static (initialized, clone): ';
        var_dump($rp->isWritable($this));
    }
}

$test = static function ($scope) {
    $rc = new ReflectionClass(A::class);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from ' . ($scope ?? 'global') . ': ';
        var_dump($rp->isWritable(new A(), $scope));

        if ($rp->name == 'g') {
            $a = new A();
            $a->setG(42);
            echo $rp->getName() . ' from ' . ($scope ?? 'global') . ' (initialized): ';
            var_dump($rp->isWritable($a, $scope));
            clone $a;
        }
    }
};

$test('A');
$test->bindTo(null, 'A')('static');

$test(null);
$test->bindTo(null, null)('static');

?>
--EXPECT--
a from A: bool(true)
b from A: bool(true)
c from A: bool(true)
d from A: bool(true)
e from A: bool(false)
f from A: bool(true)
g from A: bool(true)
g from A (initialized): bool(false)
g from static (initialized, clone): bool(true)
h from A: bool(true)
a from static: bool(true)
b from static: bool(true)
c from static: bool(true)
d from static: bool(true)
e from static: bool(false)
f from static: bool(true)
g from static: bool(true)
g from static (initialized): bool(false)
g from static (initialized, clone): bool(true)
h from static: bool(true)
a from global: bool(true)
b from global: bool(false)
c from global: bool(false)
d from global: bool(false)
e from global: bool(false)
f from global: bool(true)
g from global: bool(false)
g from global (initialized): bool(false)
g from static (initialized, clone): bool(true)
h from global: bool(false)
a from static: bool(true)
b from static: bool(false)
c from static: bool(false)
d from static: bool(false)
e from static: bool(false)
f from static: bool(true)
g from static: bool(false)
g from static (initialized): bool(false)
g from static (initialized, clone): bool(true)
h from static: bool(false)
