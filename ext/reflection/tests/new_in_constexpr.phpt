--TEST--
Handling of new in constant expressions in reflection
--FILE--
<?php

class Test1 {
    const A = new stdClass;
}

$rc = new ReflectionClass(Test1::class);
$rcc = $rc->getReflectionConstant('A');
var_dump($rcc->getValue() === Test1::A);

function test1() {
    static $x = new stdClass;
    return $x;
}

$rf = new ReflectionFunction('test1');
$s = $rf->getStaticVariables();
var_dump($s['x'] === test1());

function test2($x = new stdClass) {
    return $x;
}

$rf = new ReflectionFunction('test2');
$rp = $rf->getParameters()[0];
// Parameter default should *not* be the same.
var_dump($rp->getDefaultValue() !== test2());

class Test2 {
    public static $prop1 = new stdClass;
    public $prop2 = new stdClass;
}

$rc = new ReflectionClass(Test2::class);
$rp1 = $rc->getProperty('prop1');
$rp2 = $rc->getProperty('prop2');
// For static properties, the value should be the same,
// but the default value shouldn't be.
var_dump($rp1->getValue() === Test2::$prop1);
// TODO: There is a pre-existing bug where the static property
// default value is actually the current value without opcache.
// var_dump($rp1->getDefaultValue() !== Test2::$prop1);
$o = new Test2;
var_dump($rp2->getValue($o) === $o->prop2);
var_dump($rp2->getDefaultValue() !== $o->prop2);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
