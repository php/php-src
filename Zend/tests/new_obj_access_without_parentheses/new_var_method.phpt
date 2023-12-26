--TEST--
Immediate method call on new object with ctor parentheses created from var
--FILE--
<?php

class A
{
    public function test(): void
    {
        echo 'called';
    }
}

$class = A::class;

new $class()->test();

?>
--EXPECT--
called
