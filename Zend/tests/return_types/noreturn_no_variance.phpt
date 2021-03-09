--TEST--
noreturn return type: prevent unacceptable cases
--FILE--
<?php

class A
{
    public function bar(): noreturn
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
Fatal error: Declaration of B::bar(): string must be compatible with A::bar(): noreturn in %s on line %d
