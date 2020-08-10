--TEST--
The default value is a constant in the parent class method's signature.
--FILE--
<?php

use Foo\Bar;

class A
{
    public function foo(
        $param1 = \Foo\Bar::CONSTANT,
        $param2 = Foo\Bar::CONSTANT,
        $param3 = Bar::CONSTANT
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
Fatal error: Declaration of B::foo() must be compatible with A::foo($param1 = Foo\Bar::CONSTANT, $param2 = Foo\Bar::CONSTANT, $param3 = Foo\Bar::CONSTANT) in %s on line %d
