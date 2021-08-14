--TEST--
never type: non-explicit widening disallowed
--FILE--
<?php

class A
{
    public function someReturningStaticMethod(never $var)
    {
    }
}

class B extends A
{
    public function someReturningStaticMethod($var)
    {
        throw new UnexpectedValueException('child');
    }
}
?>
--EXPECTF--
Fatal error: Declaration of B::someReturningStaticMethod($var) must be compatible with A::someReturningStaticMethod(never $var) in %s on line %d