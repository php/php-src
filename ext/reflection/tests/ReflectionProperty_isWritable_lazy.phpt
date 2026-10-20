--TEST--
Test ReflectionProperty::isWritable() lazy
--CREDITS--
Arnaud Le Blanc (arnaud-lb)
--FILE--
<?php

class A {
    public public(set) readonly int $a;
    public public(set) readonly int $b;

    public function __construct() {
        $this->a = 1;
    }
}

$rc = new ReflectionClass(A::class);
$obj = $rc->newLazyProxy(fn() => new A());

$rp = new ReflectionProperty(A::class, 'a');
var_dump($rp->isWritable(null, $obj));

$rp = new ReflectionProperty(A::class, 'b');
var_dump($rp->isWritable(null, $obj));

?>
--EXPECT--
bool(false)
bool(true)
