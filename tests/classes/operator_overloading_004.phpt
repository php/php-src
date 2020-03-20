--TEST--
Userspace operator overloading - Inheritance test
--FILE--
<?php
abstract class A
{
    public static function __add($lhs, $rhs)
    {
        echo "__add on Class A called\n";
        return 1;
    }

    public static function __sub($lhs, $rhs)
    {
        echo "__sub on Class A called\n";
        return 1;
    }
}

class B extends A {
    public static function __sub($lhs, $rhs)
    {
        echo "__sub on Class B called\n";
        return parent::__sub($lhs, $rhs);
    }
}

$b = new B();
$b + 1;
$b - 1;
--EXPECT--
__add on Class A called
__sub on Class B called
__sub on Class A called
