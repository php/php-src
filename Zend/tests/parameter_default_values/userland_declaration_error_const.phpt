--TEST--
The default value is a constant in the parent class method's signature.
--FILE--
<?php

use const Foo\CONSTANT;

class A
{
    public function foo(
        $param1 = \Foo\CONSTANT,
        $param2 = Foo\CONSTANT,
        $param3 = CONSTANT
    ) {
    }
}

class B extends A
{
    public function foo()
    {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of B::foo() must be compatible with A::foo($param1 = Foo\CONSTANT, $param2 = Foo\CONSTANT, $param3 = Foo\CONSTANT) in %s on line %d
