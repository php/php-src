--TEST--
Interface default methods work with static methods
--FILE--
<?php

interface Interface1
{
    static function method1() { echo static::class; }
}

class Class1
    implements Interface1
{
}

(new Class1())->method1();

?>
--EXPECTF--
Class1

