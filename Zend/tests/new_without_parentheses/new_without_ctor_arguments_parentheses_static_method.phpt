--TEST--
Immediate static method call on new object without constructor parentheses
--FILE--
<?php

class A
{
    public static function test(): void
    {
        echo 'called';
    }
}

new A::test();

?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "test", expecting variable or "$" in %s on line %d
