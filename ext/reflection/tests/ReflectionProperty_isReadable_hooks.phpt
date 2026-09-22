--TEST--
Test ReflectionProperty::isReadable() hooks
--FILE--
<?php

class A {
    public $a { get => $this->a; }
    public $b { get => 42; }
    public $c { set => $value; }
    public $d { set {} }
    public $e { get => $this->e; set => $value; }
    public $f { get {} set {} }
}

function test($scope) {
    $rc = new ReflectionClass(A::class);
    foreach ($rc->getProperties() as $rp) {
        echo $rp->getName() . ' from ' . ($scope ?? 'global') . ': ';
        var_dump($rp->isReadable($scope, null));
    }
}

test('A');
test(null);

?>
--EXPECT--
a from A: bool(true)
b from A: bool(true)
c from A: bool(true)
d from A: bool(false)
e from A: bool(true)
f from A: bool(true)
a from global: bool(true)
b from global: bool(true)
c from global: bool(true)
d from global: bool(false)
e from global: bool(true)
f from global: bool(true)
