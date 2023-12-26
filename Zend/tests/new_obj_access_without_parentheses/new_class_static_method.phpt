--TEST--
Immediate static method call on new object with ctor parentheses
--FILE--
<?php

class A
{
    public static function test(): void
    {
        echo 'called';
    }
}

new A()::test();

?>
--EXPECT--
called
