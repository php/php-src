--TEST--
Immediate static method call on new object with ctor parentheses created from expr
--FILE--
<?php

class A
{
    public static function test(): void
    {
        echo 'called';
    }
}

new (trim(' A '))()::test();

?>
--EXPECT--
called
