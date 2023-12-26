--TEST--
Immediate static method call on new object with ctor parentheses created from var
--FILE--
<?php

class A
{
    public static function test(): void
    {
        echo 'called';
    }
}

$class = A::class;

new $class()::test();

?>
--EXPECT--
called
