--TEST--
Bug 001
--FILE--
<?php

abstract class A {
    abstract public $x { get; }
}

class C extends A {
    private $_x;
    public $x {
        get => $this->_x;
    }
}

var_dump((new ReflectionProperty(C::class, 'x'))->isVirtual());

$c = new C;

try {
    $c->x = 3;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(true)
Property C::$x is read-only
