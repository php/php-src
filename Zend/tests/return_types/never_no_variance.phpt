--TEST--
never return type: prevent unacceptable cases
--FILE--
<?php

class A
{
    public function bar(): never
    {
        throw new \Exception('parent');
    }
}

class B extends A
{
    public function bar(): string
    {
        return "hello";
    }
}

(new B)->bar();

?>
--EXPECTF--
Fatal error: Declaration of B::bar(): string must be compatible with A::bar(): never in %s on line %d
