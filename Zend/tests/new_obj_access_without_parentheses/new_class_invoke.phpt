--TEST--
Immediate invocation of new object with ctor parentheses
--FILE--
<?php

class A
{
    public function __invoke(): void
    {
        echo 'invoked';
    }
}

new A()();

?>
--EXPECT--
invoked
