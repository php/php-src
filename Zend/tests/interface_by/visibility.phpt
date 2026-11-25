--TEST--
Interface delegation: generated method is public
--FILE--

<?php

interface A {
    public function foo(): string;
}

class AImpl implements A {
    public function foo(): string {
        return 'A';
    }
}

class C implements A by $a {
    public function __construct(protected A $a) {}
}

$method = new ReflectionMethod(C::class, 'foo');

var_dump($method->isPublic());
var_dump($method->isAbstract());
var_dump($method->isStatic());
?>

--EXPECT--
bool(true)
bool(false)
bool(false)
