--TEST--
Test ReflectionProperty::isReadable() lazy dynamic
--CREDITS--
Arnaud Le Blanc (arnaud-lb)
--FILE--
<?php

#[AllowDynamicProperties]
class A {
    public $_;

    public function __construct() {
        $this->prop = 1;
    }
}

$rc = new ReflectionClass(A::class);
$obj = $rc->newLazyProxy(fn() => new A());

$rp = new ReflectionProperty(new A, 'prop');
var_dump($rp->isReadable(null, $obj));

?>
--EXPECT--
bool(true)
