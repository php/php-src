--TEST--
Immediate invocation of new object with ctor parentheses created from var
--FILE--
<?php

class A
{
    public function __invoke(): void
    {
        echo 'invoked';
    }
}

$class = A::class;

new $class()();

?>
--EXPECT--
invoked
