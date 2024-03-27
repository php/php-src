--TEST--
Immediate invocation of new object with ctor parentheses created from expr
--FILE--
<?php

class A
{
    public function __invoke(): void
    {
        echo 'invoked';
    }
}

new (trim(' A '))()();

?>
--EXPECT--
invoked
