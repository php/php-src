--TEST--
GH-25264: Implementing an interface must not modify inherited property hooks
--FILE--
<?php

class P {
    public $prop { get => 1; }
}

class Q {
    public $prop { get => 2; set {} }
}

interface I {
    public $prop { get; set; }
}

interface J {
    public $prop { get; }
}

abstract class A extends P implements I {}

class B extends Q implements I {}

abstract class C implements J, I {}

class D implements J {
    public $prop { get => 3; }
}

var_dump((new ReflectionProperty(P::class, 'prop'))->hasHook(PropertyHookType::Set));
var_dump((new ReflectionProperty(J::class, 'prop'))->hasHook(PropertyHookType::Set));
var_dump((new ReflectionProperty(A::class, 'prop'))->hasHook(PropertyHookType::Set));

$p = new P;
try {
    $p->prop = 4;
} catch (Error $e) {
    echo $e::class, "\n";
}

var_dump((new B)->prop);
var_dump((new D)->prop);

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
Error
int(2)
int(3)
