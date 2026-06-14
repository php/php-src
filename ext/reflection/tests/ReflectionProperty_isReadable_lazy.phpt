--TEST--
Test ReflectionProperty::isReadable() lazy
--CREDITS--
Arnaud Le Blanc (arnaud-lb)
--FILE--
<?php

class A {
    public int $a;
    public int $b;

    public function __construct() {
        $this->a = 1;
    }
}

$rc = new ReflectionClass(A::class);
$obj = $rc->newLazyProxy(fn() => new A());

$rp = new ReflectionProperty(A::class, 'a');
var_dump($rp->isReadable(null, $obj));

$rp = new ReflectionProperty(A::class, 'b');
var_dump($rp->isReadable(null, $obj));

?>
--EXPECT--
bool(true)
bool(false)
