--TEST--
Test ReflectionProperty::isWritable() visibility
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
        var_dump($rp->isWritable($scope, null));
    }
}

test('A');
test(null);

?>
--EXPECT--
a from A: bool(true)
b from A: bool(false)
c from A: bool(true)
d from A: bool(true)
e from A: bool(true)
f from A: bool(true)
a from global: bool(true)
b from global: bool(false)
c from global: bool(true)
d from global: bool(true)
e from global: bool(true)
f from global: bool(true)
